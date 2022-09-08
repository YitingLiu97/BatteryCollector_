// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
ABatteryCollectorGameMode::ABatteryCollectorGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	DelayTime = 0.2f;
	DecayAmount = 50.0f;

	PowerToWinMultiplier = 1.5f;
}

float ABatteryCollectorGameMode::GetPowerAmountToWin() const
{
	return PowerAmountToWin;
}

EGameState ABatteryCollectorGameMode::GetCurrentGameState() const
{
	return CurrentGameState;
}

void ABatteryCollectorGameMode::SetCurrentGameState(EGameState NewState)
{
	CurrentGameState= NewState;
}


void ABatteryCollectorGameMode::BeginPlay() {

	Super::BeginPlay();

	SetCurrentGameState(EGameState::Playing);

	FTimerHandle PowerDecayTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(PowerDelayTimerHandle, this, &ABatteryCollectorGameMode::StartPowerLevelDecay, DelayTime, true);
	// Get a reference to our player class 
	ABatteryCollectorCharacter* PlayerCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	// Check if our power level is greater than zero 
	if (PlayerCharacter) {

		PowerAmountToWin = PlayerCharacter->GetBasePowerLevel() * PowerToWinMultiplier;
	}

	if (MainHUDClass) {

		ActiveWidget = CreateWidget<UUserWidget>(GetWorld(), MainHUDClass);
		ActiveWidget->AddToViewport();
	}


}

// should make sure the third person is moving - Game Mode related
void ABatteryCollectorGameMode::StartPowerLevelDecay()
{
	// Get a reference to our player class 
	ABatteryCollectorCharacter* PlayerCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	// Check if our power level is greater than zero 
	if (PlayerCharacter ) {

		if (PlayerCharacter->GetCurrentPowerLevel() > PowerAmountToWin) {
			UE_LOG(LogTemp, Warning, TEXT("PowerAmountToWin %d: "), PowerAmountToWin)

			SetCurrentGameState(EGameState::Won);

		}
		else if (PlayerCharacter->GetCurrentPowerLevel() > 0.0f) {

			PlayerCharacter->UpdateCurrentPowerLevel(-DecayAmount);

		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("should lose"));
			SetCurrentGameState(EGameState::Lost);

		
		}

	}





	// Call the function that updates our power level 




}
