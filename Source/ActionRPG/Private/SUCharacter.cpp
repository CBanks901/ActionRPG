// Fill out your copyright notice in the Description page of Project Settings.


#include "SUCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "SInteractionComponents.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "MySBlackHoleProjectile.h"
#include "SDashProjectile.h"
#include "SAttributeComponent.h"
#include "SActionComponent.h"

// Sets default values
ASUCharacter::ASUCharacter()
{

 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>("Spring Arm Comp");
	SpringArmComp->SetupAttachment(RootComponent);
	SpringArmComp->bUsePawnControlRotation = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>("Camera Comp");
	CameraComp->SetupAttachment(SpringArmComp);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	
	InteractionComp = CreateDefaultSubobject<USInteractionComponents>("InteractionComp");

	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");

	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	
	lock = false;

	dashlock = false;

	bUseControllerRotationYaw = false;

}

void ASUCharacter::HealSelf(float Amount /* = 100 */)
{
	AttributeComp->ApplyHealthChange(this, Amount);
}

void ASUCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASUCharacter::OnHealthChanged);
}

// Called when the game starts or when spawned
void ASUCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ASUCharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (NewHealth <= 0.f && Delta < 0.0f)
	{
		APlayerController* playercontroller = Cast<APlayerController>(GetController() );
		DisableInput(playercontroller);
	}
	else if (Delta < 0.0f)
	{
		GetMesh()->SetScalarParameterValueOnMaterials("Time To Hit", GetWorld()->TimeSeconds);
		GetMesh()->SetScalarParameterValueOnMaterials("Speed", 0.5f);
		GetMesh()->SetVectorParameterValueOnMaterials("Color", FVector(1.0f, 0.0f, 0.0f));

		AttributeComp->AddRage(-Delta);
	}
}

// Called every frame
void ASUCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -- Rotation Visualization -- //
	const float DrawScale = 100.0f;
	const float Thickness = 5.0f;

	FVector LineStart = GetActorLocation();
	// Offset to the right of pawn
	LineStart += GetActorRightVector() * 100.0f;
	// Set line end in direction of the actor's forward
	FVector ActorDirection_LineEnd = LineStart + (GetActorForwardVector() * 100.0f);
	// Draw Actor's Direction
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ActorDirection_LineEnd, DrawScale, FColor::Yellow, false, 0.0f, 0, Thickness);

	FVector ControllerDirection_LineEnd = LineStart + (GetControlRotation().Vector() * 100.0f);
	// Draw 'Controller' Rotation ('PlayerController' that 'possessed' this character)
	DrawDebugDirectionalArrow(GetWorld(), LineStart, ControllerDirection_LineEnd, DrawScale, FColor::Green, false, 0.0f, 0, Thickness);


}

// Called to bind functionality to input
void ASUCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	PlayerInputComponent->BindAxis("MoveForward", this, &ASUCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASUCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

	// Attacks
	PlayerInputComponent->BindAction("PrimaryAttack", EInputEvent::IE_Pressed, this, &ASUCharacter::PrimaryAttack);
	PlayerInputComponent->BindAction("PrimaryInteract", EInputEvent::IE_Pressed, this, &ASUCharacter::PrimaryInteract);
	PlayerInputComponent->BindAction("Dash", EInputEvent::IE_Pressed, this, &ASUCharacter::DashAbility);
	PlayerInputComponent->BindAction("ThrowProjectile", EInputEvent::IE_Pressed, this, &ASUCharacter::ThrowBlackHole);

	// Sprints
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ASUCharacter::SprintStart);
	PlayerInputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ASUCharacter::SprintStop);
}

void ASUCharacter::MoveForward(float value)
{
	FRotator controlRot = GetControlRotation();
	controlRot.Roll = 0.f;
	controlRot.Pitch = 0.f;

	AddMovementInput(controlRot.Vector() , value);
}

void ASUCharacter::MoveRight(float value)
{
	FRotator controlRot = GetControlRotation();
	controlRot.Pitch = 0.0f;
	controlRot.Roll = 0.0f;

	FVector RightVector = FRotationMatrix(controlRot).GetScaledAxis(EAxis::Y);
	AddMovementInput(RightVector, value);
}

void ASUCharacter::SprintStart()
{
	ActionComp->StartActionByName(this, "Sprint");
}

void ASUCharacter::SprintStop()
{
	ActionComp->StopActionByName(this, "Sprint");
}

FVector ASUCharacter::GetPawnViewLocation() const
{
	return CameraComp->GetComponentLocation();
}

void ASUCharacter::PrimaryAttack()
{
	ActionComp->StartActionByName(this, "PrimaryAttack");
	
	// Removed in Converting Actions at 12:40
	/*if (AttackAnim)
	{
		if (!lock)
		{
			lock = true;
			PlayAnimMontage(AttackAnim);


			GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASUCharacter::PrimaryAttack_TimerElapsed, 0.2f);
		}
	}*/
}

// Triggered by a button press
void ASUCharacter::PrimaryInteract()
{
	if (InteractionComp)
	{
		InteractionComp->PrimaryInteract();
		//UE_LOG(LogTemp, Warning, TEXT("Interaction?") );
	}
}

// @REMOVEME NOT NEEDED ANYMORE
// Actually spawns the projectile that is used
void ASUCharacter::PrimaryAttack_TimerElapsed()
{
	FVector HandLocation = GetMesh()->GetSocketLocation("Muzzle_01");
	FTransform SpawnTM = FTransform(GetControlRotation(), HandLocation);


	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Instigator = this;


	

	// Delays the lock boolean by one second by using the primaryattack handler and the primaryattak_ended function
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASUCharacter::PrimaryAttack_Ended, 1.0f);

	FVector trace_end = CameraComp->GetComponentLocation()  + (CameraComp->GetComponentRotation().Vector() * 1000.0f);
	FVector trace_endc = CameraComp->GetComponentLocation() + (CameraComp->GetComponentRotation().Vector() * 1000.0f);

	FHitResult hit;
	FCollisionShape shape;
	shape.SetSphere(20.0f);

	FCollisionObjectQueryParams objectparams;
	objectparams.RemoveObjectTypesToQuery(ECollisionChannel::ECC_Camera);
	objectparams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldDynamic);
	objectparams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
	//ECollisionChannel::ECC_Camera
	
	objectparams.RemoveObjectTypesToQuery(ECollisionChannel::ECC_Pawn);

	bool impacted = GetWorld()->SweepSingleByObjectType(hit, HandLocation, trace_end, FQuat::Identity, objectparams , shape);

	if (impacted)
	{
		//DrawDebugLine(GetWorld(), CameraComp->GetComponentLocation(), hit.ImpactPoint, FColor::Red, false, 2.0f);

		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Some variable values: x: %f, y: %f"), x, y));

		// How to print to to the console
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, hit.GetActor()->GetName() );

		DrawDebugSphere(GetWorld(), hit.Location, shape.GetSphereRadius(), 15, FColor::Red, false, 2.0f);

		FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, hit.Location);

		SpawnTM = FTransform(newRotation, HandLocation);
		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams); 
	}

	else
	{
		DrawDebugLine(GetWorld(), CameraComp->GetComponentLocation(), trace_endc, FColor::Green, false, 2.0f);

		FRotator newRotation = UKismetMathLibrary::FindLookAtRotation(HandLocation, trace_endc);
		SpawnTM = FTransform(newRotation, HandLocation);

		GetWorld()->SpawnActor<AActor>(ProjectileClass, SpawnTM, SpawnParams);
	}
}

void ASUCharacter::PrimaryAttack_Ended()
{
	lock = false;
	;
}

void ASUCharacter::DashAbility()
{
	ActionComp->StartActionByName(this, "Dash");

	// Original dash implementation
	/*if (!dashlock)
	{
		if (DashAnim)
		{
			dashlock = true;
			GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASUCharacter::DashAbilityTimer, .2);


			PlayAnimMontage(DashAnim);

			FTransform SpawnTM = FTransform(GetControlRotation(), GetMesh()->GetSocketLocation("Muzzle_01"));
			FActorSpawnParameters SpawnParams;

			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SpawnParams.Instigator = this;

			
			//projectileRef = 
			GetWorld()->SpawnActor<ASDashProjectile>(DashingProjectileClass, SpawnTM, SpawnParams)->SetOwner(this);
			//projectileRef->SetOwner(this);
		}
	}*/
}

// @REMOVEME NOT NEEDED ANYMORE
void ASUCharacter::DashAbilityTimer()
{
	//SetActorLocation(projectileRef->GetActorLocation());
	
	GetWorldTimerManager().SetTimer(TimerHandle_PrimaryAttack, this, &ASUCharacter::OnGround, 4);
}

void ASUCharacter::ThrowBlackHole()
{
	ActionComp->StartActionByName(this, "Blackhole");


	// Original Black hole implementation
	/*if (BlackHoleAnim)
	{
		PlayAnimMontage(BlackHoleAnim);

		FTransform SpawnTM = FTransform(GetControlRotation(), GetMesh()->GetSocketLocation("Muzzle_01"));
		FActorSpawnParameters SpawnParams;

		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.Instigator = this;


		GetWorld()->SpawnActor<AMySBlackHoleProjectile>(BlackHoleProjectileClass, SpawnTM, SpawnParams);
	}*/
}

void ASUCharacter::OnGround()
{
	dashlock = false;
}