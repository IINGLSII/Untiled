// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Character System/Character_Base.h"
#include "Combat.generated.h"

#define TRACE_TIME_INC 0.015625
#define ATTACK_DELAY 0.15
DECLARE_DELEGATE_OneParam(FOnHitDelegate, AActor*)
DECLARE_DELEGATE(FAttackResetDelegate)

UENUM(BlueprintType)
enum class EHitDetectionType : uint8 {			// params:
	CONE = 0 UMETA(DisplayName = "CONE"),		// radius, angle
	LINE = 1 UMETA(DisplayName = "LINE"),		// length, width
	RADIAL = 2 UMETA(DisplayName = "RADIAL")	// distance
};

USTRUCT(BlueprintType)
struct FAttackInfo {
	GENERATED_BODY()

public:
	// attack speed (detection % per second)
	UPROPERTY(EditDefaultsOnly)
	float AttackSpeed;

	// parameter for line trace (trace distance)
	UPROPERTY(EditDefaultsOnly)
	float HitDetectionParam1;

	// parameter for line trace (length, trace increment)
	UPROPERTY(EditDefaultsOnly)
	float HitDetectionParam2;

	// time before line trace begins (after animation start)
	UPROPERTY(EditDefaultsOnly)
	float TraceDelay;

	// type of trace performed for hit detection
	UPROPERTY(EditDefaultsOnly)
	EHitDetectionType HitDetectionType;

	// attack animations played
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* AttackAnim;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombat();

	// callback for when trace hits an actor
	FOnHitDelegate OnHitDelegate;

	FAttackResetDelegate AttackResetDelegate;

private:
	// Trace Collision parameters
	FCollisionQueryParams HitTraceParams;

	// Trace Timer handler
	FTimerHandle TraceTimerHandle;

	// Trace Timer Callback
	FTimerDelegate TraceTimerDelegate;

	// Trace calls left for attack
	int8 TraceCallsRemaining;

	// whether the character is able to attack
	bool bCanAttack = true;

private:
	// Cone Trace (radius, degrees)
	UFUNCTION()
	void ConeAttackTrace(FAttackInfo& AttackInfo, float trace_param_increment, float trace_param_start);

	// Line Trace (length)
	UFUNCTION()
	void LineAttackTrace(FAttackInfo& AttackInfo, float trace_param_increment, float trace_param_start);

	// Radial Trace (radius, degrees)
	UFUNCTION()
	void RadialAttackTrace(FAttackInfo& AttackInfo, float trace_param_increment, float trace_param_start);

	// On hit registration
	UFUNCTION()
	void HitRegistration(FHitResult& hit_result);

	// runs trace from start to end and registers hit
	void RunTrace(FVector start, FVector end_offset);

	// decreases traces remaining and resets attack if <= 0
	void IncrementTrace();

	UFUNCTION()
	void EndAttack();

public:
	void AttackTrace(FAttackInfo& AttackInfo, FVector location);

	void FinishTrace();
};
