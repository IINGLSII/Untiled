// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Character System/Character_Base.h"
#include "Combat.generated.h"

#define TRACE_TIME_INC 0.015625
#define ATTACK_DELAY 0.1
DECLARE_DELEGATE_OneParam(FOnHitDelegate, AActor*)

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
	float attack_speed;

	// parameter for line trace (trace distance)
	UPROPERTY(EditDefaultsOnly)
	float hit_detection_param1;

	// parameter for line trace (length, trace increment)
	UPROPERTY(EditDefaultsOnly)
	float hit_detection_param2;

	// time before line trace begins (after animation start)
	UPROPERTY(EditDefaultsOnly)
	float trace_delay;

	// type of trace performed for hit detection
	UPROPERTY(EditDefaultsOnly)
	EHitDetectionType hit_detection_type;

	// attack animations played
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* attack_anim_left;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* attack_anim_right;

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombat : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombat();

	// callback for when trace hits an actor
	FOnHitDelegate on_hit_delegate;

private:
	// Trace Collision parameters
	FCollisionQueryParams hit_trace_params;

	// Trace Timer handler
	FTimerHandle trace_timer_handle;

	// Trace Timer Callback
	FTimerDelegate trace_timer_delegate;

	// Trace calls left for attack
	int8 trace_calls_remaining;

	// whether the character is able to attack
	bool can_attack = true;

	// direction of current/most recent attack
	int dir = 1;

	// decreases traces remaining and resets attack if <= 0
	void increment_trace();

private:
	// Cone Trace (radius, degrees)
	UFUNCTION()
		void cone_attack_trace(FAttackInfo& attack_info, float trace_param_increment, float trace_param_start);
	// Line Trace (length)
	UFUNCTION()
		void line_attack_trace(FAttackInfo& attack_info, float trace_param_increment, float trace_param_start);
	// Radial Trace (radius, degrees)
	UFUNCTION()
		void radial_attack_trace(FAttackInfo& attack_info, float trace_param_increment, float trace_param_start);
	// On hit registration
	UFUNCTION()
		void hit_registration(FHitResult& hit_result);
	// runs trace from start to end and registers hit
	UFUNCTION()
		void run_trace(FVector start, FVector end_offset);
	// sets and returns direction of attack
	int8 get_left_right(AActor*);


public:
	UFUNCTION()
	void attack(FAttackInfo& attack_info, FVector location);

	UFUNCTION()
	void reset_attack();

};
