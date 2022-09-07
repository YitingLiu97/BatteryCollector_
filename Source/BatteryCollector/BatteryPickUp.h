// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PickUpBase.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BatteryPickUp.generated.h"

UCLASS()
class BATTERYCOLLECTOR_API ABatteryPickUp : public APickUpBase
{GENERATED_BODY()
public:	
	// Sets default values for this actor's properties
	ABatteryPickUp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power", Meta = (BlueprintProtected = "true"))
		float ChargeAmount;

public:
	// Called every frame

	virtual void Tick(float DeltaTime) override;
	// when i have override, it gives me an error - but it didnt on the tutorial https://youtu.be/YjC8-5KhJFs?t=480
	// had to add a virtual void function after the void function in PickUpBase.h

	void OnPickupCollected_Implementation() override;

	float GetBatteryChargeAmount();
};
