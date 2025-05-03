// Fill out your copyright notice in the Description page of Project Settings.


#include "VRPawn.h"
#include "Engine/Engine.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RendererInterface.h"
#include "DrawDebugHelpers.h"
#include "../level/MasterMakerSaveGame.h"
#include "Json.h"
#include "../widget/GamePlayWidget.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UserPlayerState.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AVRPawn::AVRPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	scene = CreateDefaultSubobject<USceneComponent>(TEXT("Secene"));

	camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	camera->SetupAttachment(scene);

	movementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

	leftHandConroller = CreateDefaultSubobject<UMotionControllerComponent>("LeftHandController");
	leftHandConroller->SetupAttachment(scene);
	leftHandConroller->SetTrackingSource(EControllerHand::Left);

	rightHandController = CreateDefaultSubobject<UMotionControllerComponent>("RightHandController");
	rightHandController-> SetupAttachment(scene);
	rightHandController->SetTrackingSource(EControllerHand::Right);

	leftHand = CreateDefaultSubobject<UStaticMeshComponent>("LeftHand");
	rightHand = CreateDefaultSubobject<UStaticMeshComponent>("RightHand");
	rightHand->SetupAttachment(rightHandController);


	Ghost_Item = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Ghost"));
	Ghost_Item->SetSimulatePhysics(false);
	Ghost_Item->SetCollisionProfileName("OverlapAll");
	Ghost = Ghost_Item;

	mode = Mode::BuildMode;



}

void AVRPawn::openBuildMode()
{

	switch (mode)
	{
	case Mode::NormalMode:

		Builder_Mode_Enable = false;
		if (Target_Usable_Weapon)
			Target_Usable_Weapon->Owner_Character = nullptr;

		buildEnd();
		mode = Mode::BuildMode;
		break;

	case Mode::BuildMode:

		Builder_Mode_Enable = true;
		if (Target_Usable_Weapon)
			Target_Usable_Weapon->Owner_Character = nullptr;

		Target_Usable_Weapon = nullptr;

		buildStart();
		mode = Mode::NormalMode;
		break;

	default:
		mode = Mode::NormalMode;

		Builder_Mode_Enable = false;

		if (Target_Usable_Weapon)
			Target_Usable_Weapon->Owner_Character = nullptr;
		Target_Usable_Weapon = nullptr;

		buildEnd();

		break;
	}
}

// Called when the game starts or when spawned
void AVRPawn::BeginPlay()
{
	Super::BeginPlay();

	Game_Instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());
	userCharacter = GetWorld()->SpawnActor<AUserCharacter>(User_Character_Container, GetActorLocation(), FRotator(0, 0, 0));

	
}

void AVRPawn::setupMesh()
{
}

// Called every frame
void AVRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Builder_Mode_Enable == true)
	{
		FHitResult OutHit;
		FVector Start = rightHand->GetComponentLocation();
		FVector forward_vector = rightHand->GetComponentRotation().Vector();
		FVector End = Start + (forward_vector * 3000.0f);
		FCollisionQueryParams CollisionParams;

		GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, CollisionParams);
		Aim_Impact_Point = OutHit.ImpactPoint;
		DrawDebugLine(
			GetWorld(),
			Start,
			End,
			FColor::Green,
			false,      // Set to true to make it "permanent" (for the duration of game session)
			0.1f,       // Duration in seconds
			0,
			2.0f        // Thickness
		);
		lookForBuildingSpace(OutHit.GetActor(), OutHit);
	}

}

// Called to bind functionality to input
void AVRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	Player_State = Cast<AUserPlayerState>(GetPlayerState());

	PlayerInputComponent->BindAxis(TEXT("MoveForward"), this, &AVRPawn::moveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"), this, &AVRPawn::moveRight);
	PlayerInputComponent->BindAxis(TEXT("MoveCharacterForward"), this, &AVRPawn::moveCharacterForward);
	PlayerInputComponent->BindAxis(TEXT("MoveCharacterRight"), this, &AVRPawn::moveCharacterRight);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AVRPawn::fire);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AVRPawn::openBuildMode);

}


void AVRPawn::moveForward(float AxisValue)
{
	if (Controller && AxisValue != 0.0f)
	{
		const FRotator YawRotation(0, camera->GetComponentRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, AxisValue);
	}
}

void AVRPawn::moveRight(float AxisValue)
{
	if (Controller && AxisValue != 0.0f)
	{
		const FRotator YawRotation(0, camera->GetComponentRotation().Yaw, 0);
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, AxisValue);
	}
}

void AVRPawn::moveCharacterForward(float AxisValue)
{
	const FRotator yawRotation(0, camera->GetComponentRotation().Yaw, 0);
	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::X);
	userCharacter->moveRemote(AxisValue, direction);
}

void AVRPawn::moveCharacterRight(float AxisValue)
{
	const FRotator yawRotation(0, camera->GetComponentRotation().Yaw, 0);
	const FVector direction = FRotationMatrix(yawRotation).GetUnitAxis(EAxis::Y);
	userCharacter->moveRemote(AxisValue, direction);
}

void AVRPawn::fire()
{
	FVector forwardVector = rightHand->GetForwardVector();
	FVector location = rightHand->GetComponentLocation() + (rightHand->GetForwardVector() * 10);
	ABullet* bullet_ptr = GetWorld()->SpawnActor<ABullet>(Bullet_Container, location, FRotator(0, 0, 0));

	if (bullet_ptr != nullptr) {
		bullet_ptr->Strenght_ = 40;
		bullet_ptr->setMassAndStrenght(3, 100);
		bullet_ptr->addFireImpulse(forwardVector, 30000);
	}
}
