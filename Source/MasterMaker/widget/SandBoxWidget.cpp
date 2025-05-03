// Fill out your copyright notice in the Description page of Project Settings.


#include "SandBoxWidget.h"
#include "Engine/Engine.h"
#include "../character/UserCharacter.h"
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

#include "../brick/Brick.h"



bool USandBoxWidget::Initialize()
{
    Super::Initialize();
    // Button_1x1Triangle->OnClicked.AddDynamic(this, &USandBoxWidget::Button_1x1TriangleClicked);

    Category_Buttons.push_back(std::make_pair(BrickMenuCategory::General, Button_General_Category));
    Category_Buttons.push_back(std::make_pair(BrickMenuCategory::Vehicle, Button_Vehicle_Category));
    Category_Buttons.push_back(std::make_pair(BrickMenuCategory::Weapon, Button_Weapon_Category));
    Category_Buttons.push_back(std::make_pair(BrickMenuCategory::Building, Button_Building_Category));
    Category_Buttons.push_back(std::make_pair(BrickMenuCategory::Mechanic, Button_Mechanic_Category));
    Category_Buttons.push_back(std::make_pair(BrickMenuCategory::character, Button_Character_Category));

    TArray<UObject*> general_brick_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/general/normal"), general_brick_assets, EngineUtils::ATL_Regular);
    for (auto asset : general_brick_assets)
    {
        UButton* btn = createItemButton(asset, GeneralBrick_Buttons);
        GeneralBrick_Buttons.push_back(std::make_tuple(asset->GetName(), btn, Mesh_Categories.at(general)));

    }

    TArray<UObject*> vehicle_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/vehicles/normal"), vehicle_assets, EngineUtils::ATL_Regular);
    for (auto asset : vehicle_assets)
    {
        UButton* btn = createItemButton(asset, VehicleBrick_Buttons);
        VehicleBrick_Buttons.push_back(std::make_tuple(asset->GetName(), btn, Mesh_Categories.at(vehicles)));

    }

    TArray<UObject*> character_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/characters/normal"), character_assets, EngineUtils::ATL_Regular);
    for (auto asset : character_assets)
    {
        UButton* btn = createItemButton(asset, Character_Buttons);
        Character_Buttons.push_back(std::make_tuple(asset->GetName(), btn, Mesh_Categories.at(characters)));


    }

    TArray<UObject*> weapon_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/weapons/normal"), weapon_assets, EngineUtils::ATL_Regular);
    for (auto asset : weapon_assets)
    {
        UButton* btn = createItemButton(asset, WeaponBrick_Buttons);
        WeaponBrick_Buttons.push_back(std::make_tuple(asset->GetName(), btn, Mesh_Categories.at(weapons)));

    }

    TArray<UObject*> buildings_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/buildings/normal"), buildings_assets, EngineUtils::ATL_Regular);
    for (auto asset : buildings_assets)
    {
        UButton* btn = createItemButton(asset, BuildingBrick_Buttons);
        BuildingBrick_Buttons.push_back(std::make_tuple(asset->GetName(), btn, Mesh_Categories.at(buildings)));


    }

    TArray<UObject*> machines_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/mechanics/normal"), machines_assets, EngineUtils::ATL_Regular);
    for (auto asset : machines_assets)
    {
        UButton* btn = createItemButton(asset, MechanicBrick_Buttons);
        MechanicBrick_Buttons.push_back(std::make_tuple(asset->GetName(), btn, Mesh_Categories.at(mechanics)));

    }


    TArray<UObject*> color_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/colors/"), color_assets, EngineUtils::ATL_Regular);
    for (auto asset : color_assets)
    {
        UButton *btn = createItemButton(asset, Color_Buttons);
        setButtonHover(asset, btn);
        Color_Buttons.push_back(std::make_tuple(asset->GetName(), btn, "colors"));

    }

    TArray<UObject*> general_brick_pressed;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/general/pressed"), general_brick_pressed, EngineUtils::ATL_Regular);
    for (auto asset : general_brick_pressed)
    {
        for (auto& item : GeneralBrick_Buttons) {
            if (asset->GetName().Find(std::get<0>(item)) >= 0) {
                setButtonHover(asset, std::get<1>(item));
            }
        }
    }

    TArray<UObject*> vehicle_assets_pressed;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/vehicles/pressed"), vehicle_assets_pressed, EngineUtils::ATL_Regular);
    for (auto asset : vehicle_assets_pressed)
    {
        for (auto& item : VehicleBrick_Buttons) {
            if (asset->GetName().Find(std::get<0>(item)) >= 0) {
                setButtonHover(asset, std::get<1>(item));
            }
        }
    }

    TArray<UObject*> character_assets_pressed;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/characters/pressed"), character_assets_pressed, EngineUtils::ATL_Regular);
    for (auto asset : character_assets_pressed)
    {
        for (auto& item : Character_Buttons) {
            if (asset->GetName().Find(std::get<0>(item)) >= 0) {
                setButtonHover(asset, std::get<1>(item));
            }
        }
    }

    TArray<UObject*> weapon_assets_pressed;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/weapons/pressed"), weapon_assets_pressed, EngineUtils::ATL_Regular);
    for (auto asset : weapon_assets_pressed)
    {
        for (auto& item : WeaponBrick_Buttons) {
            if (asset->GetName().Find(std::get<0>(item)) >= 0) {
                setButtonHover(asset, std::get<1>(item));
            }
        }
    }

    TArray<UObject*> buildings_assets_pressed;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/buildings/pressed"), buildings_assets_pressed, EngineUtils::ATL_Regular);
    for (auto asset : buildings_assets_pressed)
    {
        for (auto& item : BuildingBrick_Buttons) {
            if (asset->GetName().Find(std::get<0>(item)) >= 0) {
                setButtonHover(asset, std::get<1>(item));
            }
        }
    }

    TArray<UObject*> machines_assets_pressed;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/widgets/thumbnails/mechanics/pressed"), machines_assets_pressed, EngineUtils::ATL_Regular);
    for (auto asset : machines_assets_pressed)
    {
        for (auto& item : MechanicBrick_Buttons) {
            if (asset->GetName().Find(std::get<0>(item)) >= 0) {
                setButtonHover(asset, std::get<1>(item));
            }
        }
    }

    Game_Instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    Current_Category_Buttons = &GeneralBrick_Buttons;
    setVisibilityCategoryButtons(GeneralBrick_Buttons, true);
    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);

    setVisibilityCategoryButtons(Color_Buttons, false);


    return true;
}


void USandBoxWidget::removeWidget()
{
    SetVisibility(ESlateVisibility::Collapsed);

    Game_Instance->loadCharacterPanel();
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    Current_Category_Buttons = &GeneralBrick_Buttons;
    setVisibilityCategoryButtons(GeneralBrick_Buttons, true);
    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
    setVisibilityCategoryButtons(Character_Buttons, false);

    setVisibilityCategoryButtons(Color_Buttons, false);

    controller->bShowMouseCursor = false;


    controller->SetInputMode(FInputModeGameOnly());

}

void USandBoxWidget::checkCategory()
{

    for (int i = 0; i < Category_Buttons.size(); i++) {

        if (Category_Buttons[i].second != nullptr) {
            if (Category_Buttons[i].second->IsPressed()) {

                switch (Category_Buttons[i].first)
                {

                case BrickMenuCategory::General:
                    Current_Category_Buttons = &GeneralBrick_Buttons;
                    setVisibilityCategoryButtons(GeneralBrick_Buttons, true);
                    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
                    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
                    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
                    setVisibilityCategoryButtons(Color_Buttons, false);
                    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
                    setVisibilityCategoryButtons(Character_Buttons, false);

                    break;

                case BrickMenuCategory::Weapon:
                    Current_Category_Buttons = &WeaponBrick_Buttons;

                    setVisibilityCategoryButtons(GeneralBrick_Buttons, false);
                    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
                    setVisibilityCategoryButtons(WeaponBrick_Buttons, true);
                    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
                    setVisibilityCategoryButtons(Color_Buttons, false);
                    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
                    setVisibilityCategoryButtons(Character_Buttons, false);

                    break;

                case BrickMenuCategory::Vehicle:
                    Current_Category_Buttons = &VehicleBrick_Buttons;

                    setVisibilityCategoryButtons(GeneralBrick_Buttons, false);
                    setVisibilityCategoryButtons(VehicleBrick_Buttons, true);
                    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
                    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
                    setVisibilityCategoryButtons(Color_Buttons, false);
                    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
                    setVisibilityCategoryButtons(Character_Buttons, false);

                    break;

                case BrickMenuCategory::Building:
                    Current_Category_Buttons = &BuildingBrick_Buttons;

                    setVisibilityCategoryButtons(GeneralBrick_Buttons, false);
                    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
                    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
                    setVisibilityCategoryButtons(BuildingBrick_Buttons, true);
                    setVisibilityCategoryButtons(Color_Buttons, false);
                    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
                    setVisibilityCategoryButtons(Character_Buttons, false);

                    break;

                case BrickMenuCategory::Mechanic:
                    Current_Category_Buttons = &MechanicBrick_Buttons;

                    setVisibilityCategoryButtons(GeneralBrick_Buttons, false);
                    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
                    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
                    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
                    setVisibilityCategoryButtons(Color_Buttons, false);
                    setVisibilityCategoryButtons(MechanicBrick_Buttons, true);
                    setVisibilityCategoryButtons(Character_Buttons, false);

                    break;
                case BrickMenuCategory::character:
                    Current_Category_Buttons = &Character_Buttons;

                    setVisibilityCategoryButtons(GeneralBrick_Buttons, false);
                    setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
                    setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
                    setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
                    setVisibilityCategoryButtons(Color_Buttons, false);
                    setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
                    setVisibilityCategoryButtons(Character_Buttons, true);

                    break;

                default:
                    break;
                }
            }
        }

    }

}

UButton* USandBoxWidget::createItemButton(UObject *asset, std::vector<std::tuple<FString, UButton*, FString>> &Buttons)
{
    UButton* btn;

    UTexture2D* btn_texture = Cast<UTexture2D>(asset);

    FString name_appendix = FString::FromInt(Buttons.size());

    btn = NewObject<UButton>(UButton::StaticClass());
    btn->AppendName(name_appendix);

    // Set the button's background image using the slate brush
    btn->WidgetStyle.Normal.SetResourceObject(btn_texture);
    btn->WidgetStyle.Normal.SetImageSize(FVector2D(200, 200));
    btn->WidgetStyle.Normal.DrawAs = ESlateBrushDrawType::Image;


    box->AddChild(btn);

    return btn;
}

void USandBoxWidget::setButtonHover(UObject* asset, UButton* Btn)
{
    UTexture2D* btn_texture = Cast<UTexture2D>(asset);

    Btn->WidgetStyle.Hovered.SetResourceObject(btn_texture);
    Btn->WidgetStyle.Hovered.SetImageSize(FVector2D(200, 200));
    Btn->WidgetStyle.Hovered.DrawAs = ESlateBrushDrawType::Image;

    Btn->WidgetStyle.Pressed.SetResourceObject(btn_texture);
    Btn->WidgetStyle.Pressed.SetImageSize(FVector2D(200, 200));
    Btn->WidgetStyle.Pressed.DrawAs = ESlateBrushDrawType::Image;
}

void USandBoxWidget::setVisibilityCategoryButtons(const std::vector<std::tuple<FString, UButton*, FString>>& CategoryButtons, bool Visible)
{
    for (int i = 0; i < CategoryButtons.size(); i++) {
        if (Visible) {
            std::get<1>(CategoryButtons[i])->SetVisibility(ESlateVisibility::Visible);
        }
        else {
            std::get<1>(CategoryButtons[i])->SetVisibility(ESlateVisibility::Collapsed);
        }
    }
}

void USandBoxWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{

    if (GetVisibility() == ESlateVisibility::Visible) {

        checkCategory();
        std::vector<std::tuple<FString, UButton*, FString>>& category = *Current_Category_Buttons;

        if (category == Color_Buttons) {
            for (int i = 0; i < category.size(); i++) {

                if (std::get<1>(category[i]) != nullptr) {
                    if (std::get<1>(category[i])->IsPressed()) {

                        Game_Instance->selectCurrentProductColor(Colors[std::get<0>(category[i])]);
                        removeWidget();
                    }
                }

            }
        }
        else
        {
            for (int i = 0; i < category.size(); i++) {

                if (std::get<1>(category[i]) != nullptr) {
                    if (std::get<1>(category[i])->IsPressed()) {

                        FItemOptions itemOptions;
                        itemOptions.Name = std::get<0>(category[i]);
                        itemOptions.CategoryName = std::get<2>(category[i]);

                        Game_Instance->selectCurrentProductItem(itemOptions);

                        setVisibilityCategoryButtons(GeneralBrick_Buttons, false);
                        setVisibilityCategoryButtons(VehicleBrick_Buttons, false);
                        setVisibilityCategoryButtons(WeaponBrick_Buttons, false);
                        setVisibilityCategoryButtons(BuildingBrick_Buttons, false);
                        setVisibilityCategoryButtons(MechanicBrick_Buttons, false);
                        setVisibilityCategoryButtons(Character_Buttons, false);

                        setVisibilityCategoryButtons(Color_Buttons, true);

                        Current_Category_Buttons = &Color_Buttons;
                    }
                }

            }
        }


    }


}


