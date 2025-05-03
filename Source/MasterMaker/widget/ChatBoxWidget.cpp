// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatBoxWidget.h"
#include "Engine/Engine.h"
#include "../character/UserCharacter.h"
#include "../character/UserPlayerState.h"
#include "../character/EnemyCharacter.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "RendererInterface.h"
#include "DrawDebugHelpers.h"
#include "../MasterMakerGameInstance.h"
#include "Engine/World.h"
#include "Components/Image.h"
#include "Slate/SlateBrushAsset.h"
#include "Styling/SlateBrush.h"
#include "SlateBasics.h"
#include "Widgets/Images/SImage.h"


#include "Runtime/Engine/Classes/Engine/Texture2D.h"

bool UChatBoxWidget::Initialize()
{

    Super::Initialize();
    Button_Say->OnClicked.AddDynamic(this, &UChatBoxWidget::onClickedButton_Say);

    return true;
}

void UChatBoxWidget::removeWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());


    game_instance->loadCharacterPanel();
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    controller->SetInputMode(FInputModeGameOnly());
    controller->bShowMouseCursor = false;

}

void UChatBoxWidget::setInterlocutor(AActor* Actor)
{
    Interlocutor = Actor;
}

void UChatBoxWidget::onClickedButton_Say()
{
    FText txt = message->GetText();
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    if (Cast<AEnemyCharacter>(Interlocutor)) {
        Cast<AEnemyCharacter>(Interlocutor)->speak(txt.ToString());
    }

    removeWidget();
}
