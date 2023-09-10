// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "MySaveGame.h"






bool ASPlayerState::AddCredits(int32 Delta)
{
	// Avoid user-error of adding a negative amount
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}

	Credits += Delta;

	OnCreditsChanged.Broadcast(this, Credits, Delta);

	return true;
}

bool ASPlayerState::RemoveCredits(int32 Delta)
{
	// Avoid user-error of adding a subtracting negative amount
	if (!ensure(Delta >= 0.0f))
	{
		return false;
	}

	if (Credits < Delta)
	{
		// Not enough credits available
		return false;
	}

	Credits -= Delta;

	OnCreditsChanged.Broadcast(this, Credits, -Delta);

	return true;
}

void ASPlayerState::SavePlayerState_Implementation(UMySaveGame* SaveObject)
{
	if (SaveObject)
	{
		//SaveObject->Credits = Credits;
		if (SaveObject->PlayerCredits.Contains(GetPlayerName()))
			*SaveObject->PlayerCredits.Find(GetPlayerName()) = Credits;
		else
			SaveObject->PlayerCredits.Add(GetPlayerName(), Credits);

		UE_LOG(LogTemp, Warning, TEXT("%s 's credits: %s"), *GetPlayerName(), *FString::FromInt(Credits));
	}
}

void ASPlayerState::LoadPlayerState_Implementation(UMySaveGame* SaveObject)
{
	if (SaveObject)
	{
		// If the playercredits variable isn't empty and this name exists inside of it then proceed
		if (!SaveObject->PlayerCredits.IsEmpty() && SaveObject->PlayerCredits.Contains(GetPlayerName() ))
		{
			//Credits = SaveObject->Credits;
			Credits = *SaveObject->PlayerCredits.Find(GetPlayerName());
		}
		else
		{
			// If the playercredits object inside the save game is empty and or this player doesn't exist inside of it, then simply 
			// set it's credits to 0 on load
			Credits = 0;
		}
	}
}

/*
void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}
*/

int32 ASPlayerState::GetCredits() const
{
	return Credits;
}


/*
void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsChanged.Broadcast(this, Credits, Credits - OldCredits);
}
*/
