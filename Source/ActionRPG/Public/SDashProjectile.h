// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseProjectile.h"
#include "SDashProjectile.generated.h"

/**
 * 
 */

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class ACTIONRPG_API ASDashProjectile : public ASBaseProjectile
{
	GENERATED_BODY()

public:
	ASDashProjectile();

	UPROPERTY(EditAnywhere)
	USphereComponent* sphereComp;

protected:


	// Even though these inherit from the ASBaseProjectile class we still need to create our own to use them
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
};
