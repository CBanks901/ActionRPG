// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoinCreditActor.h"
#include "SPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "Net/UnrealNetwork.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ASCoinCreditActor::ASCoinCreditActor()
{
	CreditsAward = 80;
	bIsActive = true;
	bReplicates = true;
}

void ASCoinCreditActor::Interact_Implementation(APawn* InstigatorPawn)
{
	if (bIsActive)
	{
		// Get the player state coming from the InstigatorPawn parameter placed here
		ASPlayerState* TargetPs = Cast<ASPlayerState>(InstigatorPawn->GetPlayerState());
		
		// If that player state is valid proceed.
		if (TargetPs)
		{
			// Add the credits from here to the targets credit system and then hide this actor entirely
			TargetPs->AddCredits(CreditsAward);

			// This function will hide the collision of the actor, it's visibility and will also change the bIsActive variable to false
			// until it is initialized again
			HideAndCooldownPowerup();
		}
	}

}

FText ASCoinCreditActor::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	/*APlayerState;
	AGameStateBase *GameState = GetGameInstance()->GetWorld()->GetGameState();
	
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);
		if (PS)
		{
			
		}
	}*/

	if (!bIsActive)
		return LOCTEXT("Coin_CompletedMessage", "Credits Awarded.. destroying.");
	else
		return FText::Format(LOCTEXT("Coin_InteractMessage", "Awards {0} Credits."), CreditsAward);
}

void ASCoinCreditActor::OnRep_IsActive(APawn* InstigatorPawn)
{
	
	/*FTimerDelegate delo;
	delo.BindUFunction(this, "HideAndCooldownPowerup", NULL);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_RespawnTimer, delo, 10, false);
	//SetLifeSpan(15);
	AGameStateBase* GameState = GetGameInstance()->GetWorld()->GetGameState();
	APawn* Owner_Ref;
	for (int32 i = 0; i < GameState->PlayerArray.Num(); i++)
	{
		ASPlayerState* PS = Cast<ASPlayerState>(GameState->PlayerArray[i]);

		//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Red, FString::Printf(TEXT("Name of the current player character (Instigator Parameter): %s"), *GetNameSafe(InstigatorPawn) ) );
		//GEngine->AddOnScreenDebugMessage(-1, 20, FColor::Green, FString::Printf(TEXT("Name of the current player character (Player State): %s"), *GetNameSafe(PS->GetPlayerController()->GetPawn()) )   );
			
		if (PS)
		{
			Owner_Ref = PS->GetPlayerController()->GetPawn();
			if (InstigatorPawn == Owner_Ref)
			{
				PS->AddCredits(CreditsAward);
				break;
			}
		}
	*/
}
/*
void ASCoinCreditActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASCoinCreditActor, bIsActive);
}
*/
#undef LOCTEXT_NAMESPACE