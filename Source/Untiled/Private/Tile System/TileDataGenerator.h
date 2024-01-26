// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Tile.h"
#include "Math/UnrealMathUtility.h"
#include "Engine/DataTable.h"
#include "TileDataGenerator.generated.h"

USTRUCT(BlueprintType)
struct FRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<float> Vector;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UTileDataGenerator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTileDataGenerator();

protected:

private:
	// PROBABILITY MATRICES
	Eigen::MatrixXf ENVxPREV_ENV_M;
	Eigen::MatrixXf WEATHERxPREV_WEATHER_M;
	Eigen::MatrixXf WEATHERxENV_M;
	Eigen::MatrixXf DIFFxPREV_DIFF_M;
	Eigen::MatrixXf DIFFxENV_M;
	Eigen::MatrixXf DIFFxWEATHER_M;
	Eigen::MatrixXf LOOTxPREV_LOOT_M;
	Eigen::MatrixXf LOOTxDIFF_M;

public:	

	// calculate new random tile information and load it into given tile. Calls tile update method.
	tile_data_t GenerateTile(tile_data_t ParentTileInfo);

	// reads tile data from data table and initalize markov matrix data
	void InitMatrixFromData(Eigen::MatrixXf* matrix, const TCHAR* utable_ref, int num_row, int num_col);

	// move data table information to matrix
	void ConvertDataTableToMatrix(Eigen::MatrixXf* M, UDataTable* T);

	// read all the utable transition matrix data and store it in the appropirate matrix
	void ReadData();

	// given a weighted probability vector, chooses an index according to given random weights
	int MakeRandomSelection(Eigen::VectorXf* probability_vector) const;

	// normalizes vector
	void PNormalizeVector(Eigen::VectorXf* vector);
};
