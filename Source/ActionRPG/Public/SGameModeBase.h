// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "Engine/DataTable.h"
#include "SGameModeBase.generated.h"

/**
 * 
 */

class UEnvQuery;
class UEnvQueryInstanceBlueprintWrapper;
class UCurveFloat;
class UMySaveGame;
class UDataTable;
class USMonsterData;
class ATargetPoint;
class ASPlayerState;

USTRUCT(BlueprintType)
struct FMonsterTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:

	FMonsterTableRow()
	{
		Weight = 1.0f;
		SpawnCost = 0.0f;
		KillReward = 20.0f;
	}

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId MonsterID;
	//TSubclassOf<AActor> MonsterClass;

	/* Relative chance to pick this monster*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Weight;

	/* Points required by gamemode to spawn this unit*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpawnCost;

	/* Amount of credits rewarded to the killer of this unit*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float KillReward;
};

UCLASS()
class ACTIONRPG_API ASGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

protected:

	FString SlotName;

	UPROPERTY()
	UMySaveGame* CurrentSaveGame;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	float SpawnTimerInterval;

	FTimerHandle TimerHanlde_SpawnBots;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UEnvQuery* SpawnBotQuery;

	UFUNCTION()
	void SpawnBotTimerElapsed();

	/*UPROPERTY(EditDefaultsOnly, Category = "AI")
	TSubclassOf<AActor> MinionClass;*/

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UCurveFloat* DifficultyCurve;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	float RequiredPowerUpDistance;

	UPROPERTY(EditDefaultsOnly, Category = "PowerUps")
	int32 DesiredPowerUpCount;

	int32 CreditsPerKill;

	UFUNCTION()
	void RespawnPlayerElapsed(AController* Controller);

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	UDataTable* MonsterTable;

	void OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation, float Reward);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	TArray<TSoftObjectPtr<ATargetPoint>> Targets;

	UPROPERTY(BlueprintReadOnly, Category = "AI")
	TArray<FVector> StoredHidingSpots;

	UFUNCTION()
	void LoadInHidingSpots();

	UFUNCTION()
	void GetSpawnedClass(FPrimaryAssetId Id, ASPlayerState* PS, AActor* VictimActor, float Reward);

public:

	virtual void OnActorKilled(AActor* VictimActor, AActor* killer);

	ASGameModeBase();

	virtual void StartPlay() override;

	void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

	UFUNCTION()
	void OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus);

	UFUNCTION(Exec)
	void KillAll();

	UFUNCTION(BlueprintCallable, Category = "SaveGame")
	void WriteSaveGame();

	void LoadSaveGame();

	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;

	UFUNCTION(BlueprintCallable)
	AGameStateBase* GetCurrentGameState();

public:
	bool resume;
	
};
