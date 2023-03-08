// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SBarrelActor.generated.h"

class UStaticMeshComponent;
class URadialForceComponent;

UCLASS()
class ACTIONRPG_API ASBarrelActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASBarrelActor();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	URadialForceComponent* RadialForceComp;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
