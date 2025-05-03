// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsWidget.h"
#include "../MasterMakerGameInstance.h"
#include "../character/UserCharacter.h"

bool USettingsWidget::Initialize()
{

    Super::Initialize();

    Button_Continue->OnClicked.AddDynamic(this, &USettingsWidget::onClickedButton_Continue);
    Button_Start->OnClicked.AddDynamic(this, &USettingsWidget::onClickedButton_Start);
    Button_Load_Item->OnClicked.AddDynamic(this, &USettingsWidget::onClickedButton_Load_Item);
    Button_Multiplayer->OnClicked.AddDynamic(this, &USettingsWidget::onClickedButton_Multiplayer);
    Button_LLAMA->OnClicked.AddDynamic(this, &USettingsWidget::onClickedButton_LLAMA);

    return true;
}

void USettingsWidget::removeWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);
    UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetGameInstance());


    game_instance->loadCharacterPanel();
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    controller->SetInputMode(FInputModeGameOnly());
    controller->bShowMouseCursor = false;
}


void USettingsWidget::onClickedButton_Continue()
{
    removeWidget();
}

void USettingsWidget::onClickedButton_Start()
{
}

void USettingsWidget::onClickedButton_Multiplayer()
{
    AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
	if (character) {
        character->host();
        removeWidget();

	}

}

void USettingsWidget::onClickedButton_Load_Item()
{

    AUserCharacter* character = Cast<AUserCharacter>(GetOwningPlayerPawn());
    if (character) {
        character->saveAndLoad();
        removeWidget();
    }
}

void USettingsWidget::onClickedButton_LLAMA()
{
    SetVisibility(ESlateVisibility::Collapsed);
    Cast<UMasterMakerGameInstance>(GetGameInstance())->loadLlamaPanel();
}
