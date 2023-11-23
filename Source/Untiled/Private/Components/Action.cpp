// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Action.h"

// Sets default values for this component's properties
UAction::UAction()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	FinishActionTimerDelegate.BindUFunction(this, FName("FinishAction"));
	
	// ...
}


// Called when the game starts
void UAction::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UAction::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UAction::FinishAction()
{
	bCanAct = true;
	QueuedAction.ExecuteIfBound(QueuedActionArgs);
}

void UAction::FinishActionTimer(float Time)
{
	GetWorld()->GetTimerManager().SetTimer(FinishActionTimerHandler, FinishActionTimerDelegate, 1, 0, Time);
}

bool UAction::StartAction(FName CallbackFunctionName, const FInputActionValue& Args)
{
	QueuedAction.Unbind();
	if (bCanAct) {
		bCanAct = false;
		return true;
	}
	
	QueuedActionArgs = Args;
	QueuedAction.BindUFunction(GetOwner(), CallbackFunctionName);
	return false;
	
}

