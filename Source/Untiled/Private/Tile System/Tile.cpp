// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/Tile.h"

// Sets default values
ATile::ATile()
{
	tile_data.setZero();
	tile_size = 0;
	tile_state.Init(0, NUM_FEATURES);
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATile::init(tile_data_t data, float size) {
	tile_data = data;
	tile_size = size;

	for (int i = 0; i < NUM_FEATURES; i++)
		tile_state[i] = tile_data[i];
	
}

// returns the chunk offset of a given location, bool indicates axis (1 = X, 0 = Y)
FOffset ATile::get_player_offset(FVector loc) const
{
	FVector position_offset;
	FOffset tile_offset = FOffset(0, 0);
	position_offset = loc - (tile_size / 2) - Super::GetActorLocation();

	if (abs(position_offset.X) > tile_size / 2) {
		if (position_offset.X > 0)
			tile_offset.row = 1;
		else
			tile_offset.row = -1;
	}

	if (abs(position_offset.Y) > tile_size / 2) {
		if (position_offset.Y > 0)
			tile_offset.col = 1;
		else
			tile_offset.col = -1;
	}
	return tile_offset;
}

tile_data_t ATile::get_tile_data() {
	return tile_data;
}