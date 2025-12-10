// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEII_FightingGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GEII_FightingGameGameMode.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGEII_FightingGameCharacter

AGEII_FightingGameCharacter::AGEII_FightingGameCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;


	// Create Right Hand Hitbox Sphere
	RightHandHitbox = CreateDefaultSubobject<USphereComponent>(TEXT("RightHandHitbox"));
	RightHandHitbox->SetupAttachment(GetMesh(), FName("hand_rSocket"));

	// Create Left Hand Hitbox Sphere
	LeftHandHitBox = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHandHitBox"));
	LeftHandHitBox->SetupAttachment(GetMesh(), FName("hand_lSocket"));

	// Create HurtBox
	HurtBox = CreateDefaultSubobject<UBoxComponent>(TEXT("HurtBox"));
	HurtBox->SetupAttachment(RootComponent);
	HurtBox->InitBoxExtent(FVector(60.f, 60.f, 100.f));

	//Health
	MaxHealth = 100.f;
	Health = MaxHealth;

	AttackCount = 0;

	Damage = 10.f;

	RightHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGEII_FightingGameCharacter::BeginPlay()
{

	Super::BeginPlay();

	RightHandHitbox->OnComponentBeginOverlap.AddDynamic(this, &AGEII_FightingGameCharacter::OnRightHandBeginOverlap);
	LeftHandHitBox->OnComponentBeginOverlap.AddDynamic(this, &AGEII_FightingGameCharacter::OnLeftHandBeginOverlap);



}

void AGEII_FightingGameCharacter::SetRival(AGEII_FightingGameCharacter* InRival)
{
	Rival = InRival;
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGEII_FightingGameCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	if (!GameMode)
	{
		GameMode = Cast<AGEII_FightingGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		const int32 ControllerIndex = PlayerController->GetLocalPlayer()->GetControllerId();

		if (ControllerIndex == 0)
		{
			if (GameMode)
			{
				GameMode->AddPlayer1ToGameMode(this);
			}
		}
		else if (ControllerIndex == 1)
		{
			if (GameMode)
			{
				GameMode->AddPlayer2ToGameMode(this);
				GetMesh()->SetSkeletalMeshAsset(Player2Mesh);
			}
		}
	}

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGEII_FightingGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGEII_FightingGameCharacter::Move);

		EnhancedInputComponent->BindAction(ComboAttackAction1, ETriggerEvent::Triggered, this, &AGEII_FightingGameCharacter::FastCombo);

		EnhancedInputComponent->BindAction(ComboAttackAction2, ETriggerEvent::Triggered, this, &AGEII_FightingGameCharacter::ComboAttack2);

		EnhancedInputComponent->BindAction(ComboAttackAction3, ETriggerEvent::Triggered, this, &AGEII_FightingGameCharacter::ComboAttack3);

		EnhancedInputComponent->BindAction(ComboAttackAction4, ETriggerEvent::Triggered, this, &AGEII_FightingGameCharacter::ComboAttack4);


	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AGEII_FightingGameCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		AController* CurrentController = GetController();
		APlayerController* PlayerController1 = UGameplayStatics::GetPlayerController(GetWorld(), 0);

		if (CurrentController == PlayerController1) {
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.X);
		}
		else {
			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X)*(-1);
			// add movement 
			AddMovementInput(ForwardDirection, MovementVector.X);
		}
		

	}
}

void AGEII_FightingGameCharacter::FastCombo(const FInputActionValue& Value)
{
	if (!isAttacking) {
		isAttacking = true;
		OnSaveFastCombo = true;
		AttackCount = 0;
		PlayAnimMontage(FastComboAnim[AttackCount]);
		Damage = 2;
		
	}
	else if(!SaveFastCombo){
		SaveFastCombo = true;
	}
}

void AGEII_FightingGameCharacter::ComboAttack2(const FInputActionValue& Value)
{
	if (!isAttacking) {
		isAttacking = true;
		OnSaveCombo2 = true;
		AttackCount = 0;
		PlayAnimMontage(AttackCombo2[AttackCount]);
		Damage = 5;

	}
	else if (!SaveCombo2) {
		SaveCombo2 = true;
	}
}

void AGEII_FightingGameCharacter::ComboAttack3(const FInputActionValue& Value)
{
	if (!isAttacking) {
		isAttacking = true;
		OnSaveCombo3 = true;
		AttackCount = 0;
		PlayAnimMontage(AttackCombo3[AttackCount]);
		Damage = 10;

	}
	else if (!SaveCombo3) {
		SaveCombo3 = true;
	}
}

void AGEII_FightingGameCharacter::ComboAttack4(const FInputActionValue& Value)
{
	if (!isAttacking) {
		isAttacking = true;
		OnSaveCombo4 = true;
		AttackCount = 0;
		PlayAnimMontage(AttackCombo4[AttackCount]);
		Damage = 10;

	}
	else if (!SaveCombo4) {
		SaveCombo4 = true;
	}
}

void AGEII_FightingGameCharacter::ComboAttackSave()
{
	RightHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (SaveFastCombo && OnSaveFastCombo && AttackCount + 1 < FastComboAnim.Num()) {
		SaveFastCombo = false;
		AttackCount += 1;
		PlayAnimMontage(FastComboAnim[AttackCount]);
	}
	else if (SaveCombo2 && OnSaveCombo2 && AttackCount + 1 < AttackCombo2.Num()) {
		SaveCombo2 = false;
		AttackCount += 1;
		PlayAnimMontage(AttackCombo2[AttackCount]);
		if (AttackCount == 3) {
			Damage = 13;
		}
	}
	else if (SaveCombo3 && OnSaveCombo3 && AttackCount + 1 < AttackCombo3.Num()) {
		SaveCombo3 = false;
		AttackCount += 1;
		PlayAnimMontage(AttackCombo3[AttackCount]);
	}
	else if (SaveCombo4 && OnSaveCombo4 && AttackCount + 1 < AttackCombo4.Num()) {
		SaveCombo4 = false;
		AttackCount += 1;
		PlayAnimMontage(AttackCombo4[AttackCount]);
	}
}

void AGEII_FightingGameCharacter::ResetCombo()
{
	AttackCount = 0;
	SaveFastCombo = false;
	OnSaveFastCombo = false;
	SaveCombo2 = false;
	OnSaveCombo2 = false;
	SaveCombo3 = false;
	OnSaveCombo3 = false;
	SaveCombo4 = false;
	OnSaveCombo4 = false;
	isAttacking = false;

}

void AGEII_FightingGameCharacter::EnableRightHandCollision()
{
	RightHandHitbox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGEII_FightingGameCharacter::EnableLeftHandCollision()
{
	LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AGEII_FightingGameCharacter::OnLeftHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
															int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this) {
		if (isAttacking) {
			LeftHandHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (Particles.Num() > 0) {
				int randomParticle = FMath::RandRange(0, Particles.Num() - 1);
				UGameplayStatics::SpawnEmitterAttached(Particles[randomParticle], LeftHandHitBox);
			}
			if (Rival) {
				Rival->DamageTaken(Damage);
			}
			

		}
	}
}

void AGEII_FightingGameCharacter::OnRightHandBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
															int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor != this) {
		if (isAttacking) {
			RightHandHitbox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			if (Particles.Num() > 0) {
				int randomParticle = FMath::RandRange(0, Particles.Num() - 1);
				UGameplayStatics::SpawnEmitterAttached(Particles[randomParticle], RightHandHitbox);
			}
			if (Rival) {
				Rival->DamageTaken(Damage);
			}
			
		}
	}
}

void AGEII_FightingGameCharacter::DamageTaken(float IncomingDamage)
{
	Health = Health - IncomingDamage;
	if (!(OnSaveFastCombo || OnSaveCombo2 || OnSaveCombo3 || OnSaveCombo4)) {
		PlayAnimMontage(GettingHittedAnim);
	}
	

	if (Health <= 0) {
		GameMode->SetCurrentState(EFightingGamePlayState::EPlayerWin);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		HurtBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}



