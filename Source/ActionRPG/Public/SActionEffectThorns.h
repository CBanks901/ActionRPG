// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "GameplayTagContainer.h"
#include "SActionEffectThorns.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API USActionEffectThorns : public USActionEffect
{
	GENERATED_BODY()
	
public:
	void StartAction_Implementation(AActor* Instigator) override;

	void StopAction_Implementation(AActor* Instigator) override;


/*protected:
	UFUNCTION()
	void ExecutePeriodicEffect_Implementation(AActor* Instigator) override;
	*/
public:
	USActionEffectThorns();
};
