// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SUCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponents;
class UAnimMontage;
class AMySBlackHoleProjectile;
class ASDashProjectile;
class USAttributeComponent;
class USActionComponent;


UCLASS()
class ACTIONRPG_API ASUCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* BlackHoleAnim;
	
	UPROPERTY(EditAnywhere, Category = "Abilities")
	UAnimMontage* DashAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AMySBlackHoleProjectile> BlackHoleProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Abilities")
	TSubclassOf<ASDashProjectile> DashingProjectileClass;

	FTimerHandle TimerHandle_PrimaryAttack;
	ASDashProjectile* projectileRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USActionComponent* ActionComp;

	bool lock;
	bool dashlock;
	
public:
	// Sets default values for this character's properties
	ASUCharacter();

	UFUNCTION(Exec)
	void HealSelf(float Amount = 100);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArmComp;

	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponents* InteractionComp;

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void MoveForward(float value);
	void MoveRight(float value);

	void SprintStart();
	void SprintStop();

	virtual FVector GetPawnViewLocation() const override;

	void PrimaryAttack();
	void PrimaryAttack_TimerElapsed();
	void PrimaryAttack_Ended();
	void PrimaryInteract();

	void DashAbility();
	void DashAbilityTimer();
	void ThrowBlackHole();

	void OnGround();
};
