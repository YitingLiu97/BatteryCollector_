// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PickUpBase.h"
#include "BatterySpawnerBase.h"

// Sets default values
ABatterySpawnerBase::ABatterySpawnerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	SpawnVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Spawner"));


}

// Called when the game starts or when spawned
void ABatterySpawnerBase::BeginPlay()
{
	Super::BeginPlay();

	RandomSpawnDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABatterySpawnerBase::SpawnBatteryActor, RandomSpawnDelay, false);

}

// Called every frame
void ABatterySpawnerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// set up random spawn point 
FVector ABatterySpawnerBase::GetRandomSpawnPoint()
{
	const FVector SpawnOrigin = SpawnVolume->Bounds.Origin;
	const FVector SpawnLimit = SpawnVolume->Bounds.BoxExtent;
	return UKismetMathLibrary::RandomPointInBoundingBox(SpawnOrigin, SpawnLimit);
}

void ABatterySpawnerBase::SpawnBatteryActor()
{
	// make sure we have an actor to spawn 
	if (!ActorToSpawn) { return; }

	// create random rotation
	FRotator RandomLocation;
	RandomLocation.Yaw = FMath::RandRange(1, 3) * 360.0f;
	RandomLocation.Pitch = FMath::RandRange(1, 3) * 360.0f;
	RandomLocation.Roll = FMath::RandRange(1, 3) * 360.0f;

	// set up spawn parameters 
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.Instigator = GetInstigator();
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// spawn actor 
	if (!GetWorld()) { return; }

	APickUpBase* ActorSpawned = GetWorld()->SpawnActor<APickUpBase>(ActorToSpawn, GetRandomSpawnPoint(), RandomLocation, Params);


	RandomSpawnDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
	GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &ABatterySpawnerBase::SpawnBatteryActor, RandomSpawnDelay, false);

}

