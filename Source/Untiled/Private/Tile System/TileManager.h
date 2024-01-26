// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GenericPlatform/GenericPlatformMath.h"
#include "Tile.h"
#include "TileManager.generated.h"

constexpr int GRID_DIM = 3;
constexpr float TILE_SIZE = 1200;


UCLASS()
class ATileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATileManager();

	UPROPERTY(EditAnywhere)
	TSubclassOf <class ATile> TileBp;

	// Owning Players Index
	UPROPERTY()
	uint8 PlayerIndex;

private:
	// array of tiles representing DIM x DIM grid
	UPROPERTY()
	TArray<ATile*> GridArray;

	// gets tiles at r/c offset of a given array of tiles
	ATile* GetTile(FOffset offset, const TArray<ATile*>& arr) const;

	// sets tiles at r/c offset of a given array of tiles
	void SetTile(FOffset offset, ATile* tile, TArray<ATile*>& arr);
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// PURE FUNCTIONS
	// return if given offset is within valid range
	static bool ValidateOffset(FOffset offset);

	// convert offset from center to array index
	static int ConvertFOffsetoIdx(FOffset offset);

	// convert array index to offset from center
	static FOffset ConvertIdxToOffset(int index);

	// given a location, returns the offset of the occupying tile
		FOffset GetOffset(FVector Location);

	// returns the location of where a tile is placed at a given offset
		FVector GetTileLocation(FOffset Offset);

	// returns tile at a given row/columb offset from GridArray
		ATile* GetTile(FOffset offset) const;

	// returns tile at specific index of the GridArray
		ATile* GetTileByIndex(uint32 index) const;

	// returns location of the grid origin (origin of the world is no valid center tile exists)
		FVector GetGridLocation();

	// Shifts tiles in grid according to a given offset, undefined tiles are given null references.
	void ShiftTiles(FOffset GridOffset);

	// inserts a tile reference to given offset, does not clean prior reference
	void InsertTile(ATile* NewTile, FOffset GridPosition);

	// spawns a tile at a given offset and tile information data
	ATile* SpawnTile(FOffset offset, tile_data_t TileData);

};
