// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SMiscellanousAIComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONRPG_API USMiscellanousAIComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USMiscellanousAIComponent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Credits")
	float KillReward;

	UPROPERTY(BlueprintReadOnly, Category = "Helpers")
	bool Spawned;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


};
