// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GEII_FightingGameGameMode.h"
#include "Camera.h"

// Sets default values
ACamera::ACamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create and attach the camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);

	

}

// Called when the game starts or when spawned
void ACamera::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AGEII_FightingGameGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetViewTarget(this);
}

// Called every frame
void ACamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (GameMode && GameMode->Player1 && GameMode->Player2)
	{
		float Distance = GameMode->Player2->GetDistanceTo(GameMode->Player1) / 2.f;

		FVector ActorLocation = GameMode->Player1->GetActorLocation();

		FVector CurrentLocation = CameraComponent->GetComponentLocation();

		FVector TargetLocation(CurrentLocation.X, ActorLocation.Y + Distance, CurrentLocation.Z);

		// Set up latent info for smooth movement
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = NAME_None; // Optional callback
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = 1;

		UKismetSystemLibrary::MoveComponentTo(CameraComponent, TargetLocation, FRotator::ZeroRotator, false, false, 0.2f, false, EMoveComponentAction::Move, LatentInfo);
	}
	
}

