// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"

#include <vector>
#include <memory>
#include <map>

#include "ChatBoxWidget.generated.h"


/**
 * 
 */
UCLASS()
class MASTERMAKER_API UChatBoxWidget : public UUserWidget
{

	GENERATED_BODY()

    virtual bool Initialize();

    UPROPERTY(meta = (BindWidget))
    UButton* Button_Say;

    UPROPERTY(meta = (BindWidget))
    UEditableTextBox* message;
    AActor* Interlocutor;

    void removeWidget();

public:

    UFUNCTION()
    void onClickedButton_Say();
    void setInterlocutor(AActor* Actor);

};
