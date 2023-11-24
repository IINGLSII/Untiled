// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/Tile_Manager.h"

ATile_Manager::ATile_Manager()
{
	ReadData();
}

void ATile_Manager::BeginPlay() {
	GridArray.Init(nullptr, GRID_DIM * GRID_DIM);
	ParentGeneratorInformation.setZero();
	// GRID_DIMension must be odd (has a center tile)
	verify(GRID_DIM % 2 != 0);
}


bool ATile_Manager::ValidateOffset(FOffset offset) const
{
	return (offset.Row >= -GRID_DIM / 2 && offset.Row <= GRID_DIM / 2) && (offset.Col >= -GRID_DIM / 2 && offset.Col <= GRID_DIM / 2);
}


int ATile_Manager::ConvertFOffsetoIdx(FOffset offset) const
{
	return ((-offset.Row + GRID_DIM/2) * GRID_DIM) + (offset.Col + GRID_DIM/2);
}

FOffset ATile_Manager::ConvertIdxToOffset(int idx) const 
{
	return FOffset(-(idx / GRID_DIM) + GRID_DIM / 2, (idx % GRID_DIM) - GRID_DIM / 2);
}

ATile* ATile_Manager::GetTile(FOffset offset) const
{
	return GetTile(offset, GridArray);
}

ATile* ATile_Manager::GetTileByIndex(uint32 idx) const
{
	return GetTile(ConvertIdxToOffset(idx), GridArray);
}

ATile* ATile_Manager::GetTile (FOffset offset, const TArray<ATile*>& arr) const
{
	// check if row/column offsets fall within GRID_DIMension of array (GRID_DIM)
	verify(ATile_Manager::ValidateOffset(offset));
	
	// index and return contents of array at indexed position
	int pos = ATile_Manager::ConvertFOffsetoIdx(offset);
	return arr[pos];
}

void ATile_Manager::SetTile(FOffset offset, ATile* tile, TArray<ATile*>& arr)
{
	// check if row/column fall within GRID_DIMension of array (GRID_DIM)
	verify(ATile_Manager::ValidateOffset(offset));

	// set contents of array at indexed position
	int pos = ATile_Manager::ConvertFOffsetoIdx(offset);
	arr[pos] = tile;
}


void ATile_Manager::SpawnTile(FOffset tile_offset)
{
	// get reference to center
	ATile* center = ATile_Manager::GetTile(FOffset(0,0), GridArray);
	FVector location_offset = FVector(tile_offset.Row * TILE_SIZE, tile_offset.Col * TILE_SIZE, 0);
	FTransform spawn_transform;
	// determine if center exists
	if (center != nullptr)
		spawn_transform.SetLocation(center->GetActorLocation() + location_offset);
	else
		spawn_transform.SetLocation(this->GetActorLocation() + location_offset);

	// spawn actor, adjust size variable, begin calling beginplay, and then set tile reference in grid array
	ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(TileBp, spawn_transform);
	ATile_Manager::GenerateTile(tile);
	tile->FinishSpawning(spawn_transform);
	ATile_Manager::SetTile( tile_offset, tile, GridArray);
}

void ATile_Manager::UpdateGrid(FVector location) 
{

	ATile* center_tile = ATile_Manager::GetTile(FOffset(0, 0), GridArray);
	
	verify(center_tile != nullptr);

	FOffset tile_offset = center_tile->GetPlayerOffset(location);

	// check if player has moved from center tile (0,0)
	if (abs(tile_offset.Row) + abs(tile_offset.Col) != 0) {

		ParentGeneratorInformation = GetTile(tile_offset)->GetTileData();

		// create empty grid array
		TArray<ATile*> temp_arr;
		temp_arr.Init(nullptr, GRID_DIM*GRID_DIM);

		// iterate through every tile to shift to new pos
		for (int32 Index = 0; Index != GridArray.Num(); ++Index)
		{
			// new offset of tile after shifting array
			FOffset offset = ATile_Manager::ConvertIdxToOffset(Index);
			FOffset forward_offset = FOffset(offset.Row - tile_offset.Row, offset.Col - tile_offset.Col);
			FOffset backward_offset = FOffset(offset.Row + tile_offset.Row, offset.Col + tile_offset.Col);
				
			// if tiles shifted location is within GRID_DIM bounds, set shifted pos of temp array to unshifted pos in GridArray
			if (ATile_Manager::ValidateOffset(forward_offset))
				ATile_Manager::SetTile(forward_offset, ATile_Manager::GetTile(offset, GridArray), temp_arr);
			else
				GetTile(offset, GridArray)->Destroy();
		}
		// set contents of grid array to temp array
		GridArray = temp_arr;

		//spawn tiles for null positions of array (newly discovered tiles)
		ATile_Manager::PopulateGrid(false);
	}
}

void ATile_Manager::PopulateGrid(bool flush_array)
{
	// iterate through all tiles
	for (int32 Index = 0; Index != GridArray.Num(); ++Index)
	{
		// if tile is null, always spawn tile at location
		FOffset offset = ATile_Manager::ConvertIdxToOffset(Index);
		ATile* tile = ATile_Manager::GetTile(offset);
		if (tile == nullptr)
			ATile_Manager::SpawnTile(offset);

		// otherwise only spawn and replace tiles if 'flush' parameter is true
		else if (flush_array) {
			tile->Destroy();
			ATile_Manager::SpawnTile(offset);
		}
	}
}

void ATile_Manager::PNormalizeVector(Eigen::VectorXf* vector) {
	*vector /= vector->sum();
}

void ATile_Manager::GenerateTile(ATile* tile)
{
	// vector for tile state information
	tile_data_t data;
	data.setZero();

	// current for state information of current tile
	Eigen::VectorXf ENV(6);
	Eigen::VectorXf WEATHER(4);
	Eigen::VectorXf DIFF(5);
	Eigen::VectorXf LOOT(3);

	Eigen::VectorXf t1;
	Eigen::VectorXf t2;
	Eigen::VectorXf t3;

	ENV.setZero();
	WEATHER.setZero();
	DIFF.setZero();
	LOOT.setZero();

	ENV[ParentGeneratorInformation[0]] = 1;
	t1.resize(ENV.size());
	t1 = ENVxPREV_ENV_M * ENV;
	data[0] = MakeRandomSelection(&t1);
	ENV.setZero();
	ENV[data[0]] = 1;

	WEATHER[ParentGeneratorInformation[1]] = 1;
	t1.resize(WEATHER.size());
	t2.resize(WEATHER.size());
	t1 = WEATHERxPREV_WEATHER_M * WEATHER;
	t2 = WEATHERxENV_M * ENV;
	t1 = t1.cwiseProduct(t2);
	PNormalizeVector(&t1);
	data[1] = MakeRandomSelection(&t1);
	WEATHER.setZero();
	WEATHER[data[1]] = 1;

	DIFF[ParentGeneratorInformation[2]] = 1;
	t1.resize(DIFF.size());
	t2.resize(DIFF.size());
	t3.resize(DIFF.size());
	t1 = DIFFxPREV_DIFF_M * DIFF;
	t2 = DIFFxENV_M * ENV;
	t3 = DIFFxWEATHER_M * WEATHER;
	t1 = t1.cwiseProduct(t2);
	t1 = t1.cwiseProduct(t3);
	PNormalizeVector(&t1);
	data[2] = MakeRandomSelection(&t1);
	DIFF.setZero();
	DIFF[data[2]] = 1;

	LOOT[ParentGeneratorInformation[3]] = 1;
	t1.resize(LOOT.size());
	t2.resize(LOOT.size());
	t1 = LOOTxPREV_LOOT_M * LOOT;
	t2 = LOOTxDIFF_M * DIFF;
	t1 = t1.cwiseProduct(t2);
	PNormalizeVector(&t1);
	data[3] = MakeRandomSelection(&t1);
	LOOT.setZero();
	LOOT[data[3]] = 1;

	tile->Init(data, TILE_SIZE);
	return;
}

void ATile_Manager::InitMatrixFromData(Eigen::MatrixXf* matrix, const TCHAR* utable_ref, int num_row, int num_col) {
	// get data table object
	ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(utable_ref);
	UDataTable* DataTable;
	if (DataTableFinder.Succeeded())
		DataTable = DataTableFinder.Object;
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("COULD NOT GET DATA TABLE")));
		return;
	}

	// initialize matrix and store information inside data table
	matrix->resize(num_row, num_col);
	ConvertDataTableToMatrix(matrix, DataTable);
	DataTableFinder.UnregisterGCObject();
	return;
}

void ATile_Manager::ReadData() {
	InitMatrixFromData(&ENVxPREV_ENV_M, TEXT("DataTable'/Game/TileSystem/Data/P_Environment__Environment.P_Environment__Environment'"), 6, 6);
	InitMatrixFromData(&WEATHERxPREV_WEATHER_M, TEXT("DataTable'/Game/TileSystem/Data/P_Weather__Weather.P_Weather__Weather'"), 4, 4);
	InitMatrixFromData(&WEATHERxENV_M, TEXT("DataTable'/Game/TileSystem/Data/Environment__Weather.Environment__Weather'"), 4, 6);
	InitMatrixFromData(&DIFFxPREV_DIFF_M, TEXT("DataTable'/Game/TileSystem/Data/P_Difficulty__Difficulty.P_Difficulty__Difficulty'"), 5, 5);
	InitMatrixFromData(&DIFFxENV_M, TEXT("DataTable'/Game/TileSystem/Data/Environment__Difficulty.Environment__Difficulty'"), 5, 6);
	InitMatrixFromData(&DIFFxWEATHER_M, TEXT("DataTable'/Game/TileSystem/Data/Weather__Difficulty.Weather__Difficulty'"), 5, 4);
	InitMatrixFromData(&LOOTxPREV_LOOT_M, TEXT("DataTable'/Game/TileSystem/Data/P_LootCount__LootCount.P_LootCount__LootCount'"), 3, 3);
	InitMatrixFromData(&LOOTxDIFF_M, TEXT("DataTable'/Game/TileSystem/Data/Difficulty__LootCount.Difficulty__LootCount'"), 3, 5);
}

void ATile_Manager::ConvertDataTableToMatrix(Eigen::MatrixXf* M, UDataTable* T) {
	M->setZero();

	FString ContextString;
	TArray<FName> RowNames = T->GetRowNames();

	// iterate through rows inside data table and store float array information inside matrix
	int row_index = 0;
	for (auto& name : RowNames)
	{
		FRow* row = T->FindRow<FRow>(name, ContextString);
		int col_index = 0;
		for (auto& value : row->Vector){
			(*M)(row_index, col_index) = value;
			col_index++;
		}
		row_index++;
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SUCCESSFULLY READ UTABLE")));
	return;
}


int ATile_Manager::MakeRandomSelection(Eigen::VectorXf* probability_vector) {
	float value = FMath::FRandRange(0.0f, 1.0f);

	int selection;
	for (selection = 0; selection < probability_vector->size(); selection++) {

		if (value < (*probability_vector)[selection]) {
			return selection;
		}
		value -=  (*probability_vector)[selection];
	}
	return 0;
}
