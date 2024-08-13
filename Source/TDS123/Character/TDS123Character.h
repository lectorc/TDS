// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TDS123/FuncLibrary/UType.h"
#include "TDS123Character.generated.h"

UCLASS(Blueprintable)
class ATDS123Character : public ACharacter
{
	GENERATED_BODY()

public:
	ATDS123Character();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

    virtual void SetupPlayerInputComponent(class UInputComponent* NewInputComponent    ) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
    
    bool IsSprintForward = false;
    bool SprintEnabled = false;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        EMovementState MovementState = EMovementState::Run_State;
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
        FCharacterSpeed MovementInfo;


    UFUNCTION()
         void InputAxisX(float Value);
    UFUNCTION()
         void InputAxisY(float Value);

    float AxisX = 0.0f;
    float AxisY = 0.0f; 
    // Tick Func
    UFUNCTION()
        void MovementTick(float DeltaTime);

    UFUNCTION(BlueprintCallable)
        void CharacterUpdate();

    UFUNCTION(BlueprintCallable)
        void ChangeMovementState(EMovementState NewMovementState);

    UFUNCTION(BlueprintCallable)
    void ChangeBlockedSprint();
    

    UFUNCTION(BlueprintCallable)
    void BlockMovementVector();

    UFUNCTION(BlueprintCallable)
    void StaminaSystem();

};

