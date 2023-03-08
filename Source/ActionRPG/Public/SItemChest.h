// Fill out your copyright notice in the Description page of Project Settings.
// Purpose: Simple object that mimicks a treasure chest. It has two static meshes (Base and Lid) and inherits the ISGameplayInterface which is for interactable
// objects. Notes - The Interact_Implementation method comes from the Gameplayinterface class (_Implementation part is REQUIRED to use it)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SItemChest.generated.h"

UCLASS()
class ACTIONRPG_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere)
	float TargetPitch;

	// Implementation that comes from the ISGameplayInterface header
	// _Implementation is important here BECAUSE Native was specified in the original header (SGameplayInterface.h)
	void Interact_Implementation(APawn* InstigatorPawn);


public:	
	// Sets default values for this actor's properties
	ASItemChest();

protected:

	UPROPERTY(ReplicatedUsing="OnRep_LidOpened", BlueprintReadOnly)		//RepNotify
	bool blidOpen;

	UFUNCTION()
	void OnRep_LidOpened();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* LidMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
