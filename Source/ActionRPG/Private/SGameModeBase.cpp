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

// create a console cheat that we can use to stop or continue spawning of AI bots
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true, TEXT("Enabling spawning of bots via timer"), ECVF_Cheat);


ASGameModeBase::ASGameModeBase()
{
	// Initialize all basic variables
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

	// Parse the data coming from the blueprint call when trying to open a new level from blueprint. Varaibles matches 'SaveGame' here
	FString SelectGameSlot = UGameplayStatics::ParseOption(Options, "SaveGame");

	if (SelectGameSlot.Len() > 0)
	{
		SlotName = SelectGameSlot;
	}
	
	// Call LoadSaveGame to pull in actor positions and save data
	LoadSaveGame();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	LoadInHidingSpots();

	// Create the spawn timer that constantly spawns in actors 
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
	//Purpose = Randomly select a type of actor from the data table MonsterTable and then attempt to load it based on its assetID
	
	
	// if the query wasn't successfull, don't do anything else
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		//UE_LOG(LogTemp, Log, TEXT("Spawn bot query failed!!"));
		return;
	}

	// Get all the locations of the query and store them into a TArray of FVectors called Locations
	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// Ensure the location numbers are greater than 0 and that a data table for monster has been set
	if (Locations.Num() > 0)
	{
		if (MonsterTable)
		{
			// An array of FMonsterTableRows being pulled and stored for Rows
			TArray<FMonsterTableRow*> Rows;
			MonsterTable->GetAllRows("", Rows);

			// Grab a random index from the rows variable and store that into a SelectRow variable
			int32 RandIndex = FMath::RandRange(0, Rows.Num() - 1);
			FMonsterTableRow* SelectRow = Rows[RandIndex];

			// Store the asset manager
			UAssetManager* AssetManager = UAssetManager::GetIfValid();

			if (AssetManager)
			{
				LogOnScreen(this, "Loading monster....", FColor::Green);
				// Necessary variable for Asset Manager
				TArray<FName> Bundles;

				// Create a streamable delegate that contains all the data we need as the actor is spawned in
				FStreamableDelegate Delegate = FStreamableDelegate::CreateUObject(this, &ASGameModeBase::OnMonsterLoaded, SelectRow->MonsterID, Locations[0], SelectRow->KillReward);
				
				// Load the primary asset using the id within the row we selected, the bundles variable and the delegate we just made
				AssetManager->LoadPrimaryAsset(SelectRow->MonsterID, Bundles, Delegate);
			}

			
		}

		DrawDebugSphere(GetWorld(), Locations[0], 50.f, 20, FColor::Blue, false, 60.0f);
	} 
}

void ASGameModeBase::OnMonsterLoaded(FPrimaryAssetId LoadedId, FVector SpawnLocation, float Reward)
{
	// Purpose = Attempt to spawn a selected actor type in and then assign it the SpawnLocation (based on query) as well as the reward which should be 
	// based on the Monster table. Should follow the OnQueryCompleted function

	LogOnScreen(this, "Finished loading monster....", FColor::Green);
	UAssetManager* AssetManager = UAssetManager::GetIfValid();

	if (AssetManager)
	{
		// Get MonsterData based on the incoming assetId LoadedI
		USMonsterData* MonsterData = Cast<USMonsterData>(AssetManager->GetPrimaryAssetObject(LoadedId));

		if (MonsterData)
		{
			// Begin spawning the monster based on the spawnlocation, as well as the class which is stored in MonsterData
			// and ensure that it always spawn no matter what
			FActorSpawnParameters Fparams;
			Fparams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			AActor* NewBot = GetWorld()->SpawnActor<AActor>(MonsterData->MonsterClass, SpawnLocation, FRotator::ZeroRotator, Fparams);

			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green, *MonsterData->MonsterClass.Get()->GetFName().ToString());

			// If the new AI spot is successfull then...
			if (NewBot)
			{
				LogOnScreen(this, FString::Printf(TEXT("Spawned enemy: %s (%s)"), *GetNameSafe(NewBot), *GetNameSafe(MonsterData)));

				// Grants Buffs and Debuffs
				USActionComponent* ActionComp = Cast<USActionComponent>(NewBot->GetComponentByClass(USActionComponent::StaticClass()));

				// If the ActionComp is valid then for every action within the MonsterData->Actions array.. add that to the ActionComp
				// This allows us to mimic it as an A.I starts with these actions 
				if (ActionComp)
				{
					for (TSubclassOf<USAction> ActionClass: MonsterData->Actions)
					{
						ActionComp->AddAction(NewBot, ActionClass);
					}
				}
				
				// Get the miscellanous component specifically for killreward points/data and assign it the reward that was in the funciton call
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
	// If the gloabl spawn bots parameter is disabled, stop immediately
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot Spawning disabled via cvar 'CVarSpawnBots'"));
		return;
	}

	int32 NOfAliveBots = 0;

	// Iterates through the number of actors based on ASAICharacter in the world and get their attribute components to determine 
	// if the actor is alive or dead
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

	// If the difficultly curve is set, increase MaxCount based on the amount of time currently passed in the world
	// More time = More possible enemies
	if (DifficultyCurve)
	{
		MaxCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds);
	}

	// Stops the timer from consantly spawning in new bots if the number of bots alive currently is equal to the limit.
	// Controllable in blueprint game mode
	if (NOfAliveBots >= MaxCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity. Skipping bot spawn"));
		return;
	}

	// Ensure that the SpawnBotQuery which should be set in blueprint is valid
	if (SpawnBotQuery)
	{
		// Create a eqs query and then bind the finished state of the query to a custom function called OnQueryCompleted which handles the end
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);
		if ensure((QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnQueryCompleted);
		}
	}
	else
	{
		// Clear the spawn timer since it is no longer possible to use it
		GetWorld()->GetTimerManager().ClearTimer(TimerHanlde_SpawnBots);
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	// Ensuring that the incoming controller is valid
	// unpossess it and then restart the player with a new one basically restarting the process
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
			//break;	// single player only
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
	}
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

	// Purpose - Allows us to load in the PlayerState in a way that happens before the game starts
	
	NewPlayer->PlayerState.Get()->SetPlayerName(NewPlayer->GetPawn()->GetName());

	// Get the player state of the player and then call LoadPlayerState based on the current save data
	ASPlayerState* PS = NewPlayer->GetPlayerState<ASPlayerState>();

	UE_LOG(LogTemp, Warning, TEXT("Player State Name: %s"), *PS->GetPlayerName());
	
	if (PS)
	{
		PS->LoadPlayerState(CurrentSaveGame);
		UE_LOG(LogTemp, Warning, TEXT("Player state '%s' s credits %s"), *PS->GetPlayerName(), *FString::FromInt(PS->GetCredits()));
	}
}

AGameStateBase* ASGameModeBase::GetCurrentGameState()
{
	return GameState;
}
