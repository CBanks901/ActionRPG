// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SMagicProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;
class UAudioComponent;
class USActionEffect;
class USActionThornEffect;

UCLASS()
class ACTIONRPG_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USphereComponent* SphereComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UProjectileMovementComponent* MovementComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UParticleSystemComponent* EffectComp;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UAudioComponent* Flight;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UAudioComponent* ImpactSound;

	UPROPERTY(EditAnywhere, Category = "Damage")
	float Damage;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UParticleSystem* StartEmitter;

	UFUNCTION(BlueprintCallable)
	void Explode();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UParticleSystem* ImpactVFX;

	UPROPERTY(EditAnywhere)
	float ImpactShakeInnerRadius;

	UPROPERTY(EditAnywhere)
	float ImpactShakeOuterRadius;

	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> BurningActionClass;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionThornEffect> ThornsActionClass;

	UPROPERTY(EditDefaultsOnly, Category = "Camera")
	TSubclassOf<UCameraShakeBase> CameraShakeClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
