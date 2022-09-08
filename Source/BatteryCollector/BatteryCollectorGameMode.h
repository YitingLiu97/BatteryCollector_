// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once
#include "HAL/Platform.h"
#include "UObject/Class.h"
#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BatteryCollectorGameMode.generated.h"


UENUM(BlueprintType)
enum class EGameState : uint8
{
	Playing,
	Won,
	Lost,
	None
};


UCLASS(minimalapi)
class ABatteryCollectorGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABatteryCollectorGameMode();

	UFUNCTION(BlueprintPure, Category="Power")
	float GetPowerAmountToWin() const;

	UFUNCTION(BlueprintPure, Category = "Power")
	EGameState GetCurrentGameState() const; 
	void SetCurrentGameState(EGameState NewState);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	float DelayTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	float DecayAmount;	


	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	float PowerAmountToWin;	
		
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	float PowerToWinMultiplier;	

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Power")
	TSubclassOf<class UUserWidget> MainHUDClass;

	// instance of the class 
	UPROPERTY()// Makes it handled by Unreal GC
	class UUserWidget* ActiveWidget;
		
	virtual void BeginPlay() override;

	void StartPowerLevelDecay();

	FTimerHandle PowerDelayTimerHandle;


private:
	EGameState CurrentGameState; 
	
	TArray<class ABatterySpawnerBase*> ActiveSpawnersInTheLevel; 

	void HandleNewState(EGameState NewState); 

};



