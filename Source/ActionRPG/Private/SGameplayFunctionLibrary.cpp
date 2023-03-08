// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayFunctionLibrary.h"
#include "SAttributeComponent.h"

bool USGameplayFunctionLibrary::ApplyDamage(AActor* DamageCauser, AActor* TargetActor, float Amount)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(TargetActor);

	if (AttributeComp)
	{
		return AttributeComp->ApplyHealthChange(DamageCauser, -Amount);
	}
	else
		return false;
}

bool USGameplayFunctionLibrary::ApplyDirectionalDamage(AActor* DamageCauser, AActor* TargetActor, float Amount, const FHitResult& result)
{
	if (ApplyDamage(DamageCauser, TargetActor, Amount))
	{
		UPrimitiveComponent* HitComp = result.GetComponent();

		if (HitComp && HitComp->IsSimulatingPhysics(result.BoneName) )
		{
			// Direction = Target - Origin
			FVector Direction = result.TraceEnd - result.TraceStart;
			Direction.Normalize();

			// the magnitude (3330000) could be a console variable
			HitComp->AddImpulseAtLocation(Direction * 330000.f, result.ImpactPoint, result.BoneName);
		}

		return true;
	}

	return false;
}
