// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/TileDataGenerator.h"

// Sets default values for this component's properties
UTileDataGenerator::UTileDataGenerator()
{
	ReadData();
}



void UTileDataGenerator::PNormalizeVector(Eigen::VectorXf* vector) {
	*vector /= vector->sum();
}

tile_data_t UTileDataGenerator::GenerateTile(tile_data_t ParentTileInfo)
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

	ENV[ParentTileInfo[0]] = 1;
	t1.resize(ENV.size());
	t1 = ENVxPREV_ENV_M * ENV;
	data[0] = MakeRandomSelection(&t1);
	ENV.setZero();
	ENV[data[0]] = 1;

	WEATHER[ParentTileInfo[1]] = 1;
	t1.resize(WEATHER.size());
	t2.resize(WEATHER.size());
	t1 = WEATHERxPREV_WEATHER_M * WEATHER;
	t2 = WEATHERxENV_M * ENV;
	t1 = t1.cwiseProduct(t2);
	PNormalizeVector(&t1);
	data[1] = MakeRandomSelection(&t1);
	WEATHER.setZero();
	WEATHER[data[1]] = 1;

	DIFF[ParentTileInfo[2]] = 1;
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

	LOOT[ParentTileInfo[3]] = 1;
	t1.resize(LOOT.size());
	t2.resize(LOOT.size());
	t1 = LOOTxPREV_LOOT_M * LOOT;
	t2 = LOOTxDIFF_M * DIFF;
	t1 = t1.cwiseProduct(t2);
	PNormalizeVector(&t1);
	data[3] = MakeRandomSelection(&t1);
	LOOT.setZero();
	LOOT[data[3]] = 1;

	return data;
}

void UTileDataGenerator::InitMatrixFromData(Eigen::MatrixXf* matrix, const TCHAR* utable_ref, int num_row, int num_col) {
	// get data table object
	ConstructorHelpers::FObjectFinder<UDataTable> DataTableFinder(utable_ref);
	UDataTable* DataTable;
	if (DataTableFinder.Succeeded())
		DataTable = DataTableFinder.Object;
	else {

		return;
	}

	// initialize matrix and store information inside data table
	matrix->resize(num_row, num_col);
	ConvertDataTableToMatrix(matrix, DataTable);
	DataTableFinder.UnregisterGCObject();
	return;
}

void UTileDataGenerator::ReadData() {
	InitMatrixFromData(&ENVxPREV_ENV_M, TEXT("DataTable'/Game/TileSystem/Data/P_Environment__Environment.P_Environment__Environment'"), 6, 6);
	InitMatrixFromData(&WEATHERxPREV_WEATHER_M, TEXT("DataTable'/Game/TileSystem/Data/P_Weather__Weather.P_Weather__Weather'"), 4, 4);
	InitMatrixFromData(&WEATHERxENV_M, TEXT("DataTable'/Game/TileSystem/Data/Environment__Weather.Environment__Weather'"), 4, 6);
	InitMatrixFromData(&DIFFxPREV_DIFF_M, TEXT("DataTable'/Game/TileSystem/Data/P_Difficulty__Difficulty.P_Difficulty__Difficulty'"), 5, 5);
	InitMatrixFromData(&DIFFxENV_M, TEXT("DataTable'/Game/TileSystem/Data/Environment__Difficulty.Environment__Difficulty'"), 5, 6);
	InitMatrixFromData(&DIFFxWEATHER_M, TEXT("DataTable'/Game/TileSystem/Data/Weather__Difficulty.Weather__Difficulty'"), 5, 4);
	InitMatrixFromData(&LOOTxPREV_LOOT_M, TEXT("DataTable'/Game/TileSystem/Data/P_LootCount__LootCount.P_LootCount__LootCount'"), 3, 3);
	InitMatrixFromData(&LOOTxDIFF_M, TEXT("DataTable'/Game/TileSystem/Data/Difficulty__LootCount.Difficulty__LootCount'"), 3, 5);
}

void UTileDataGenerator::ConvertDataTableToMatrix(Eigen::MatrixXf* M, UDataTable* T) {
	M->setZero();

	FString ContextString;
	TArray<FName> RowNames = T->GetRowNames();

	// iterate through rows inside data table and store float array information inside matrix
	int row_index = 0;
	for (auto& name : RowNames)
	{
		FRow* row = T->FindRow<FRow>(name, ContextString);
		int col_index = 0;
		for (auto& value : row->Vector) {
			(*M)(row_index, col_index) = value;
			col_index++;
		}
		row_index++;
	}

	
	return;
}


int UTileDataGenerator::MakeRandomSelection(Eigen::VectorXf* probability_vector) const
{
	float value = FMath::FRandRange(0.0f, 1.0f);

	int selection;
	for (selection = 0; selection < probability_vector->size(); selection++) {

		if (value < (*probability_vector)[selection]) {
			return selection;
		}
		value -= (*probability_vector)[selection];
	}
	return 0;
}
