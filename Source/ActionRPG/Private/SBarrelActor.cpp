// Fill out your copyright notice in the Description page of Project Settings.


#include "SBarrelActor.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"

// Sets default values
ASBarrelActor::ASBarrelActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForce");

	RootComponent = StaticMeshComp;
	RadialForceComp->SetupAttachment(StaticMeshComp);
}

// Called when the game starts or when spawned
void ASBarrelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASBarrelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

