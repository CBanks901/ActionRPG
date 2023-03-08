// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SHealthPotion.generated.h"

UCLASS()
class ACTIONRPG_API ASHealthPotion : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASHealthPotion();

	// Implementation that comes from the ISGameplayInterface header
	// _Implementation is important here BECAUSE Native was specified in the original header (SGameplayInterface.h)
	void Interact_Implementation(APawn* InstigatorPawn);

	UFUNCTION()
	void LockEffect(AActor* InstigatorObj);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* PotionMesh;

	bool TimeLock;
	FTimerHandle LockHandle;
};
