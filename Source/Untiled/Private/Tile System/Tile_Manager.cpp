// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/Tile_Manager.h"

ATile_Manager::ATile_Manager()
{
	read_data();
}

void ATile_Manager::BeginPlay() {
	grid_array.Init(nullptr, GRID_DIM * GRID_DIM);
	parent_generator_information.setZero();
	// GRID_DIMension must be odd (has a center tile)
	verify(GRID_DIM % 2 != 0);
}


bool ATile_Manager::validate_offset(FOffset offset) const
{
	return (offset.row >= -GRID_DIM / 2 && offset.row <= GRID_DIM / 2) && (offset.col >= -GRID_DIM / 2 && offset.col <= GRID_DIM / 2);
}


int ATile_Manager::convert_FOffseto_idx(FOffset offset) const
{
	return ((-offset.row + GRID_DIM/2) * GRID_DIM) + (offset.col + GRID_DIM/2);
}

FOffset ATile_Manager::convert_idx_to_offset(int idx) const 
{
	return FOffset(-(idx / GRID_DIM) + GRID_DIM / 2, (idx % GRID_DIM) - GRID_DIM / 2);
}

ATile* ATile_Manager::get_tile(FOffset offset) const
{
	return get_tile(offset, grid_array);
}

ATile* ATile_Manager::get_tile_by_index(uint32 idx) const
{
	return get_tile(convert_idx_to_offset(idx), grid_array);
}

ATile* ATile_Manager::get_tile (FOffset offset, const TArray<ATile*>& arr) const
{
	// check if row/column offsets fall within GRID_DIMension of array (GRID_DIM)
	verify(ATile_Manager::validate_offset(offset));
	
	// index and return contents of array at indexed position
	int pos = ATile_Manager::convert_FOffseto_idx(offset);
	return arr[pos];
}

void ATile_Manager::set_tile(FOffset offset, ATile* tile, TArray<ATile*>& arr)
{
	// check if row/column fall within GRID_DIMension of array (GRID_DIM)
	verify(ATile_Manager::validate_offset(offset));

	// set contents of array at indexed position
	int pos = ATile_Manager::convert_FOffseto_idx(offset);
	arr[pos] = tile;
}


void ATile_Manager::spawn_tile(FOffset tile_offset)
{
	// get reference to center
	ATile* center = ATile_Manager::get_tile(FOffset(0,0), grid_array);
	FVector location_offset = FVector(tile_offset.row * TILE_SIZE, tile_offset.col * TILE_SIZE, 0);
	FTransform spawn_transform;
	// determine if center exists
	if (center != nullptr)
		spawn_transform.SetLocation(center->GetActorLocation() + location_offset);
	else
		spawn_transform.SetLocation(this->GetActorLocation() + location_offset);

	// spawn actor, adjust size variable, begin calling beginplay, and then set tile reference in grid array
	ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(TileBP, spawn_transform);
	ATile_Manager::generate_tile(tile);
	tile->FinishSpawning(spawn_transform);
	ATile_Manager::set_tile( tile_offset, tile, grid_array);
}

void ATile_Manager::update_grid(FVector location) 
{

	ATile* center_tile = ATile_Manager::get_tile(FOffset(0, 0), grid_array);
	
	verify(center_tile != nullptr);

	FOffset tile_offset = center_tile->get_player_offset(location);

	// check if player has moved from center tile (0,0)
	if (abs(tile_offset.row) + abs(tile_offset.col) != 0) {

		parent_generator_information = get_tile(tile_offset)->get_tile_data();

		// create empty grid array
		TArray<ATile*> temp_arr;
		temp_arr.Init(nullptr, GRID_DIM*GRID_DIM);

		// iterate through every tile to shift to new pos
		for (int32 Index = 0; Index != grid_array.Num(); ++Index)
		{
			// new offset of tile after shifting array
			FOffset offset = ATile_Manager::convert_idx_to_offset(Index);
			FOffset forward_offset = FOffset(offset.row - tile_offset.row, offset.col - tile_offset.col);
			FOffset backward_offset = FOffset(offset.row + tile_offset.row, offset.col + tile_offset.col);
				
			// if tiles shifted location is within GRID_DIM bounds, set shifted pos of temp array to unshifted pos in grid_array
			if (ATile_Manager::validate_offset(forward_offset))
				ATile_Manager::set_tile(forward_offset, ATile_Manager::get_tile(offset, grid_array), temp_arr);
			else
				get_tile(offset, grid_array)->Destroy();
		}
		// set contents of grid array to temp array
		grid_array = temp_arr;

		//spawn tiles for null positions of array (newly discovered tiles)
		ATile_Manager::populate_grid(false);
	}
}

void ATile_Manager::populate_grid(bool flush_array)
{
	// iterate through all tiles
	for (int32 Index = 0; Index != grid_array.Num(); ++Index)
	{
		// if tile is null, always spawn tile at location
		FOffset offset = ATile_Manager::convert_idx_to_offset(Index);
		ATile* tile = ATile_Manager::get_tile(offset);
		if (tile == nullptr)
			ATile_Manager::spawn_tile(offset);

		// otherwise only spawn and replace tiles if 'flush' parameter is true
		else if (flush_array) {
			tile->Destroy();
			ATile_Manager::spawn_tile(offset);
		}
	}
}

void ATile_Manager::pnormalize_vector(Eigen::VectorXf* vector) {
	*vector /= vector->sum();
}

void ATile_Manager::generate_tile(ATile* tile)
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

	ENV[parent_generator_information[0]] = 1;
	t1.resize(ENV.size());
	t1 = ENVxPREV_ENV_M * ENV;
	data[0] = make_random_selection(&t1);
	ENV.setZero();
	ENV[data[0]] = 1;

	WEATHER[parent_generator_information[1]] = 1;
	t1.resize(WEATHER.size());
	t2.resize(WEATHER.size());
	t1 = WEATHERxPREV_WEATHER_M * WEATHER;
	t2 = WEATHERxENV_M * ENV;
	t1 = t1.cwiseProduct(t2);
	pnormalize_vector(&t1);
	data[1] = make_random_selection(&t1);
	WEATHER.setZero();
	WEATHER[data[1]] = 1;

	DIFF[parent_generator_information[2]] = 1;
	t1.resize(DIFF.size());
	t2.resize(DIFF.size());
	t3.resize(DIFF.size());
	t1 = DIFFxPREV_DIFF_M * DIFF;
	t2 = DIFFxENV_M * ENV;
	t3 = DIFFxWEATHER_M * WEATHER;
	t1 = t1.cwiseProduct(t2);
	t1 = t1.cwiseProduct(t3);
	pnormalize_vector(&t1);
	data[2] = make_random_selection(&t1);
	DIFF.setZero();
	DIFF[data[2]] = 1;

	LOOT[parent_generator_information[3]] = 1;
	t1.resize(LOOT.size());
	t2.resize(LOOT.size());
	t1 = LOOTxPREV_LOOT_M * LOOT;
	t2 = LOOTxDIFF_M * DIFF;
	t1 = t1.cwiseProduct(t2);
	pnormalize_vector(&t1);
	data[3] = make_random_selection(&t1);
	LOOT.setZero();
	LOOT[data[3]] = 1;

	tile->init(data, TILE_SIZE);
	return;
}

void ATile_Manager::init_matrix_from_data(Eigen::MatrixXf* matrix, const TCHAR* utable_ref, int num_row, int num_col) {
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
	convert_data_table_to_matrix(matrix, DataTable);
	DataTableFinder.UnregisterGCObject();
	return;
}

void ATile_Manager::read_data() {
	init_matrix_from_data(&ENVxPREV_ENV_M, TEXT("DataTable'/Game/TileSystem/Data/P_Environment__Environment.P_Environment__Environment'"), 6, 6);
	init_matrix_from_data(&WEATHERxPREV_WEATHER_M, TEXT("DataTable'/Game/TileSystem/Data/P_Weather__Weather.P_Weather__Weather'"), 4, 4);
	init_matrix_from_data(&WEATHERxENV_M, TEXT("DataTable'/Game/TileSystem/Data/Environment__Weather.Environment__Weather'"), 4, 6);
	init_matrix_from_data(&DIFFxPREV_DIFF_M, TEXT("DataTable'/Game/TileSystem/Data/P_Difficulty__Difficulty.P_Difficulty__Difficulty'"), 5, 5);
	init_matrix_from_data(&DIFFxENV_M, TEXT("DataTable'/Game/TileSystem/Data/Environment__Difficulty.Environment__Difficulty'"), 5, 6);
	init_matrix_from_data(&DIFFxWEATHER_M, TEXT("DataTable'/Game/TileSystem/Data/Weather__Difficulty.Weather__Difficulty'"), 5, 4);
	init_matrix_from_data(&LOOTxPREV_LOOT_M, TEXT("DataTable'/Game/TileSystem/Data/P_LootCount__LootCount.P_LootCount__LootCount'"), 3, 3);
	init_matrix_from_data(&LOOTxDIFF_M, TEXT("DataTable'/Game/TileSystem/Data/Difficulty__LootCount.Difficulty__LootCount'"), 3, 5);
}

void ATile_Manager::convert_data_table_to_matrix(Eigen::MatrixXf* M, UDataTable* T) {
	M->setZero();

	FString ContextString;
	TArray<FName> RowNames = T->GetRowNames();

	// iterate through rows inside data table and store float array information inside matrix
	int row_index = 0;
	for (auto& name : RowNames)
	{
		FRow* row = T->FindRow<FRow>(name, ContextString);
		int col_index = 0;
		for (auto& value : row->vector){
			(*M)(row_index, col_index) = value;
			col_index++;
		}
		row_index++;
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SUCCESSFULLY READ UTABLE")));
	return;
}


int ATile_Manager::make_random_selection(Eigen::VectorXf* probability_vector) {
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
