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

	RadialForceComp->SetAutoActivate(false);

	RadialForceComp->Radius = 750.0f;
	RadialForceComp->ImpulseStrength = 2500.0f;

	RadialForceComp->bImpulseVelChange = true;

	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);
}

// Called when the game starts or when spawned
void ASBarrelActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ASBarrelActor::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	StaticMeshComp->OnComponentHit.AddDynamic(this, &ASBarrelActor::OnComponentHit);
}

// Called every frame
void ASBarrelActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASBarrelActor::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	//RadialForceComp->FireImpulse();

	FString CombinedString = FString::Printf(TEXT("Hit location: %s"), *Hit.ImpactPoint.ToString() );

	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);
}