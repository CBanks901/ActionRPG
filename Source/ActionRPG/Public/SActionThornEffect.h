// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionThornEffect.generated.h"

/**
 * 
 */

class USAttributeComponent;

UCLASS()
class ACTIONRPG_API USActionThornEffect : public USActionEffect
{
	GENERATED_BODY()
	
protected:
	float ReflectFraction;
	//float Period;
	//float Duration;

public:
	USActionThornEffect();

	UFUNCTION()
	void OnHealthChanged(AActor* Instigator, USAttributeComponent* OwningComp, float NewValue, float Delta);

	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;
};
