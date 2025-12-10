// Copyright Epic Games, Inc. All Rights Reserved.

#include "GEII_FightingGameGameMode.h"
#include "GEII_FightingGameCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AGEII_FightingGameGameMode::AGEII_FightingGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AGEII_FightingGameGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Create Player
	FString OutError;
	GetWorld()->GetGameInstance()->CreateLocalPlayer(1, OutError, true);

	//Set Timer to call enable Inputs after the animations
	GetWorldTimerManager().SetTimer(InputEnableTimer, this, &AGEII_FightingGameGameMode::EnableInputs, 6.f, false);

	if (HUDWidgetClass != nullptr) {
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentWidget != nullptr) {
			CurrentWidget->AddToViewport();
		}
	}

	SetCurrentState(EFightingGamePlayState::ECountdown);
}

AActor* AGEII_FightingGameGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (Player == PlayerController) {
		return FindPlayerStart(Player, "Player1");
	}
	else {
		return FindPlayerStart(Player, "Player2");
	}

}

void AGEII_FightingGameGameMode::EnableInputs()
{
	SetCurrentState(EFightingGamePlayState::EPlaying);

	/*APlayerController* PlayerController1 = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APlayerController* PlayerController2 = UGameplayStatics::GetPlayerController(GetWorld(), 1);

	if (Player1 && PlayerController1) {
		Player1->EnableInput(PlayerController1);
	}

	if (Player2 && PlayerController2) {
		Player2->EnableInput(PlayerController2);
	}*/
}

void AGEII_FightingGameGameMode::AddPlayer1ToGameMode(AGEII_FightingGameCharacter* PlayerOne)
{
	Player1 = PlayerOne;

	APlayerController* PlayerController1 = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (PlayerController1 && Player1) {
		Player1->DisableInput(PlayerController1);
	}
}

void AGEII_FightingGameGameMode::AddPlayer2ToGameMode(AGEII_FightingGameCharacter* PlayerTwo)
{
	Player2 = PlayerTwo;

	APlayerController* PlayerController2 = UGameplayStatics::GetPlayerController(GetWorld(), 1);

	if (PlayerController2 && Player2) {
		Player2->DisableInput(PlayerController2);
	}
}

EFightingGamePlayState AGEII_FightingGameGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AGEII_FightingGameGameMode::SetCurrentState(EFightingGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(CurrentState);
}

void AGEII_FightingGameGameMode::HandleNewState(EFightingGamePlayState NewState)
{
	switch (NewState) {

	case EFightingGamePlayState::ECountdown:
	{
		if (IntroSoundFX.Num() > 0) {
			int randomIntroSound = FMath::RandRange(0, IntroSoundFX.Num() - 1);
			UGameplayStatics::PlaySound2D(this, IntroSoundFX[randomIntroSound]);
		}
	}
	break;
	case EFightingGamePlayState::EPlaying:
	{
		APlayerController* PlayerController1 = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		APlayerController* PlayerController2 = UGameplayStatics::GetPlayerController(GetWorld(), 1);

		if (Player1 && PlayerController1) {
			Player1->EnableInput(PlayerController1);
		}

		if (Player2 && PlayerController2) {
			Player2->EnableInput(PlayerController2);
		}

		Player1->SetRival(Player2);
		Player2->SetRival(Player1);
	}
	break;
	case EFightingGamePlayState::EPlayerWin: 
	{
		APlayerController* PlayerController1 = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		APlayerController* PlayerController2 = UGameplayStatics::GetPlayerController(GetWorld(), 1);

		if (Player1 && PlayerController1) {
			Player1->DisableInput(PlayerController1);
		}

		if (Player2 && PlayerController2) {
			Player2->DisableInput(PlayerController2);
		}

		if (DeathSoundFX.Num() > 0) {
			int randomDeathSound = FMath::RandRange(0, DeathSoundFX.Num() - 1);
			UGameplayStatics::PlaySound2D(this, DeathSoundFX[randomDeathSound]);
		}
	}
	break;
	break;
	default:
	case EFightingGamePlayState::EUnknown: {

	}
	break;
	}
}
