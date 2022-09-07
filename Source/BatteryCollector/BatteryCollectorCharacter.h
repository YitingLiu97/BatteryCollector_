// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameFramework/Actor.h"
#include "BatteryCollectorCharacter.generated.h"

UCLASS(config = Game)
class ABatteryCollectorCharacter : public ACharacter
{
	GENERATED_BODY()

		/** Camera boom positioning the camera behind the character */
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;


	// cant use blueprint read only or callable since it is not public - even tho it is in a public class
	UPROPERTY(VisibleAnywhere, Category = Collision, meta = (AllowPrivateAcess = "true"))
		class USphereComponent* CollisionSphere;



public:
	ABatteryCollectorCharacter();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;

protected:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
		void CollectPickups();

	void Tick(float DeltaTime);

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);


protected:
	virtual	void OnConstruction(const FTransform& Transform) override;


#pragma region Power Level 

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float BasePowerLevel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float BaseSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Power")
	float SpeedMultiplier;

	UPROPERTY(VisibleAnywhere, Category = "Power")
	float CurrentPowerLevel;

	UPROPERTY(BlueprintReadWrite, Category ="Power|VFX")
	TArray<TObjectPtr<class UMaterialInstanceDynamic>> PowerLevelDynamicMaterial; 

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerMaterialColor();

public:
	UFUNCTION(BlueprintPure, Category = "Power")
	float GetBasePowerLevel();

	UFUNCTION(BlueprintPure, Category = "Power")
	float GetCurrentPowerLevel();

	UFUNCTION(BlueprintCallable)
	void UpdateCurrentPowerLevel(float Amount);


#pragma endregion 



protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	FORCEINLINE class USphereComponent* GetSphereCollision() const { return CollisionSphere; }

};

