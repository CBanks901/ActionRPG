// Fill out your copyright notice in the Description page of Project Settings.


#include "SActionComponent.h"
#include "SAction.h"


USActionComponent::USActionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}


void USActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	for (TSubclassOf<USAction> actionclass : DefaultActions)
	{
		AddAction(GetOwner(), actionclass);
	}
}


void USActionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FString DebugMessage = GetNameSafe(GetOwner()) + " : " + ActiveGameplayTags.ToStringSimple();
	GEngine->AddOnScreenDebugMessage(-1, 0, FColor::White, DebugMessage);
}

void USActionComponent::AddAction(AActor* Instigator, TSubclassOf<USAction> ActionClass)
{
	if (!ensure(ActionClass) )
	{
		return;
	}

	USAction* NewAction = NewObject<USAction>(this, ActionClass);
	if (ensure(NewAction) )
	{
		Actions.Add(NewAction);

		if (NewAction->bAutoStart && ensure(NewAction->CanStart(Instigator) ) )
		{
			NewAction->StartAction(Instigator);
		}
	}
}

bool USActionComponent::StartActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* action : Actions)
	{
		if (action && action->ActionName == ActionName)
		{
			if (!action->CanStart(Instigator))
			{
				FString FailedMsg = FString::Printf(TEXT("Failed to run: %s"), *ActionName.ToString());
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FailedMsg);
				continue;
			}

			// If this is the client
			if (!GetOwner()->HasAuthority())
			{
				ServerStartAction(Instigator, ActionName);
			}

			
			action->StartAction(Instigator);
			return true;
		}
	}

	return false;
}

bool USActionComponent::StopActionByName(AActor* Instigator, FName ActionName)
{
	for (USAction* action : Actions)
	{
		if (action && action->ActionName == ActionName)
		{
			if (action->IsRunning())
			{
				action->StopAction(Instigator);
				return true;
			}
		}
	}

	return false;
}

void USActionComponent::RemoveAction(USAction* ActionToRemove)
{
	if (!ensure(ActionToRemove && !ActionToRemove->IsRunning() ))
	{
		return;
	}

	Actions.Remove(ActionToRemove);
}

USAction* USActionComponent::GetAction(TSubclassOf<USAction> ActionClass) const
{
	
	for (USAction* actions : Actions)
	{
		if (actions && actions->ActionName == ActionClass.Get()->GetFName())
		{
			return actions;
		}
	}
	
	return nullptr;
}

void USActionComponent::ServerStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}

void USActionComponent::ClientStartAction_Implementation(AActor* Instigator, FName ActionName)
{
	StartActionByName(Instigator, ActionName);
}
