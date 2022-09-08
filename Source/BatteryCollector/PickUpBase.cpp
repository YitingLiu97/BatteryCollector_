// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUpBase.h"

// Sets default values
APickUpBase::APickUpBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsActive = true;
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Pickup Mesh"));
	RootComponent = PickupMesh;

}

bool APickUpBase::IsPickupActive() const
{
	return bIsActive;
}

void APickUpBase::SetPickupIsActive(bool NewActiveState)
{
	bIsActive = NewActiveState;
}

// Called when the game starts or when spawned
void APickUpBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APickUpBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



void APickUpBase::OnPickupCollected_Implementation()
{

	const FString DebugString = GetName();

	// macro doesnt know it is a string, memory does
	// so * to dereference from the memory 
	UE_LOG(LogTemp, Warning, TEXT("Picked Up: %s"), *DebugString);
}



