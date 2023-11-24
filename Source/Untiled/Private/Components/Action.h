// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Action.generated.h"


DECLARE_DELEGATE_OneParam(FActionDelegate, const FInputActionValue& Value)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UAction : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAction();

	UFUNCTION()
	bool StartAction(FName CallbackFunctionName, const FInputActionValue& Args);

	UFUNCTION()
	void FinishAction();

	UFUNCTION()
	void FinishActionTimer(float Time);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;



private:

	FActionDelegate QueuedAction;
	FInputActionValue QueuedActionArgs;

	FTimerDelegate FinishActionTimerDelegate;
	FTimerHandle FinishActionTimerHandler;

	UPROPERTY()
	bool bCanAct = true;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
};
