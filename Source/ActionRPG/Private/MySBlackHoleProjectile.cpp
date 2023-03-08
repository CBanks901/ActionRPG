// Fill out your copyright notice in the Description page of Project Settings.


#include "MySBlackHoleProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"


AMySBlackHoleProjectile::AMySBlackHoleProjectile()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("Sphere Overlap");
	RootComponent = SphereComp;

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("Particle System");
	EffectComp->SetupAttachment(RootComponent);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("Movement Comp");
	MovementComp->InitialSpeed = 2000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;


	RadialComp = CreateDefaultSubobject<URadialForceComponent>("Radial Comp");
	RadialComp->SetupAttachment(RootComponent);

	// Make the overlap we made in the main class the overlap that is officially used in this component. 
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &AMySBlackHoleProjectile::OverlapBegin);

	InitialLifeSpan = 5.0f;
}

void AMySBlackHoleProjectile::OverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn *Pawn_Class = Cast<APawn>(OtherActor);

	if (!Pawn_Class)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlap detected"));
		OtherActor->Destroy();
	}
}
