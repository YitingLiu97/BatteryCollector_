// Copyright Epic Games, Inc. All Rights Reserved.


#include "BatteryCollectorCharacter.h"
#include "PickUpBase.h"
#include "BatteryPickUp.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"

//////////////////////////////////////////////////////////////////////////
// ABatteryCollectorCharacter

ABatteryCollectorCharacter::ABatteryCollectorCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
	
	
	
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Collider"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(200.0f);
	BasePowerLevel = 2500.0f;
	CurrentPowerLevel = BasePowerLevel;

	BaseSpeed = 10.0f;
	SpeedMultiplier = 0.65f;



}

void ABatteryCollectorCharacter::BeginPlay()
{
	Super::BeginPlay();

}


//////////////////////////////////////////////////////////////////////////
// Input

void ABatteryCollectorCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	UE_LOG(LogTemp, Warning, TEXT("in SetupPlayerInputComponent"));

	PlayerInputComponent->BindAction("Collect", IE_Pressed, this, &ABatteryCollectorCharacter::CollectPickups);
	UE_LOG(LogTemp, Warning, TEXT("after Collect"));


	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &ABatteryCollectorCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &ABatteryCollectorCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &ABatteryCollectorCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &ABatteryCollectorCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &ABatteryCollectorCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ABatteryCollectorCharacter::TouchStopped);
}

void ABatteryCollectorCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void ABatteryCollectorCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void ABatteryCollectorCharacter::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// get the first material 
	const TArray<UMaterialInterface*> Materials = GetMesh()->GetMaterials();

	PowerLevelDynamicMaterial.Reserve(Materials.Num());
	for (int32 Index = 0; Index < Materials.Num(); ++Index)
	{
		UMaterialInterface* MaterialToEdit = GetMesh()->GetMaterial(Index);
		PowerLevelDynamicMaterial.Add(UMaterialInstanceDynamic::Create(MaterialToEdit, this));
		//GetMesh()->SetMaterial(Index, PowerLevelDynamicMaterial[Index]);
	}

}

void ABatteryCollectorCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void ABatteryCollectorCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ABatteryCollectorCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}



void ABatteryCollectorCharacter::UpdatePlayerMaterialColor()
{
	FLinearColor FinalColor; 
	// get material set a vector parameter 
	
	// get our initial and current power level create a ratio (alpha)
	const float Alpha = GetCurrentPowerLevel() / GetBasePowerLevel();
	//use the alpha to lerp inbetween 2 color varaibles 
	FinalColor = FLinearColor::LerpUsingHSV(FLinearColor::White, FLinearColor::Red, Alpha);
	for (int32 Index = 0; Index < PowerLevelDynamicMaterial.Num(); ++Index)
	{
		PowerLevelDynamicMaterial[Index]->SetVectorParameterValue("Tint", FinalColor);
	}	
	

}

float ABatteryCollectorCharacter::GetBasePowerLevel()
{
	return BasePowerLevel;
}

float ABatteryCollectorCharacter::GetCurrentPowerLevel()
{
	return CurrentPowerLevel;
}

void ABatteryCollectorCharacter::UpdateCurrentPowerLevel(float Amount)
{
	CurrentPowerLevel += Amount;

	GetCharacterMovement()->MaxWalkSpeed= BaseSpeed * (SpeedMultiplier*CurrentPowerLevel);

	// Create Visual Representation 
	UpdatePlayerMaterialColor();
}



void ABatteryCollectorCharacter::CollectPickups()
{
	UE_LOG(LogTemp, Warning, TEXT("in CollectPickups"));

	// Create an array to put our overlapped actors in 
	TArray<AActor*> OverlappedActors;

	float CachedPowerLevel = 0.0f;

	// Fill that array by getting the overlapped actors in our collection sphere 
	CollisionSphere->GetOverlappingActors(OverlappedActors);
	// Iterate over the array and cost the actor we find a pickup. 
	UE_LOG(LogTemp, Warning, TEXT("OverlappedActors has %d: "), OverlappedActors.Num());

	for (int index = 0; index < OverlappedActors.Num(); ++index)
	{
		APickUpBase* OverlappedActor = Cast<APickUpBase>(OverlappedActors[index]);

		// If that actor found is valid and is active and is not about to be destsroyed, call OnCollected() 

		if (OverlappedActor && OverlappedActor->IsPickupActive() && IsValid(OverlappedActor))
		{
			UE_LOG(LogTemp, Warning, TEXT("in CollectPickupsed"));

			OverlappedActor->OnPickupCollected();

			ABatteryPickUp* BatteryPickUp = Cast<ABatteryPickUp>(OverlappedActor);

			if (BatteryPickUp) {
				CachedPowerLevel += BatteryPickUp->GetBatteryChargeAmount();
				UE_LOG(LogTemp, Warning, TEXT("BatteryPickUp charge amount is %d: "), CachedPowerLevel);

			}

			// and SetPickUpIsActive()

			OverlappedActor->SetPickupIsActive(false);

		}

	}


	if (CachedPowerLevel > 0.0f) {
		UpdateCurrentPowerLevel(CachedPowerLevel);
	}

}

// Called every frame
void ABatteryCollectorCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
