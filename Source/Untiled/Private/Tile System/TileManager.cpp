// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/TileManager.h"

ATileManager::ATileManager()
{
	bReplicates = true;
	bNetLoadOnClient = true;
}

void ATileManager::BeginPlay() 
{
	GridArray.Init(nullptr, GRID_DIM * GRID_DIM);
}


bool ATileManager::ValidateOffset(FOffset offset)
{
	return (offset.Row >= -GRID_DIM / 2 && offset.Row <= GRID_DIM / 2) && (offset.Col >= -GRID_DIM / 2 && offset.Col <= GRID_DIM / 2);
}


int ATileManager::ConvertFOffsetoIdx(FOffset offset)
{
	return ((-offset.Row + GRID_DIM/2) * GRID_DIM) + (offset.Col + GRID_DIM/2);
}

 FOffset ATileManager::ConvertIdxToOffset(int idx)
{
	return FOffset(-(idx / GRID_DIM) + GRID_DIM / 2, (idx % GRID_DIM) - GRID_DIM / 2);
}

FOffset ATileManager::GetOffset(FVector Location)
{
	FVector Center = GetGridLocation() + TILE_SIZE / 2;

	FVector DistanceVector = Location - Center;
	DistanceVector /= TILE_SIZE;

	int XOffset = FMath::RoundToInt(DistanceVector.X);
	int YOffset = FMath::RoundToInt(DistanceVector.Y);
	return FOffset(XOffset, YOffset);
}

FVector ATileManager::GetTileLocation(FOffset Offset)
{
	FVector Center = GetGridLocation() + TILE_SIZE / 2;
	FVector OffsetVector;
	OffsetVector.X = Offset.Row * TILE_SIZE;
	OffsetVector.Y = Offset.Col * TILE_SIZE;
	return Center + OffsetVector;
}

ATile* ATileManager::GetTile(FOffset offset) const
{
	return GetTile(offset, GridArray);
}

ATile* ATileManager::GetTileByIndex(uint32 idx) const
{
	return GetTile(ConvertIdxToOffset(idx), GridArray);
}

ATile* ATileManager::GetTile (FOffset offset, const TArray<ATile*>& arr) const
{
	// check if row/column offsets fall within GRID_DIMension of array (GRID_DIM)
	verify(ATileManager::ValidateOffset(offset));
	
	// index and return contents of array at indexed position
	int pos = ATileManager::ConvertFOffsetoIdx(offset);
	return arr[pos];
}

void ATileManager::SetTile(FOffset offset, ATile* tile, TArray<ATile*>& arr)
{
	// check if row/column fall within GRID_DIMension of array (GRID_DIM)
	verify(ATileManager::ValidateOffset(offset));

	// set contents of array at indexed position
	int pos = ATileManager::ConvertFOffsetoIdx(offset);
	arr[pos] = tile;
}


ATile* ATileManager::SpawnTile(FOffset tile_offset, tile_data_t TileData)
{
	// get reference to center
	ATile* center = ATileManager::GetTile(FOffset(0,0), GridArray);
	FVector location_offset = FVector(tile_offset.Row * TILE_SIZE, tile_offset.Col * TILE_SIZE, 0);
	FTransform spawn_transform;
	// determine if center exists
	if (center != nullptr)
		spawn_transform.SetLocation(center->GetActorLocation() + location_offset);
	else
		spawn_transform.SetLocation(location_offset);

	// spawn actor, adjust size variable, begin calling beginplay, and then set tile reference in grid array
	ATile* tile = GetWorld()->SpawnActorDeferred<ATile>(TileBp, spawn_transform);
	if (tile) {
		tile->Init(TileData, TILE_SIZE);
		tile->FinishSpawning(spawn_transform);
		ATileManager::SetTile(tile_offset, tile, GridArray);
		tile->MarkAsOwner(PlayerIndex);
		return tile;
	}
	else
		return nullptr;
}


FVector ATileManager::GetGridLocation()
{
	ATile* CenterTile = GetTile(FOffset(0, 0));
	FVector Center;
	if (CenterTile)
		return CenterTile->GetActorLocation();
	else
		return GetActorLocation();
}

void ATileManager::ShiftTiles(FOffset GridOffset)
{
	// create empty grid array
	TArray<ATile*> temp_arr;
	temp_arr.Init(nullptr, GRID_DIM * GRID_DIM);

	if (GridOffset.Col == 0 && GridOffset.Row == 0)
		return;

	// iterate through every tile to shift to new pos
	for (int32 Index = 0; Index != GridArray.Num(); ++Index)
	{
		// new offset of tile after shifting array
		FOffset offset = ATileManager::ConvertIdxToOffset(Index);
		FOffset forward_offset = FOffset(offset.Row - GridOffset.Row, offset.Col - GridOffset.Col);

		// if tiles shifted location is within GRID_DIM bounds, set shifted pos of temp array to unshifted pos in GridArray
		if (ATileManager::ValidateOffset(forward_offset))
			ATileManager::SetTile(forward_offset, ATileManager::GetTile(offset, GridArray), temp_arr);
		else{
			ATile* back_tile = GetTile(offset, GridArray);
			if (back_tile)
				back_tile->UnmarkAsOwner(PlayerIndex);
		}
			
	}

	GridArray = temp_arr;
}

void ATileManager::InsertTile(ATile* NewTile, FOffset GridPosition) {
	SetTile(GridPosition, NewTile, GridArray);
	NewTile->MarkAsOwner(PlayerIndex);
}
