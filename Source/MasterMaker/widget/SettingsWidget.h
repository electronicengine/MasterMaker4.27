// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"


#include "SettingsWidget.generated.h"

/**
 * 
 */
UCLASS()
class MASTERMAKER_API USettingsWidget : public UUserWidget
{
	GENERATED_BODY()

    virtual bool Initialize();

    UPROPERTY(meta = (BindWidget))
        UButton* Button_Continue;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Start;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Multiplayer;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_Load_Item;
    UPROPERTY(meta = (BindWidget))
        UButton* Button_LLAMA;

        void removeWidget();

public:

    UFUNCTION()
    void onClickedButton_Continue();

    UFUNCTION()
    void onClickedButton_Start();

    UFUNCTION()
    void onClickedButton_Multiplayer();

    UFUNCTION()
    void onClickedButton_Load_Item();

    UFUNCTION()
    void onClickedButton_LLAMA();

};
