// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SAction_ProjectileAttack.generated.h"

/**
 * 
 */

class UAnimMontage;
class UParticleSystem;

UCLASS()
class ACTIONRPG_API USAction_ProjectileAttack : public USAction
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(VisibleAnywhere, Category = "Effects")
	FName HandSocketName;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float AttackAnimDelay;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* CastingEffect;

	UFUNCTION()
	void SpawnDelay_Elapsed(ACharacter* InstigatorCharacter);

	UFUNCTION()
	void AttackDelay_Elapsed(ACharacter* InstigatorCharacter);


	UFUNCTION()
	void OnProjectileDataLoaded(FPrimaryAssetId LoadedId, AActor* Instigator);

	UPROPERTY(EditDefaultsOnly, Category = "Data Assets")
	FPrimaryAssetId Projectile_ID;

public:

	virtual void StartAction_Implementation(AActor* Instigator) override;

	USAction_ProjectileAttack();
};
