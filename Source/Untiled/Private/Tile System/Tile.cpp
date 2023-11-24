// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/Tile.h"

// Sets default values
ATile::ATile()
{
	TileData.setZero();
	TileSize = 0;
	TileState.Init(0, NUM_FEATURES);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATile::Init(tile_data_t data, float size) {
	TileData = data;
	TileSize = size;

	for (int i = 0; i < NUM_FEATURES; i++)
		TileState[i] = TileData[i];
	
}

// returns the chunk offset of a given location, bool indicates axis (1 = X, 0 = Y)
FOffset ATile::GetPlayerOffset(FVector loc) const
{
	FVector position_offset;
	FOffset tile_offset = FOffset(0, 0);
	position_offset = loc - (TileSize / 2) - Super::GetActorLocation();

	if (abs(position_offset.X) > TileSize / 2) {
		if (position_offset.X > 0)
			tile_offset.Row = 1;
		else
			tile_offset.Row = -1;
	}

	if (abs(position_offset.Y) > TileSize / 2) {
		if (position_offset.Y > 0)
			tile_offset.Col = 1;
		else
			tile_offset.Col = -1;
	}
	return tile_offset;
}

tile_data_t ATile::GetTileData() {
	return TileData;
}