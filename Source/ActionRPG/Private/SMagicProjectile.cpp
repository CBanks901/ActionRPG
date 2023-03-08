// Fill out your copyright notice in the Description page of Project Settings.


#include "SMagicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "SAttributeComponent.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "SGameplayFunctionLibrary.h"
#include "SActionComponent.h"
#include "SActionEffect.h"
#include "SActionThornEffect.h"
//#include "SActionThornEffect.h"

// Sets default values
ASMagicProjectile::ASMagicProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	RootComponent = SphereComp;
	 
	SphereComp->SetCollisionProfileName("Projectile");
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ASMagicProjectile::OnActorOverlap);

	// If we wanted to create a new overlap from c++ only these lines would be needed in addition to the profile name
	/*
	SphereComp->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	SphereComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	*/

	EffectComp = CreateDefaultSubobject<UParticleSystemComponent>("EffectComp");
	EffectComp->SetupAttachment(SphereComp);

	MovementComp = CreateDefaultSubobject<UProjectileMovementComponent>("MovementComp");
	MovementComp->InitialSpeed = 1000.0f;
	MovementComp->bRotationFollowsVelocity = true;
	MovementComp->bInitialVelocityInLocalSpace = true;

	Flight = CreateDefaultSubobject<UAudioComponent>("FlightComp");


	ImpactSound = CreateDefaultSubobject<UAudioComponent>("ImpactSound");

	StartEmitter = CreateDefaultSubobject<UParticleSystem>("StartEmitter");
	

	Damage = -20.0f;
	SetReplicates(true);
}

// Called when the game starts or when spawned
void ASMagicProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (!Flight->Sound.IsNull() )
		Flight->Play();

	SphereComp->IgnoreActorWhenMoving(GetInstigator(),true);

	if (StartEmitter)
		UGameplayStatics::SpawnEmitterAttached(StartEmitter, SphereComp);
}

// Called every frame
void ASMagicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASMagicProjectile::OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator() )
	{
		USActionComponent* ActionComp = Cast<USActionComponent>(OtherActor->GetComponentByClass(USActionComponent::StaticClass()));

		if (ActionComp && ActionComp->ActiveGameplayTags.HasTag(ParryTag))
		{
			MovementComp->Velocity = -MovementComp->Velocity;

			SetInstigator(Cast<APawn>(OtherActor));

			return;
		}

		USAttributeComponent* attributeComp = Cast<USAttributeComponent>(OtherActor->GetComponentByClass(USAttributeComponent::StaticClass() ) );

		if (attributeComp)
		{
			if (Damage < 0.0f)
			{
				if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, -Damage, SweepResult))
				{
					// @fixme Should explode here

					if (ActionComp)
					{
						//ActionComp->AddAction(GetInstigator(), BurningActionClass);
						ActionComp->AddAction(GetInstigator(), ThornsActionClass);
					}

					if (CameraShakeClass)
						UGameplayStatics::PlayWorldCameraShake(GetWorld(), CameraShakeClass, OtherActor->GetActorLocation(), 0.0f, 8000.f);

				}
			}
			else
			{
				// Healing effect
				if (USGameplayFunctionLibrary::ApplyDirectionalDamage(GetInstigator(), OtherActor, Damage, SweepResult)) 
				{
					// @fixme Should explode here
				}
			}

			if (!ImpactSound->Sound.IsNull() )
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound->Sound, GetActorLocation());

			Destroy();
		}
	}
}

