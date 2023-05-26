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

void ATile_Manager::generate_tile(ATile* tile)
{
	// vector for tile state information
	tile_data_t data;
	data.setZero();

	// current for state information of current tile
	Eigen::VectorXf current_env(6);
	current_env.setZero();
	current_env[parent_generator_information[0]] = 1;


	data[0] = markov(&ENV_PREV_ENV_M, &current_env);

	tile->init(data, TILE_SIZE);
	return;
}

void ATile_Manager::read_data() {
	// get data table object
	static ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(TEXT("DataTable'/Game/TileSystem/Data/P_Environment__Environment.P_Environment__Environment'"));
	UDataTable* DataTable;
	if (DataTableFinder.Succeeded())
		DataTable = DataTableFinder.Object;
	else {
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("COULD NOT GET DATA TABLE")));
		return;
	}

	// initialize matrix and store information inside data table
	ENV_PREV_ENV_M.resize(6, 6);
	convert_data_table_to_matrix(&ENV_PREV_ENV_M, DataTable);
	
	return;
}

void ATile_Manager::convert_data_table_to_matrix(Eigen::MatrixXf* M, UDataTable* T) {
	M->setZero();

	FString ContextString = FString(TEXT("Convert Data Table to Matrix"));
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

int ATile_Manager::markov(Eigen::MatrixXf* transition_matrix, Eigen::VectorXf* state_vector) const {
	Eigen::VectorXf probability_vector = (*transition_matrix) * (*state_vector);
	float value = FMath::FRandRange(0.0f, 1.0f);

	// Determine selection from random value
	int selection;
	for (selection = 0; selection < probability_vector.size(); selection++) {
		
		if (value < probability_vector[selection]) {
			return selection;
		}
		value -= probability_vector[selection];
	}
	
	// Matrix values do not sum to 1, or random value is not in range(0,1)
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("SHOULD NOT HAPPEN")));
	return 0;
}