// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickUpBase.generated.h"

UCLASS()
class BATTERYCOLLECTOR_API APickUpBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickUpBase();

	// Gets the value of weather or not this pickup is active 
	UFUNCTION(BlueprintPure, Category = "Pickup")
		bool IsPickupActive() const;


	UFUNCTION(BlueprintCallable, Category = "Pickup")
		void SetPickupIsActive(bool NewActiveState);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool bIsActive;

#pragma region COMPONENTS
	//declare 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* PickupMesh;

#pragma endregion

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintNativeEvent, Category ="Pickup")// used to be BlueprintNativeEvent but failed to build
	void OnPickupCollected();
	virtual void OnPickupCollected_Implementation();


};

