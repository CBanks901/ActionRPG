// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "SGameModeBase.generated.h"

/**
 * 
 */

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;

UCLASS()
class ACTIONRPG_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	FTimerHandle TimerHanlde_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* killer);

	ASGameModeBase();

	virtual void StartPlay() override;

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION(Exec)
	void KillAll();
};
