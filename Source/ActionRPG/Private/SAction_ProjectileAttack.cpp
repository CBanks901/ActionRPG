// Fill out your copyright notice in the Description page of Project Settings.


#include "SAction_ProjectileAttack.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include <Engine/AssetManager.h>
#include "SProjectileDataAsset.h"
#include "SBaseProjectile.h"

USAction_ProjectileAttack::USAction_ProjectileAttack()
{
	AttackAnimDelay = 2.0f;
	HandSocketName = "Muzzle_01";
}


void USAction_ProjectileAttack::StartAction_Implementation(AActor* Instigator)
{
	Super::StartAction_Implementation(Instigator);
	//USAction::StartAction_Implementation(Instigator);

	UAssetManager* AssetManager = UAssetManager::GetIfValid();
	TArray<FName> Bundles; 
	
	FStreamableDelegate AssetDelegate = FStreamableDelegate::CreateUObject(this, &USAction_ProjectileAttack::OnProjectileDataLoaded, Projectile_ID, Instigator);
	AssetManager->LoadPrimaryAsset(Projectile_ID, Bundles, AssetDelegate);
}

void USAction_ProjectileAttack::SpawnDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	if (ensureAlways(ProjectileClass))
	{
		GEngine->AddOnScreenDebugMessage(- 1, 10, FColor::White, *GetNameSafe(ProjectileClass) );

		FVector HandLocation = InstigatorCharacter->GetMesh()->GetSocketLocation(HandSocketName);

		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = InstigatorCharacter;
		SpawnParams.Owner = InstigatorCharacter;

		FCollisionShape Shape;
		Shape.SetSphere(20.0f);

		// Ignore Player
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(InstigatorCharacter);

		FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
		ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjectParams.AddObjectTypesToQuery(ECC_Pawn);

		FVector TraceStart = InstigatorCharacter->GetPawnViewLocation();

		FVector TraceEnd = TraceStart + (InstigatorCharacter->GetControlRotation().Vector() * 5000);

		FHitResult Hit;

		// returns true if we got a blocking hit
		if (GetWorld()->SweepSingleByObjectType(Hit, TraceStart, TraceEnd, FQuat::Identity, ObjectParams, Shape, Params))
		{
			// Overwrite trace end with impact point in world
			TraceEnd = Hit.ImpactPoint;
		}

		// Find new direction/rotation from Hand pointing to impact point in world.
		FRotator ProjRotation = FRotationMatrix::MakeFromX(TraceEnd - HandLocation).Rotator();

		FTransform SpawnTM = FTransform(ProjRotation, HandLocation);
		AActor* ProjectileRef = GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
		
		// We need to call getsuperclass twice IF the instance is a blueprint
		// GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("Base class static: %s"), *GetNameSafe(ProjectileClass->GetSuperClass()->GetSuperClass())));
		
		bool bpBase = ProjectileClass->IsInBlueprint();
		if (bpBase)
		{
			if (ProjectileClass->GetSuperClass()->GetSuperClass() == ASBaseProjectile::StaticClass())
			{
				ASBaseProjectile* BaseProj = Cast<ASBaseProjectile>(ProjectileRef);
				BaseProj->TraceEndPoint = TraceEnd;
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Blue, FString::Printf(TEXT("Base class static: %s"), *GetNameSafe(ProjectileClass)));
		}
	}

	//StopAction(InstigatorCharacter);


}

void USAction_ProjectileAttack::OnProjectileDataLoaded(FPrimaryAssetId LoadedId, AActor* Instigator)
{
	UAssetManager* AssetManager = UAssetManager::GetIfValid();

	USProjectileDataAsset* ProjectileAsset = Cast<USProjectileDataAsset>(AssetManager->GetPrimaryAssetObject(LoadedId) );


	if (ProjectileAsset)
	{
		if (ProjectileAsset->Montages[0])
		{
			AttackAnim = ProjectileAsset->Montages[0];
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Projectile Asset Montage unable to load. USAction_ProjectileAttack::OnProjectileDataLoaded."));
			return;
		}

		if (ProjectileAsset->Particle[0])
			CastingEffect = ProjectileAsset->Particle[0];
		else
			UE_LOG(LogTemp, Warning, TEXT("Projectile Asset particle unable to load. USAction_ProjectileAttack::OnProjectileDataLoaded."));

		if (ProjectileAsset->ProjectileClass)
			ProjectileClass = ProjectileAsset->ProjectileClass;
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Projectile Asset class unable to load. USAction_ProjectileAttack::OnProjectileDataLoaded."));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile Asset unable to load. USAction_ProjectileAttack::OnProjectileDataLoaded."));
		return;
	}


	ACharacter* Character = Cast<ACharacter>(Instigator);
	if (Character)
	{
		Character->PlayAnimMontage(AttackAnim);

		UGameplayStatics::SpawnEmitterAttached(CastingEffect, Character->GetMesh(), HandSocketName, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::SnapToTarget);

		if (Character->HasAuthority())
		{
			FTimerHandle TimerHandle_SpawnDelay;
			FTimerDelegate Delegate;
			Delegate.BindUFunction(this, "SpawnDelay_Elapsed", Instigator);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_SpawnDelay, Delegate, ProjectileAsset->SpawnTime, false);

			FTimerHandle TimerHandle_AttackDelay;
			FTimerDelegate StopDelegate;
			StopDelegate.BindUFunction(this, "AttackDelay_Elapsed", Instigator);

			GetWorld()->GetTimerManager().SetTimer(TimerHandle_AttackDelay, StopDelegate, AttackAnimDelay, false);
		}
	}
}

void USAction_ProjectileAttack::AttackDelay_Elapsed(ACharacter* InstigatorCharacter)
{
	StopAction(InstigatorCharacter);
}