// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SCoinCreditActor.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API ASCoinCreditActor : public ASPowerupActor
{
	GENERATED_BODY()
	
public:
	ASCoinCreditActor();

	FText GetInteractText_Implementation(APawn* InstigatorPawn);

	void Interact_Implementation(APawn* InstigatorPawn);
		
	void ShowPowerup();

	virtual void OnRep_IsActive(APawn* InstigatorPawn);

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Credits")
	float CreditsAward;
};
