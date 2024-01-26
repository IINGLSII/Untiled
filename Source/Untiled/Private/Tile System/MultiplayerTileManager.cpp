// Fill out your copyright notice in the Description page of Project Settings.


#include "Tile System/MultiplayerTileManager.h"

// Sets default values
AMultiplayerTileManager::AMultiplayerTileManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	TileDataGenerator = CreateDefaultSubobject<UTileDataGenerator>(TEXT("TileDataGenerator"));

}

bool AMultiplayerTileManager::HasOverlap(ATileManager* Player1Grid, ATileManager* Player2Grid)
{
	FVector ALoc = Player1Grid->GetGridLocation();
	FVector BLoc = Player2Grid->GetGridLocation();

	FVector DistanceVector = (ALoc - BLoc).GetAbs();

	return (DistanceVector.X < 3 * TILE_SIZE) && (DistanceVector.Y < 3 * TILE_SIZE);
}

ATile* AMultiplayerTileManager::SpawnTile(ATileManager* PlayerGrid, FOffset Offset)
{
	ATile* CenterTile = PlayerGrid->GetTile(FOffset(0, 0));
	tile_data_t ParentTileInfo;
	if (CenterTile)
		ParentTileInfo = CenterTile->GetTileData();
	else
		ParentTileInfo.setZero();

	tile_data_t TileData = TileDataGenerator->GenerateTile(ParentTileInfo);
	return PlayerGrid->SpawnTile(Offset, TileData);
}


void AMultiplayerTileManager::UpdateGrid(ATileManager* PlayerGrid)
{
	for (int32 Index = 0; Index != GRID_DIM * GRID_DIM; ++Index)
	{
		FOffset Offset = ATileManager::ConvertIdxToOffset(Index);
		ATile* Tile = PlayerGrid->GetTile(Offset);
		if (Tile)
			continue;

		bool found_other = 0;
		for (ATileManager* OtherPlayerGrid : PlayerTileManagers) {
			if (OtherPlayerGrid == PlayerGrid)
				continue; // Is the players grid

			FOffset OtherPlayerOffset = OtherPlayerGrid->GetOffset(PlayerGrid->GetTileLocation(Offset));
			if (!ATileManager::ValidateOffset(OtherPlayerOffset))
				continue; // No overlap at this tile
			
			ATile* SharedTileOnOther = OtherPlayerGrid->GetTile(OtherPlayerOffset);
			if (SharedTileOnOther) {
				PlayerGrid->InsertTile(SharedTileOnOther, Offset);
			}
			else {
				Tile = SpawnTile(PlayerGrid, Offset);
				OtherPlayerGrid->InsertTile(Tile, OtherPlayerOffset);
			}
			found_other = 1;
			break;
		}

		if(!found_other)
			SpawnTile(PlayerGrid, Offset);
	}
}

// Called every frame
void AMultiplayerTileManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMultiplayerTileManager::Update_Implementation()
{
	for (int i = 0; i < PlayerTileManagers.Num(); i++) {
		
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),i);
		FOffset ShiftOffset = PlayerTileManagers[i]->GetOffset(PlayerPawn->GetActorLocation());
		PlayerTileManagers[i]->ShiftTiles(ShiftOffset);
	}

	for (ATileManager* GridManager : PlayerTileManagers) {
		UpdateGrid(GridManager);
	}
		
}

ATileManager* AMultiplayerTileManager::GetTileManager(uint8 PlayerIndex)
{
	assert(PlayerIndex < PlayerTileManagers.Num);

	return PlayerTileManagers[PlayerIndex];
}

void AMultiplayerTileManager::AddPlayer_Implementation()
{
	ATileManager* Manager = GetWorld()->SpawnActor<ATileManager>(ATileManager::StaticClass());
	Manager->PlayerIndex = PlayerTileManagers.Num();
	Manager->TileBp = TileBp;
	PlayerTileManagers.Add(Manager);
}


