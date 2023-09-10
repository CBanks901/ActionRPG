// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAttributeComponent.h"
#include "GameFramework/GameStateBase.h"
#include "SPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameModeBase.h"


#define LOCTEXT_NAMESPACE "InteractableActors" 

// Sets default values
ASHealthPotion::ASHealthPotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LastInstigator = NULL;

	CreditCost = 50;
}


// Called when the game starts or when spawned
void ASHealthPotion::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASHealthPotion::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{

	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Health potion check..");

	// Get the Instigator's player state so we can pull credit information from them

	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	ASPlayerState* InstigatorPS = InstigatorPawn->GetPlayerState<ASPlayerState>();

	/*for (int32 PlayerID = 0; PlayerID < GameMode->GameState->PlayerArray.Num(); PlayerID++)
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Name: %s"), *GameMode->GameState->PlayerArray[PlayerID].Get()->GetFName().ToString());

		if (InstigatorPawn->GetName() == GameMode->GameState->PlayerArray[PlayerID].Get()->GetPlayerName() )
		{
			InstigatorPS = Cast<ASPlayerState>(GameMode->GameState->PlayerArray[PlayerID].Get());
			UE_LOG(LogTemp, Warning, TEXT("SUCESS"));
			break;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Instigator name = %s    Current array name = %s"), *InstigatorPawn->GetName(),
				*GameMode->GameState->PlayerArray[PlayerID].Get()->GetPlayerName());
		}
	}*/

	if (InstigatorPS && InstigatorPS->GetCredits() >= CreditCost)
	{
		// Convert parameter from pawn to actor and attempt to get the attribute component from there
		AActor* Actor_Ref = Cast<AActor>(InstigatorPawn);
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Actor_Ref);

		// If health isn't full and the attribute component from the instigator is valid
		if (AttributeComp && !AttributeComp->IsFullHealth())
		{
			// Apply health change and remove the credit cost (50 by defult) from the instigator
			if (AttributeComp->ApplyHealthChange(this, AttributeComp->GetHealthToMax()))
			{
				InstigatorPS->RemoveCredits(CreditCost);

				// Set the incoming actor as the last instigator of this actor
				LastInstigator = Actor_Ref;

				// Hide the actor for next use
				HideAndCooldownPowerup();

				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, "Restoring health");
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Health already at maximum");
			}
		}

	}
	// Course Answer
	/*
	USAttributeComponent* AttriComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if (ensure(AttriComp) && !AttriComp->IsFullHealth())
	{
		if (ASPlayerState* PS = InstigatorPawn->GetPlayerState<ASPlayerState>())
		{
			if (PS->RemoveCredits(CreditCost) && AttriComp->ApplyHealthChange(this, AttriComp->GetHealthMax()) )
			{
				// Only activate if healed successfully
				HideAndCooldownPowerup();
			}
		}
	}
	*/
}

FText ASHealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	// Get the attribute component of the instigator for use below
	USAttributeComponent* AttributeComponent = USAttributeComponent::GetAttributes(InstigatorPawn);

	// If the instigator is already at full health, return this message
	if (AttributeComponent && AttributeComponent->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health");
	}

	// Check if the player state for the instigator has credits that are at least equal to the credit cost. If not return this error message
	if (InstigatorPawn->GetPlayerState<ASPlayerState>()->GetCredits() < CreditCost)
	{
		return LOCTEXT("HealthPotion_InsufficientFundsWarning", "Not enough credits to heal");
	}

	// Print this message by default if the above two conditions weren't triggered
	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditCost);
}

#undef LOCTEXT_NAMESPACE