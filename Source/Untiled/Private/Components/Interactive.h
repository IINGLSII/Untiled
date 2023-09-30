// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Interfaces/Interactable.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interactive.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInteractive : public UActorComponent
{
	GENERATED_BODY()

	public:	
		// Sets default values for this component's properties
		UInteractive();

	protected:
		// Called when the game starts
		virtual void BeginPlay() override;

		/* Add interactable to interactables set */
		void removeInteractable(IInteractable*);

		/* Remove interactable from interactables set */
		void addInteractable(IInteractable*);


	private:
		/* Object player will interact with if they press R */
		IInteractable* interact_object;

		/* Set of objects player is able to interact with */
		TSet<IInteractable*> interactables;

		/** Interact boundary collision event **/
		UFUNCTION()
		void InteractBoundaryOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

		UFUNCTION()
		void InteractBoundaryOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	public:	
		// Called every frame
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		// Sets the collison used for interaction detection
		void SetupCollision(UCapsuleComponent*);

		// Calling function when actor wishes to interact
		void interact();

		
};
