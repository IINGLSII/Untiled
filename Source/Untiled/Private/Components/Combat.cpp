// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat.h"

// Sets default values for this component's properties
UCombat::UCombat()
{
	HitTraceParams.AddIgnoredActor(GetOwner());
}

void UCombat::AttackTrace(FAttackInfo& AttackInfo, FVector location)
{
	float trace_param_start;
	float trace_param_increment;
	switch (AttackInfo.HitDetectionType) {
		case EHitDetectionType::CONE:
			TraceCallsRemaining = FMath::RoundFromZero(AttackInfo.HitDetectionParam2 / abs(AttackInfo.AttackSpeed*2) / TRACE_TIME_INC);
			trace_param_increment = (abs(AttackInfo.AttackSpeed) / AttackInfo.AttackSpeed) * AttackInfo.HitDetectionParam2 / TraceCallsRemaining;
			trace_param_start = -(abs(AttackInfo.AttackSpeed)/AttackInfo.AttackSpeed)*(AttackInfo.HitDetectionParam2 / 2);
			TraceTimerDelegate.BindUFunction(this, TEXT("ConeAttackTrace"), AttackInfo, trace_param_increment, trace_param_start);
			GetOwner()->GetWorldTimerManager().SetTimer(TraceTimerHandle, TraceTimerDelegate, TRACE_TIME_INC, true, AttackInfo.TraceDelay);
			break;
		case EHitDetectionType::LINE:
			TraceCallsRemaining = FMath::RoundFromZero(AttackInfo.HitDetectionParam1 / AttackInfo.AttackSpeed / TRACE_TIME_INC);
			trace_param_increment = AttackInfo.HitDetectionParam1/TraceCallsRemaining;
			trace_param_start = TraceCallsRemaining;
			TraceTimerDelegate.BindUFunction(this, TEXT("LineAttackTrace"), AttackInfo, trace_param_increment, trace_param_start);
			GetOwner()->GetWorldTimerManager().SetTimer(TraceTimerHandle, TraceTimerDelegate, TRACE_TIME_INC, true, AttackInfo.TraceDelay);
			break;
		case EHitDetectionType::RADIAL:
			TraceCallsRemaining = FMath::RoundFromZero(AttackInfo.HitDetectionParam2 / 5);
			trace_param_increment = 5;
			trace_param_start = -AttackInfo.HitDetectionParam2 / 2;
			RadialAttackTrace(AttackInfo, trace_param_increment, trace_param_start);
			break;
	}
	Cast<ACharacter_Base>(GetOwner())->PlayAnimMontage(AttackInfo.AttackAnim);
}

void UCombat::IncrementTrace()
{
	TraceCallsRemaining--;
	if (TraceCallsRemaining <= 0) {
		FinishTrace();
	}
}

void UCombat::ConeAttackTrace(FAttackInfo& AttackInfo, float trace_param_increment, float trace_param_start)
{
	FVector trace_offset = (GetOwner()->GetActorForwardVector() * AttackInfo.HitDetectionParam1).RotateAngleAxis(TraceCallsRemaining * trace_param_increment + trace_param_start, FVector::ZAxisVector);
	FVector start = GetOwner()->GetActorLocation();
	RunTrace(start, trace_offset);
}

void UCombat::LineAttackTrace(FAttackInfo& AttackInfo, float trace_param_increment, float trace_param_start)
{	
	FVector trace_offset = GetOwner()->GetActorForwardVector()*trace_param_increment * (trace_param_start - TraceCallsRemaining);
	FVector start = GetOwner()->GetActorLocation();
	RunTrace(start, trace_offset);
}

void UCombat::RadialAttackTrace(FAttackInfo& AttackInfo, float trace_param_increment, float trace_param_start)
{
	FVector start = GetOwner()->GetActorLocation();
	FVector trace_offset;

	while (TraceCallsRemaining > 0) {
		trace_offset = (GetOwner()->GetActorForwardVector() * AttackInfo.HitDetectionParam1).RotateAngleAxis(TraceCallsRemaining * trace_param_increment + trace_param_start, FVector::ZAxisVector);
		RunTrace(start, trace_offset);
	}
}

void UCombat::RunTrace(FVector start, FVector end_offset) {
	FHitResult trace_result;
	GetWorld()->LineTraceSingleByChannel(trace_result, start, start + end_offset, ECollisionChannel::ECC_WorldDynamic, HitTraceParams);
	DrawDebugLine(GetWorld(), start, trace_result.TraceEnd, trace_result.bBlockingHit ? FColor::Blue : FColor::Red, false, 1, 0);
	HitRegistration(trace_result);
	IncrementTrace();
}

void UCombat::HitRegistration(FHitResult& hit_result)
{
	if (!hit_result.bBlockingHit)
		return;
	// Call on hit delegate and prevent further hit registration
	AActor* hit_actor = hit_result.GetActor();
	OnHitDelegate.Execute(hit_actor);
	HitTraceParams.AddIgnoredActor(hit_actor);
}

void UCombat::FinishTrace()
{
	// Remove any timers on this component
	GetOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);
	
	// Setup tracing parameters
	HitTraceParams.ClearIgnoredActors();
	HitTraceParams.AddIgnoredActor(GetOwner());

	TraceTimerDelegate.BindUFunction(this, FName("EndAttack"));
	GetOwner()->GetWorldTimerManager().SetTimer(TraceTimerHandle, TraceTimerDelegate, 1, 0, 0.125f);
}

void UCombat::EndAttack()
{
	AttackResetDelegate.Execute();
}

