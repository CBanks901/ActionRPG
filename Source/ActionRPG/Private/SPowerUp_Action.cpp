// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUp_Action.h"
#include "SActionComponent.h"
#include "SAction.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "InteractText"

void ASPowerUp_Action::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn && ActionToGrant))
	{
		return;
	}
	
	//USActionComponent* ActionComp = Cast<USActionComponent>(InstigatorPawn->GetComponentByClass(USActionComponent::StaticClass()));
	USActionComponent* ActionComp = InstigatorPawn->FindComponentByClass<USActionComponent>();

	if (ActionComp)
	{
		if (ActionComp->GetAction(ActionToGrant))
		{
			FString DebugMsg = FString::Printf(TEXT("Action '%s' already known."), *GetNameSafe(ActionToGrant) );
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, DebugMsg);
			return;
		}

		ActionComp->AddAction(InstigatorPawn, ActionToGrant);
		HideAndCooldownPowerup();
	}
}

FText ASPowerUp_Action::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		if (ensure(ActionToGrant))
		{
			USActionComponent* ActionComp = InstigatorPawn->FindComponentByClass<USActionComponent>();
			if (ActionComp)
			{
				FString GrantActionName = *GetNameSafe(ActionToGrant);
				GrantActionName.RemoveFromEnd("_c");

				if (ActionComp->GetAction(ActionToGrant) )
				{
					FText GrantActionName_Text = FText::FromString(GrantActionName);

					FFormatNamedArguments Args;
					Args.Add(TEXT("Name"), GrantActionName_Text);

					return FText::Format(LOCTEXT("InteractMessageFailure", "Action '{Name}' already granted."), Args);
				}
				else
				{
					return FText::FormatNamed(LOCTEXT("InteractMessageSuccess", "Grants '{Name}' tag"),
						TEXT("Name"), FText::FromString(GrantActionName)
					);
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("ActionToGrant for '%s' is blank.."), *GetNameSafe(this));
			return LOCTEXT("FailedInteract", "Error. ActionToGrant not set");
		}
	}

	return LOCTEXT("InteractMessage", "InstigatorPawn not valid or doesn't have ActionComponent");
}

#undef LOCTEXT