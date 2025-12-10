// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "GEII_FightingGameCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class AGEII_FightingGameGameMode;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AGEII_FightingGameCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Right Hand Hitbox Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collisions, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* RightHandHitbox;

	/** Left Hand Hitbox Sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collisions, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* LeftHandHitBox;

	/** Hurt Box from the player */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collisions, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* HurtBox;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;


	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Attack Input Action*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ComboAttackAction1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ComboAttackAction2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ComboAttackAction3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ComboAttackAction4;





public:
	AGEII_FightingGameCharacter();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "GameMode")
	void SetRival(AGEII_FightingGameCharacter* InRival);


protected:

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for attack input */
	void FastCombo(const FInputActionValue& Value);

	void ComboAttack2(const FInputActionValue& Value);

	void ComboAttack3(const FInputActionValue& Value);

	void ComboAttack4(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Combos")
	void ComboAttackSave();

	UFUNCTION(BlueprintCallable, Category = "Combos")
	void ResetCombo();

	UFUNCTION(BlueprintCallable, Category = "Combos")
	void EnableRightHandCollision();

	UFUNCTION(BlueprintCallable, Category = "Combos")
	void EnableLeftHandCollision();

	UFUNCTION(BlueprintCallable, Category = "HitBox")
	void OnLeftHandBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintCallable, Category = "HitBox")
	void OnRightHandBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void DamageTaken(float Damage);


protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	/** Returns RightHandHitbox subobject */
	FORCEINLINE class USphereComponent* GetRightHandHitbox() const { return RightHandHitbox; }

	/** Returns RightHandHitbox subobject */
	FORCEINLINE class USphereComponent* GetLeftHandHitbox() const { return LeftHandHitBox; }

	/** Returns RightHandHitbox subobject */
	FORCEINLINE class UBoxComponent* GetHurtBox() const { return HurtBox; }


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool SaveFastCombo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool SaveCombo2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool SaveCombo3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool SaveCombo4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool OnSaveFastCombo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool OnSaveCombo2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool OnSaveCombo3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attacks", meta = (AllowPrivateAccess = "true"))
	bool OnSaveCombo4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks", meta = (BlueprintProtected = true))
	bool isAttacking;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks", meta = (BlueprintProtected = true))
	int AttackCount;

	UPROPERTY(BlueprintReadOnly, Category = "GameMode", meta = (BlueprintProtected = true))
	AGEII_FightingGameGameMode* GameMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameMode", meta = (BlueprintProtected = true))
	AGEII_FightingGameCharacter* Rival;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health", meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Attacks", meta = (BlueprintProtected = true))
	float Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Mesh", meta = (BlueprintProtected = true))
	USkeletalMesh* Player2Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combos", meta = (BlueprintProtected = true))
	TArray<UAnimMontage*> FastComboAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combos", meta = (BlueprintProtected = true))
	TArray<UAnimMontage*> AttackCombo2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combos", meta = (BlueprintProtected = true))
	TArray<UAnimMontage*> AttackCombo3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combos", meta = (BlueprintProtected = true))
	TArray<UAnimMontage*> AttackCombo4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combos", meta = (BlueprintProtected = true))
	UAnimMontage* GettingHittedAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Combos", meta = (BlueprintProtected = true))
	TArray<UParticleSystem*> Particles;

};
