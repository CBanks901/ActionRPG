// Fill out your copyright notice in the Description page of Project Settings.


#include "SDashProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/SphereComponent.h"
#include "SUCharacter.h"

ASDashProjectile::ASDashProjectile()
{
	// Make the sphere the root of this object
	sphereComp = CreateDefaultSubobject<USphereComponent>("Sphere Root");
	RootComponent = sphereComp;

	// Inherited from the base class we actually create it here
	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("Radical Force");
	EffectComp->SetupAttachment(RootComponent);

	// Same as EffectComp which is inherited from the base class
	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("Movement Comp");

	MovementComp->InitialSpeed = 6000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	// Lasts only a short while before it's deleted
	InitialLifeSpan = .4;
}

void ASDashProjectile::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("Spawning projectile %s"), *this->GetFullName() );
}

void ASDashProjectile::Destroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("Projectile being destroyed %s"), *this->GetFullName()  );

	
	// Get a reference to the owner of this actor and then set it's location this way.
	// Note: the owner is set inside the owning character when it's spawning by calling SetOwner at the end
	ASUCharacter* ownerRef = Cast<ASUCharacter>(GetOwner());

	if (ownerRef)
	{
		ownerRef->SetActorLocation(GetActorLocation());
	}
}
