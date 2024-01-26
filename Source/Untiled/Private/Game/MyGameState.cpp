// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/MyGameState.h"

TArray<FVector> AMyGameState::GetPlayerLocations()
{
	TArray<FVector> PlayerLocations;
	for (APlayerState* PlayerState : PlayerArray) {
		PlayerLocations.Add(PlayerState->GetPawn()->GetActorLocation());
	}
	return PlayerLocations;
}
