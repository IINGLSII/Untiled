// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Eigen/Eigen/Core>
#include "Tile.generated.h"

#define NUM_FEATURES 4

typedef Eigen::Vector<uint8, NUM_FEATURES> tile_data_t;

USTRUCT()
struct FOffset
{
	GENERATED_BODY()

	FOffset() {
		Row = 0;
		Col = 0;
	}

	FOffset(int r, int c) {
		Row = r;
		Col = c;
	}

	UPROPERTY()
		int Row;

	UPROPERTY()
		int Col;

	friend bool operator==(FOffset OffsetA, FOffset OffsetB) {
		return ((OffsetA.Col == OffsetB.Col) && (OffsetA.Row == OffsetB.Row));
	}

	friend bool operator!=(FOffset OffsetA, FOffset OffsetB) {
		return !(OffsetA == OffsetB);
	}

	friend FOffset operator-(FOffset Offset) {
		FOffset NegativeOffset = Offset;
		NegativeOffset.Col = -NegativeOffset.Col;
		NegativeOffset.Row = -NegativeOffset.Row;
		return NegativeOffset;
	}

	friend FOffset operator-(FOffset OffsetA, FOffset OffsetB) {
		FOffset Difference;
		Difference.Col = OffsetA.Col-OffsetB.Col;
		Difference.Row = OffsetA.Row - OffsetB.Row;
		return Difference;
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
	tile_data_t GetTileData();
	
private:
	// Vector containing the randomly generated tile information
	tile_data_t TileData;

	int8 Owners = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// End to end tile size
	UPROPERTY(BlueprintReadOnly)
	float TileSize;

	// Blueprint readable tile data container
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=Update)
	TArray<uint8> TileState;
	

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:	

	UFUNCTION()
	FOffset GetOffset(FVector loc) const;

	UFUNCTION(BlueprintImplementableEvent)
	void Update();

	// Set the values off parameters before BeginPlay is called
	void Init(tile_data_t, float);

	char GetOwners();

	void MarkAsOwner(uint8 PlayerIndex);

	void UnmarkAsOwner(uint8 PlayerIndex);

	bool IsOwner(uint8 PlayerIndex) const;
};
