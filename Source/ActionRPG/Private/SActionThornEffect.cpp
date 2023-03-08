// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionThornEffect.h"
#include "SAttributeComponent.h"
#include "SActionComponent.h"
#include "SGameplayFunctionLibrary.h"

USActionThornEffect::USActionThornEffect()
{
	ReflectFraction = 0.2f;

	Duration = 0.0f;
	Period = 0.0f;
}

void USActionThornEffect::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);

	// Start Listening
	USAttributeComponent* Attri = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (Attri)
	{
		Attri->OnHealthChanged.AddDynamic(this, &USActionThornEffect::OnHealthChanged);
	}
}

void USActionThornEffect::StopAction_Implementation(AActor* Instigator)
{
	Super::StopAction_Implementation(Instigator);

	USAttributeComponent* Attri = USAttributeComponent::GetAttributes(GetOwningComponent()->GetOwner());
	if (Attri)
	{
		Attri->OnHealthChanged.RemoveDynamic(this, &USActionThornEffect::OnHealthChanged);
	}
}

void USActionThornEffect::OnHealthChanged(AActor* Instigator, USAttributeComponent* OwningComp, float NewValue, float Delta)
{
	AActor* OwningActor = GetOwningComponent()->GetOwner();

	// Damage Only
	if (Delta < 0.0f && OwningActor != Instigator)
	{
		// Round to nearest to avoid 'ugly' damage numbers and tiny refelections
		int32 ReflectAmount = FMath::RoundToInt(Delta * ReflectFraction);
		if (ReflectAmount == 0)
		{
			return;
		}

		// Flip to positive, so we don't end up healing ourselves when passed into damage
		ReflectAmount = FMath::Abs(ReflectAmount);

		// Return damage sender
		USGameplayFunctionLibrary::ApplyDamage(OwningActor, Instigator,ReflectAmount);
	}
}