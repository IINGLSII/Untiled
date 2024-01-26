// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "Tile System/TileManager.h"
#include "Tile System/TileDataGenerator.h"
#include "MultiplayerTileManager.generated.h"


UCLASS()
class AMultiplayerTileManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMultiplayerTileManager();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf <class ATile> TileBp;

private:

	UPROPERTY()
	UTileDataGenerator* TileDataGenerator;

	UPROPERTY()
	TArray<ATileManager*>PlayerTileManagers;


private:
	bool HasOverlap(ATileManager* Player1Grid, ATileManager* Player2Grid);

	ATile* SpawnTile(ATileManager* PlayerGrid, FOffset Offset);

	void UpdateGrid(ATileManager* Grid);
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, SERVER, RELIABLE)
	void Update();

	UFUNCTION(BlueprintCallable, SERVER, RELIABLE)
	void AddPlayer();

	ATileManager* GetTileManager(uint8 PlayerIndex);
};
