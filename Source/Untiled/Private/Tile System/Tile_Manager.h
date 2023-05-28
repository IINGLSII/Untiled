// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Math/UnrealMathUtility.h"
#include "Tile.h"
#include "Tile_Manager.generated.h"

#define GRID_DIM 3
#define TILE_SIZE 500

USTRUCT(BlueprintType) 
struct FRow : public FTableRowBase 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> vector;
};

UCLASS()
class ATile_Manager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile_Manager();

protected:
	// class of tiles to be spawned
	UPROPERTY(EditAnywhere)
		TSubclassOf <class ATile> TileBP;

private:
	// array of tiles representing DIM x DIM grid
	UPROPERTY()
		TArray<ATile*> grid_array;

	// tile information from which new tile information will be generated
	tile_data_t parent_generator_information;

	// PROBABILITY MATRICES
	Eigen::MatrixXf ENVxPREV_ENV_M;
	Eigen::MatrixXf WEATHERxPREV_WEATHER_M;
	Eigen::MatrixXf WEATHERxENV_M;
	Eigen::MatrixXf DIFFxPREV_DIFF_M;
	Eigen::MatrixXf DIFFxENV_M;
	Eigen::MatrixXf DIFFxWEATHER_M;
	Eigen::MatrixXf LOOTxPREV_LOOT_M;
	Eigen::MatrixXf LOOTxDIFF_M;

// INTERNAL FUNCTIONS
	// gets tiles at r/c offset of a given array of tiles
	ATile* get_tile(FOffset offset, const TArray<ATile*>& arr) const;

	// sets tiles at r/c offset of a given array of tiles
	void set_tile(FOffset offset, ATile* tile, TArray<ATile*>& arr);

	// spawn tile at r/c offset at grid_array
	void spawn_tile(FOffset offset);

	// calculate new random tile information and load it into given tile. Calls tile update method.
	void generate_tile(ATile* tile);

	// reads tile data from data table and initalize markov matrix data
	void init_matrix_from_data(Eigen::MatrixXf* matrix, const TCHAR* utable_ref, int num_row, int num_col);

	// move data table information to matrix
	void convert_data_table_to_matrix(Eigen::MatrixXf* M, UDataTable* T);

	// read all the utable transition matrix data and store it in the appropirate matrix
	void read_data();

	int make_random_selection(Eigen::VectorXf* probability_vector);



// PURE FUNCTIONS
	// return if given offset is within valid range
	bool validate_offset(FOffset offset) const;

	// convert offset from center to array index
	int convert_FOffseto_idx(FOffset offset) const;

	// convert array index to offset from center
	FOffset convert_idx_to_offset(int index) const;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

// INTERFACE FUNCTIONS

	// returns tile at a given row/columb offset from grid_array
	UFUNCTION()
		ATile* get_tile(FOffset offset) const;

	// returns tile at specific index of the grid_array
	UFUNCTION()
		ATile* get_tile_by_index(uint32 index) const;

	// cull/update/spawn tiles according to a new given position.
	UFUNCTION(BlueprintCallable)
		void update_grid(FVector location);

	// spawns tiles in all empty indicies of grid_array, if flush is true, it will reintialize the whole list with new chunks.
	UFUNCTION(BlueprintCallable)
		void populate_grid(bool flush_array);

};
