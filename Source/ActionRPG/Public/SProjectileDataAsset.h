// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SProjectileDataAsset.generated.h"

class UAnimMontage;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API USProjectileDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectiles")
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Montages")
	TArray<UAnimMontage*> Montages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Particle System")
	TArray<UParticleSystem*> Particle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnTime")
	float SpawnTime;

	FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Projectiles", GetFName());
	}
};
