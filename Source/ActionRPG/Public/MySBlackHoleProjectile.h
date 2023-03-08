// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SBaseProjectile.h"
#include "MySBlackHoleProjectile.generated.h"

class USphereComponent;
class URadialForceComponent;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API AMySBlackHoleProjectile : public ASBaseProjectile
{
	GENERATED_BODY()
	
public:

	AMySBlackHoleProjectile();

	UPROPERTY(BlueprintReadOnly, VisibleAnyWhere)
	USphereComponent* SphereComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	URadialForceComponent* RadialComp;

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
