// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat.h"

// Sets default values for this component's properties
UCombat::UCombat()
{
	hit_trace_params.AddIgnoredActor(GetOwner());
}

void UCombat::attack(FAttackInfo& attack_info, FVector location)
{
	if (!can_attack)
		return;
	// rotational direction of attacking entity
	get_left_right(GetOwner());
	// start of delta paramter
	float trace_param_start;
	// increment for delta parameter
	float trace_param_increment;
	switch (attack_info.hit_detection_type) {
		case EHitDetectionType::CONE:
			trace_calls_remaining = FMath::RoundFromZero(attack_info.hit_detection_param2 / attack_info.attack_speed / TRACE_TIME_INC);
			trace_param_increment = attack_info.hit_detection_param2 / trace_calls_remaining * dir;
			trace_param_start = -dir * (attack_info.hit_detection_param2 / 2);
			trace_timer_delegate.BindUFunction(this, TEXT("cone_attack_trace"), attack_info, trace_param_increment, trace_param_start);
			GetOwner()->GetWorldTimerManager().SetTimer(trace_timer_handle, trace_timer_delegate, TRACE_TIME_INC, true, attack_info.trace_delay);
			break;
		case EHitDetectionType::LINE:
			trace_calls_remaining = FMath::RoundFromZero(attack_info.hit_detection_param1 / attack_info.attack_speed / TRACE_TIME_INC);
			trace_param_increment = attack_info.hit_detection_param1/trace_calls_remaining;
			trace_param_start = trace_calls_remaining;
			trace_timer_delegate.BindUFunction(this, TEXT("line_attack_trace"), attack_info, trace_param_increment, trace_param_start);
			GetOwner()->GetWorldTimerManager().SetTimer(trace_timer_handle, trace_timer_delegate, TRACE_TIME_INC, true, attack_info.trace_delay);
			break;
		case EHitDetectionType::RADIAL:
			trace_calls_remaining = FMath::RoundFromZero(attack_info.hit_detection_param2 / 5);
			trace_param_increment = 5;
			trace_param_start = -attack_info.hit_detection_param2 / 2;
			radial_attack_trace(attack_info, trace_param_increment, trace_param_start);
			break;
	}
	can_attack = false;
	Cast<ACharacter_Base>(GetOwner())->PlayAnimMontage(dir > 0 ? attack_info.attack_anim_left : attack_info.attack_anim_right);
}

void UCombat::increment_trace()
{
	trace_calls_remaining--;
	if (trace_calls_remaining <= 0) {
		// Remove timer on trace and add timer to reset
		GetOwner()->GetWorldTimerManager().SetTimer(trace_timer_handle, this, &UCombat::reset_attack, TRACE_TIME_INC, false, ATTACK_DELAY);
	}
}

void UCombat::cone_attack_trace(FAttackInfo& attack_info, float trace_param_increment, float trace_param_start)
{
	FVector trace_offset = (GetOwner()->GetActorForwardVector() * attack_info.hit_detection_param1).RotateAngleAxis(trace_calls_remaining * trace_param_increment + trace_param_start, FVector::ZAxisVector);
	FVector start = GetOwner()->GetActorLocation();
	run_trace(start, trace_offset);
}

void UCombat::line_attack_trace(FAttackInfo& attack_info, float trace_param_increment, float trace_param_start)
{	
	FVector trace_offset = GetOwner()->GetActorForwardVector()*trace_param_increment * (trace_param_start - trace_calls_remaining);
	FVector start = GetOwner()->GetActorLocation();
	run_trace(start, trace_offset);
}

void UCombat::radial_attack_trace(FAttackInfo& attack_info, float trace_param_increment, float trace_param_start)
{
	FVector start = GetOwner()->GetActorLocation();
	FVector trace_offset;

	while (trace_calls_remaining > 0) {
		trace_offset = (GetOwner()->GetActorForwardVector() * attack_info.hit_detection_param1).RotateAngleAxis(trace_calls_remaining * trace_param_increment + trace_param_start, FVector::ZAxisVector);
		run_trace(start, trace_offset);
	}
}

void UCombat::run_trace(FVector start, FVector end_offset) {
	FHitResult trace_result;
	GetWorld()->LineTraceSingleByChannel(trace_result, start, start + end_offset, ECollisionChannel::ECC_WorldDynamic, hit_trace_params);
	DrawDebugPoint(GetWorld(), trace_result.TraceEnd, 15.0f, trace_result.bBlockingHit ? FColor::Blue : FColor::Red, false, 1.0f, 0);
	hit_registration(trace_result);
	increment_trace();
}

int8 UCombat::get_left_right(AActor* actor)
{
	float ang_v = Cast<ACharacter>(actor)->GetMesh()->GetPhysicsAngularVelocityInDegrees().Z;
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("ang_v = %f"), ang_v));

	// flip direction from previous if no angular velocity
	if (ang_v == 0)
		dir = -dir;
	else
		dir = ang_v > 0 ? -1 : 1;

	return dir;
}

void UCombat::hit_registration(FHitResult& hit_result)
{
	if (!hit_result.bBlockingHit)
		return;
	// Call on hit delegate and prevent further hit registration
	AActor* hit_actor = hit_result.GetActor();
	on_hit_delegate.Execute(hit_actor);
	hit_trace_params.AddIgnoredActor(hit_actor);
}

void UCombat::reset_attack()
{
	// Remove timers set on delegate and enable attacking
	GetOwner()->GetWorldTimerManager().SetTimer(trace_timer_handle, trace_timer_delegate, 0, false);
	can_attack = true;

	// Remove any timers on this component
	GetOwner()->GetWorldTimerManager().ClearAllTimersForObject(this);
	
	// Setup tracing parameters
	hit_trace_params.ClearIgnoredActors();
	hit_trace_params.AddIgnoredActor(GetOwner());
}

