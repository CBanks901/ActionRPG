// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/EnvQueryInstanceBlueprintWrapper.h"
#include "AI/SAICharacter.h"
#include "SAttributeComponent.h"
#include "EngineUtils.h"
#include "Curves/CurveFloat.h"
#include "DrawDebugHelpers.h"
#include "SUCharacter.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enabling spawning of bots via timer"), ECVF_Cheat);


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	GetWorldTimerManager().SetTimer(TimerHanlde_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{

	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Log, TEXT("Spawn bot query failed!!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		FActorSpawnParameters Fparams;
		Fparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator, Fparams);

		DrawDebugSphere(GetWorld(), Locations[0], 50.f, 20, FColor::Blue, false, 60.0f);
	} 
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> it(GetWorld()); it; ++it)
	{
		ASAICharacter* Bot = *it;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

		if (ensure(AttributeComp) && AttributeComp->isAlive())
		{
			AttributeComp->Kill(this);		// @fixme this can be the player for kill credit
		}

	}
}

void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot Spawning disabled via cvar 'CVarSpawnBots'"));
		return;
	}

	int32 NOfAliveBots = 0;
	// Iterates through the number of actors in the world
	for (TActorIterator<ASAICharacter> it(GetWorld()); it; ++it)
	{
		ASAICharacter* Bot = *it;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

		if (ensure(AttributeComp) && AttributeComp->isAlive())
		{
			NOfAliveBots++;
		}

	}

	UE_LOG(LogTemp, Warning, TEXT("Number of alive bots: %i"), NOfAliveBots);

	float MaxCount = 10.0f;

	if (DifficultyCurve)
	{
		MaxCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	if (NOfAliveBots >= MaxCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn"));
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
	if ensure((QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		Controller->UnPossess();

		RestartPlayer(Controller);
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* killer)
{
	ASUCharacter* player = Cast<ASUCharacter>(VictimActor);

	if (player)
	{
		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", player->GetController() );

		float respawndelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, respawndelay, false );
	}

	UE_LOG(LogTemp, Warning, TEXT("OnActor Killed! Victim: %s Killer: %s "), *GetNameSafe(player), *GetNameSafe(killer) );
}