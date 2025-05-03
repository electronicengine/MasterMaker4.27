// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacter.h"
#include "../brick/weapon/Bullet.h"
#include "../brick/Brick.h"
#include "EnemyCharacterAIController.h"
#include "../level/MainForestTemplateScriptActor.h"

// Sets default values
AEnemyCharacter::AEnemyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	Healt_ = 1;

	Punch_Animation = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/characters/cube_man/animations/cube_man_Punch_Montage.cube_man_Punch_Montage"));
	Destroyed_Id = 0;

	OverheadText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("OverheadText"));
	OverheadText->SetupAttachment(RootComponent);

	// Optional: Configure appearance
	OverheadText->SetHorizontalAlignment(EHTA_Center);
	OverheadText->SetVerticalAlignment(EVRTA_TextTop);
	OverheadText->SetWorldSize(50.0f);
	OverheadText->SetText("");

	// Position the text above the character
	OverheadText->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));
	Model_Path =  "H:/workspace/Unreal Projects/MasterMaker4.27/MasterMaker/Binaries/Win64/dolphin3b.gguf";
	if (Model_Path.empty()) {
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, "Model Can ");
	}

}

// Called when the game starts or when spawned
void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetMesh()->SetNotifyRigidBodyCollision(true);
	GetMesh()->SetCollisionProfileName(FName("BlockAll"));
	GetMesh()->OnComponentHit.AddDynamic(this, &AEnemyCharacter::OnHit);
	Cast<AMainForestTemplateScriptActor>(GetWorld()->GetLevelScriptActor())->enemySpawned(this);
	
}


// Called to bind functionality to input
void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyCharacter::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	AEnemyCharacterAIController* AIController = Cast<AEnemyCharacterAIController>(GetController());

	if (AIController)
	{
		if (Cast<ABrick>(OtherActor)) {
			AIController->StopMovement();
			punch(OtherActor);

		}
	}

}


void AEnemyCharacter::punch(AActor* PunchtoActor)
{

	GetCharacterMovement()->DisableMovement();
	PlayAnimMontage(Punch_Animation, 1.0f, FName("Punch"));

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PunchtoActor]()
		{
			if (Cast<ABrick>(PunchtoActor)) {
				Cast<ABrick>(PunchtoActor)->addDamage(3, GetActorLocation(), GetActorForwardVector() * 100);
			}
			GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

		}, 1, false);


}

void AEnemyCharacter::speak(const FString& Message)
{
	OverheadText->SetText(FString(""));
	//UMasterMakerGameInstance *game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());


	//service.setCallBackFunction([&](const std::string& responsePiece) {
	//		if (responsePiece == "<end>")
	//			return;

	//		FString ExistingText = OverheadText->Text.ToString();
	//		ExistingText += FString(UTF8_TO_TCHAR(responsePiece.c_str()));
	//		OverheadText->SetText(FText::FromString(ExistingText));

	//});

	//service.chat(TCHAR_TO_UTF8(*Message));

}
