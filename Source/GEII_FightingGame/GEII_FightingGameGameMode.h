// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GEII_FightingGameCharacter.h"
#include "TimerManager.h"
#include "GEII_FightingGameGameMode.generated.h"

UENUM(BlueprintType)
enum class EFightingGamePlayState : uint8
{
	ECountdown,
	EPlaying,
	EPlayerWin,
	EUnknown
};


UCLASS(minimalapi)
class AGEII_FightingGameGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGEII_FightingGameGameMode();

	virtual void BeginPlay() override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	//Enables both player Inputs
	UFUNCTION(BlueprintCallable, Category = "Players")
	void EnableInputs();

	//Adds the first player to the game mode
	UFUNCTION(BlueprintCallable, Category = "Players")
	void AddPlayer1ToGameMode(AGEII_FightingGameCharacter* PlayerOne);

	//Adds the second player to the game mode
	UFUNCTION(BlueprintCallable, Category = "Players")
	void AddPlayer2ToGameMode(AGEII_FightingGameCharacter* PlayerTwo);

	UFUNCTION(BlueprintPure, Category = "Players")
	EFightingGamePlayState GetCurrentState() const;

	void SetCurrentState(EFightingGamePlayState NewState);

private:
	void HandleNewState(EFightingGamePlayState NewState);

public:
	// Store reference to Player 1
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Players", meta = (BlueprintProtected = true))
	AGEII_FightingGameCharacter* Player1;

	// Store reference to Player 2
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Players", meta = (BlueprintProtected = true))
	AGEII_FightingGameCharacter* Player2;

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Players", meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	TArray<USoundBase*> IntroSoundFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Sound")
	TArray<USoundBase*> DeathSoundFX;

	UPROPERTY()
	class UUserWidget* CurrentWidget;

	FTimerHandle InputEnableTimer;


private:
	EFightingGamePlayState CurrentState;
};



