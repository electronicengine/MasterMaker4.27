// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "../brick/Brick.h"
#include "../brick/weapon/Bullet.h"
#include "Materials/Material.h"
#include "../interface/BuilderInterface.h"
#include "../interface/InteractInterface.h"
#include "../interface/SlingInterface.h"
#include "../interface/BreakableInterface.h"
#include "UserCharacter.h"

#include "VRPawn.generated.h"

enum Mode {
	NormalMode,
	BuildMode,
};


UCLASS()
class MASTERMAKER_API AVRPawn : public APawn, public IInteractInterface
{
	GENERATED_BODY()

private:
	void openBuildMode();
	Mode mode;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Ghost_Item;

	AWeaponBrick* Interactable_Weapon;

	float Last_Touch;

	bool Punching;
	void setupMesh();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool Is_In_Car;

	UPROPERTY(EditDefaultsOnly, Category = "Bullet")
	TSubclassOf<class ABullet> Bullet_Container;

	UPROPERTY(EditDefaultsOnly, Category = "Character")
	TSubclassOf<class AUserCharacter> User_Character_Container;

	AUserCharacter* userCharacter;

public:	

	AVRPawn();
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void moveForward(float AxisValue);
	void moveRight(float AxisValue);
	void moveCharacterForward(float AxisValue);
	void moveCharacterRight(float AxisValue);

	
	USceneComponent* scene;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCameraComponent* camera;

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* movementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMotionControllerComponent* rightHandController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMotionControllerComponent* leftHandConroller;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* leftHand;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* rightHand;

    void fire();


    FConstructionInfo Construction_Info;
    UStaticMeshComponent* Sling_Bed;
    AWeaponBrick* Target_Usable_Weapon;
    UAnimMontage* Punch_Animation;
    FVector Initial_Touch_Location;



};
