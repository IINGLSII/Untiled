// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameFramework/PlayerState.h"
#include "MyGameState.generated.h"

/**
 * 
 */
UCLASS()
class AMyGameState : public AGameState
{
	GENERATED_BODY()
	

public:
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetPlayerLocations();
};
