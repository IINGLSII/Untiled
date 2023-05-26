// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Eigen/Eigen/Core>
#include "Tile.generated.h"

#define NUM_FEATURES 5

typedef Eigen::Vector<uint8, NUM_FEATURES> tile_data_t;

USTRUCT()
struct FOffset
{
	GENERATED_BODY()

	FOffset() {
		row = 0;
		col = 0;
	}

	FOffset(int r, int c) {
		row = r;
		col = c;
	}

	UPROPERTY()
		int row;

	UPROPERTY()
		int col;

	friend bool operator==(FOffset OffsetA, FOffset OffsetB) {
		return ((OffsetA.col == OffsetB.col) && (OffsetA.row == OffsetB.row));
	}

	friend bool operator!=(FOffset OffsetA, FOffset OffsetB) {
		return !(OffsetA == OffsetB);
	}
};

UCLASS()
class ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

	// returns vector of tile information
	tile_data_t get_tile_data();
	
private:
	// Vector containing the randomly generated tile information
	tile_data_t tile_data;
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// End to end tile size
	UPROPERTY(BlueprintReadOnly)
	float tile_size;

	// Blueprint readable tile data container
	UPROPERTY(BlueprintReadOnly)
	TArray<uint8> tile_state;

public:	

	UFUNCTION()
	FOffset get_player_offset(FVector loc) const;

	// Set the values off parameters before BeginPlay is called
	void init(tile_data_t, float);
};
