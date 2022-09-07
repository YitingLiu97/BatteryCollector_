// Copyright Epic Games, Inc. All Rights Reserved.

#include "BatteryCollectorGameMode.h"
#include "BatteryCollectorCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"

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
}


void ABatteryCollectorGameMode::BeginPlay() {

	Super::BeginPlay();

	FTimerHandle PowerDecayTimerHandle;

	GetWorld()->GetTimerManager().SetTimer(PowerDelayTimerHandle, this, &ABatteryCollectorGameMode::StartPowerLevelDecay, DelayTime, true);

}

// should make sure the third person is moving - Game Mode related
void ABatteryCollectorGameMode::StartPowerLevelDecay()
{
	// Get a reference to our player class 
	ABatteryCollectorCharacter* PlayerCharacter = Cast<ABatteryCollectorCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));

	// Check if our power level is greater than zero 
	if (PlayerCharacter && PlayerCharacter->GetCurrentPowerLevel() > 0.0f) {

		PlayerCharacter->UpdateCurrentPowerLevel(-DecayAmount);

	}
	// Call the function that updates our power level 




}
