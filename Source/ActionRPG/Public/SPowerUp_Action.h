// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActor.h"
#include "SPowerUp_Action.generated.h"

/**
 * 
 */

class USAction;

UCLASS()
class ACTIONRPG_API ASPowerUp_Action : public ASPowerupActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "PowerUp")
	TSubclassOf<USAction> ActionToGrant;

public:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	FText GetInteractText_Implementation(APawn* InstigatorPawn);
};
