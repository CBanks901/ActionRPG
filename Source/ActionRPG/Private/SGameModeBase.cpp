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
#include "Kismet/GameplayStatics.h"
#include "MySaveGame.h"
#include "SPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "SGameplayInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "SMonsterData.h"
#include <ActionRPG/ActionRPG.h>
#include "SActionComponent.h"
#include <Engine/AssetManager.h>
#include "Engine/TargetPoint.h"
#include "SGameplayFunctionLibrary.h"
#include "Components/BillboardComponent.h"
#include "SMiscellanousAIComponent.h"

static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enabling spawning of bots via timer"), ECVF_Cheat);


ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	CreditsPerKill = 20;

	DesiredPowerUpCount = 10;
	RequiredPowerUpDistance = 2000;

	PlayerStateClass = ASPlayerState::StaticClass();

	SlotName = "SaveGame01";
}

void ASGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	FString SelectGameSlot = UGameplayStatics::ParseOption(Options, "SaveGame");

	if (SelectGameSlot.Len() > 0)
	{
		SlotName = SelectGameSlot;
	}
	
	LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	LoadInHidingSpots();

	GetWorldTimerManager().SetTimer(TimerHanlde_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed, SpawnTimerInterval, true);
}

void ASGameModeBase::LoadInHidingSpots()
{
	ATargetPoint* Point;
	if (!Targets.IsEmpty())
	{
		for (TSoftObjectPtr<ATargetPoint> Points : Targets)
		{
			if (Points.IsValid())
			{			
				Point = Points.LoadSynchronous();
				TArray<UActorComponent*> BillBoards = Point->GetComponentsByClass(UBillboardComponent::StaticClass());

				if (BillBoards.Num() > 0)
				{
					for (UActorComponent* Boards : BillBoards)
					{
						UBillboardComponent* BillBoard = Cast<UBillboardComponent>(Boards);

						if (BillBoard)
						{
							StoredHidingSpots.Add(BillBoard->GetComponentLocation());
						}
					}
				}
			}
		}
	}
}

void ASGameModeBase::OnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{

	if (QueryStatus != EEnvQueryStatus::Success)
	{
		//UE_LOG(LogTemp, Log, TEXT("Spawn bot query failed!!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.Num() > 0)
	{
		if (MonsterTable)
		{

			TArray<FMonsterTableRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			int32 RandIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterTableRow* SelectRow = Rows[RandIndex];

			UAssetManager* AssetManager = UAssetManager::GetIfValid();

			if (AssetManager)
			{
				LogOnScreen(this, "Loading monster....", FColor::Green);
				TArray<FName> Bundles;

				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectRow->MonsterID, Locations[0], SelectRow->KillReward);
				AssetManager->LoadPrimaryAsset(SelectRow->MonsterID, Bundles, Delegate);
			}

			
		}

		DrawDebugSphere(GetWorld(), Locations[0], 50.f, 20, FColor::Blue, false, 60.0f);
	} 
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation, float Reward)
{
	LogOnScreen(this, "Finished loading monster....", FColor::Green);
	UAssetManager* AssetManager = UAssetManager::GetIfValid();

	if (AssetManager)
	{
		USMonsterData* MonsterData = Cast<USMonsterData>(AssetManager->GetPrimaryAssetObject(LoadedId));

		if (MonsterData)
		{
			FActorSpawnParameters Fparams;
			Fparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator, Fparams);

			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, *MonsterData->MonsterClass.Get()->GetFName().ToString());

			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grants Buffs and Debuffs
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));

				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass: MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
				
				USMiscellanousAIComponent* AIComp = NewBot->FindComponentByClass<USMiscellanousAIComponent>();
				AIComp->Spawned = true;
				if (AIComp)
				{
					AIComp->KillReward = Reward;
				}
				
			}
		}
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
	else
	{
		//resume = true;
		for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
		{
			ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);

			if (PS->GetPlayerName() == *GetNameSafe(killer) )
			{
				UE_LOG(LogTemp, Warning, TEXT("Killer equals player..."));
			}

			if (PS)
			{
				USMiscellanousAIComponent* AIComp = VictimActor->FindComponentByClass<USMiscellanousAIComponent>();
				if (AIComp)
				{
					float Reward = AIComp->KillReward;
					PS->AddCredits(Reward);
				}

				/*if (MonsterTable)
				{
					TArray<FMonsterTableRow*> Rows;
					MonsterTable->GetAllRows("", Rows);

					for (FMonsterTableRow* Row : Rows)
					{
						if (resume)
						{
							FStreamableDelegate del = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::GetSpawnedClass, Row->MonsterID, PS, VictimActor, Row->KillReward);
							TArray<FName> Bundle;
							Bundle.Add("ClassBundle");

							UAssetManager* AssetManager = UAssetManager::GetIfValid();

							AssetManager->LoadPrimaryAsset(Row->MonsterID, Bundle, del);
						}
						else
							break;
					}
				}
			}*/
			}
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("OnActor Killed! Victim: %s Killer: %s "), *GetNameSafe(player), *GetNameSafe(killer) );
}

void ASGameModeBase::GetSpawnedClass(FPrimaryAssetId Id, ASPlayerState* PS, AActor* VictimActor, float Reward)
{
	UAssetManager* AssetManager = UAssetManager::GetIfValid();

	USMonsterData* Monster = Cast<USMonsterData>(AssetManager->GetPrimaryAssetObject(Id));

	if (VictimActor->GetClass() == Monster->MonsterClass)
	{
		PS->AddCredits(Reward);
		resume = false;
	}
}

void ASGameModeBase::WriteSaveGame()
{

	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			PS->SavePlayerState(CurrentSaveGame);
			break;	// single player only
		}
	}

	CurrentSaveGame->SavedActors.Empty();

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		//Only use actors that have the SGameplayInterface 
		if (!Actor->Implements<USGameplayInterface>())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetName();
		ActorData.Transform = Actor->GetTransform();

		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY in game
		Ar.ArIsSaveGame = true;
		// Converts actors SaveGame properties to binary
		Actor->Serialize(Ar);

		CurrentSaveGame->SavedActors.Add(ActorData);
	}

	if (!UGameplayStatics::SaveGameToSlot(CurrentSaveGame, SlotName, 0) )
	{
		UE_LOG(LogTemp, Warning, TEXT("Something went wrong"));
	};
}

void ASGameModeBase::LoadSaveGame()
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		CurrentSaveGame = Cast<UMySaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0) );

		if (CurrentSaveGame == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to load SaveGame Data. ") ) ;
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Successfully loaded SaveGame Data. "));

		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			//Only use actors that have the SGameplayInterface 
			if (!Actor->Implements<USGameplayInterface>())
			{
				continue;
			}


			for (FActorSaveData ActorData : CurrentSaveGame->SavedActors)
			{
				if (ActorData.ActorName == Actor->GetName())
				{
					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					// Find only variables with UPROPERTY in game
					Ar.ArIsSaveGame = true;
					// Converts actors from Binary to SaveGame
					Actor->Serialize(Ar);

					ISGameplayInterface::Execute_OnActorLoaded(Actor);

					Actor->SetActorTransform( ActorData.Transform);
					break;
				}
			}
		}
	}
	else
	{
		CurrentSaveGame = Cast<UMySaveGame>(UGameplayStatics::CreateSaveGameObject(UMySaveGame::StaticClass()));
		
		UE_LOG(LogTemp, Warning, TEXT("Successfully created SaveGame Data. "));
	}
}

void ASGameModeBase::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	ASPlayerState* PS = Cast<ASPlayerState>(NewPlayer->PlayerState);

	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
	}
}
