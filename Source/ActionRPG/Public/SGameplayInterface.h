// Fill out your copyright notice in the Description page of Project Settings.
// Purpose: Interaface class that is used to provide interface functionality for certain objects, like opening chests or other things. 

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SGameplayInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USGameplayInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONRPG_API ISGameplayInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/* Called after the Actor state was restored from a SaveGame file. */
	UFUNCTION(BlueprintNativeEvent)
	void OnActorLoaded();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FText GetInteractText(APawn* InstigatorPawn);

	// Blueprintimplementable event - only defined in Blueprint, even it if is in c++
	// Blueprintnative event - f0r both c++ and blueprint
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Interact(APawn* InstigatorPawn);
};
