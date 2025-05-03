// Fill out your copyright notice in the Description page of Project Settings.


#include "ComposerInterface.h"
#include "../vehicle/EnemyVehicle.h"
#include "../brick/Brick.h"
#include "../MasterMakerGameInstance.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "../widget/SandBoxWidget.h"
#include "../MasterMakerGameInstance.h"


// Add default functionality here for any IBuiltInInterface functions that are not pure virtual.

FString IComposerInterface::Spawned_Vehicle_Name;
int IComposerInterface::Start_ID = 0;

FConstructionInfo IComposerInterface::compileConstructInfo(IComposerInterface* Object)
{
    FConstructionInfo build_info;


    AVehicleBase* vehicle = Cast<AVehicleBase>(Object);

    if (vehicle)
    {

        if (Cast<AEnemyVehicle>(vehicle)) {
            build_info.Item_Options.Name = Cast<AEnemyVehicle>(vehicle)->GetName();
        }
        else {
            build_info.Item_Options.Name = Cast<AVehicleBase>(vehicle)->GetName();
        }
        build_info.Item_Options.Color = Colors["White"];
        build_info.Item_Options.CategoryName = Mesh_Categories.at(vehicles);
        build_info.Item_Options.SubCategoryName = Mesh_Sub_Categories.at(vehicle_skeleton);
        build_info.Offset_Rotation = vehicle->Offset_Rotation;
        build_info.Offset_Location = vehicle->Offset_Location;


        for (std::pair<int, ABrick*> var : vehicle->Plugged_Items_OnIt)
        {
            FConstructionInfo brick_info;
            brick_info = compileConstructInfo(var.second);

            build_info.Plugged_Items.push_back(brick_info);
        }

    }
    else
    {
        ABrick* brick = Cast<ABrick>(Object);
        if (brick)
        {

            build_info.Item_Options = brick->Item_Options;
            build_info.Plugin_Relative_Location = brick->getPluginRelativeLocation();
            build_info.Offset_Rotation = brick->Offset_Rotation;
            build_info.Offset_Location = brick->Offset_Location;


            for (std::pair<int, ABrick*> var : brick->Plugged_Items_OnIt)
            {
                FConstructionInfo brick_info;
                brick_info = compileConstructInfo(var.second);

                build_info.Plugged_Items.push_back(brick_info);
            }

        }

    }



    return build_info;
}



void IComposerInterface::buildFromConstructionInfo(const FConstructionInfo& Info, const FVector& BuildLocation,
    UMasterMakerGameInstance* GameInstance, ABrick* MainBrick, bool IsServer)
{

    AVehicleBase* vehicle = nullptr;
    ABrick* main_brick = nullptr;
    AWeaponBrick* weapon = nullptr;
    AMechanicBrick* machine = nullptr;

    if (Info.Item_Options.CategoryName == Mesh_Categories.at(vehicles) && Info.Item_Options.SubCategoryName == Mesh_Sub_Categories.at(vehicle_skeleton))
    {
        if (IsServer) {

            vehicle = GameInstance->spawnVehicle(Info.Item_Options, BuildLocation + FVector(0, 0, 30.0f), FRotator(0, 0, 0));
            Spawned_Vehicle_Name = vehicle->GetName();
        }
        else {
            vehicle = Cast<AVehicleBase>(GameInstance->findActorByName(Info.Item_Options.Name));
        }
        Cast<IPlugInterface>(vehicle)->setId(Start_ID++);

    }
    else
    {
        if (MainBrick == nullptr)
        {
            GameInstance->selectCurrentProductColor(Info.Item_Options.Color);
            if (Info.Item_Options.CategoryName == Mesh_Categories.at(mechanics)) {
                main_brick = Cast<ABrick>(GameInstance->spawnMechanic(Info.Item_Options, FVector(0, 0, 0), FRotator(0, 0, 0)));
                main_brick->putTheItem(BuildLocation + FVector(0, 0, 10.0f), FRotator(0, 0, 0));

            }
            else {
                main_brick = GameInstance->spawnBrick(Info.Item_Options, FVector(0, 0, 0), FRotator(0, 0, 0));
                main_brick->putTheItem(BuildLocation + FVector(0, 0, 10.0f), FRotator(0, 0, 0));
            }
            Cast<IPlugInterface>(main_brick)->setId(Start_ID++);


        }
        else
        {
            main_brick = MainBrick;
        }
    }



    for (int i = 0; i < Info.Plugged_Items.size(); i++)
    {
        ABrick* brick;

        GameInstance->selectCurrentProductColor(Info.Plugged_Items[i].Item_Options.Color);
        if (Info.Plugged_Items[i].Item_Options.CategoryName == Mesh_Categories.at(weapons)) {
            brick = Cast<ABrick>(GameInstance->spawnWeapon(Info.Plugged_Items[i].Item_Options, FVector(0, 0, 0), FRotator(0, 0, 0)));
        }
        else if (Info.Plugged_Items[i].Item_Options.CategoryName == Mesh_Categories.at(mechanics)) {
            FVector location;
            if (main_brick)
                location = main_brick->GetActorLocation() + FVector(50, 0, 0);
            else if (vehicle)
                location = vehicle->GetActorLocation() + FVector(50, 0, 0);

            brick = Cast<ABrick>(GameInstance->spawnMechanic(Info.Item_Options, location, FRotator(0, 0, 0)));
            brick->enablePhysics(false);
            brick->setCollisionProfile("OverlapAll");
        }
        else {
            brick = GameInstance->spawnBrick(Info.Plugged_Items[i].Item_Options, FVector(0, 0, 0), FRotator(0, 0, 0));
        }

        Cast<IPlugInterface>(brick)->setId(Start_ID++);


        if (vehicle != nullptr)
        {
            vehicle->plugTheItem(brick, vehicle->GetActorLocation() + Info.Plugged_Items[i].Plugin_Relative_Location,
                vehicle->GetActorRotation(),
                Info.Plugged_Items[i].Offset_Rotation,
                Info.Plugged_Items[i].Offset_Location);
        }
        else if (main_brick != nullptr)
        {
            main_brick->plugTheItem(brick, main_brick->GetActorLocation() + Info.Plugged_Items[i].Plugin_Relative_Location,
                main_brick->GetActorRotation(),
                Info.Plugged_Items[i].Offset_Rotation,
                Info.Plugged_Items[i].Offset_Location);
        }


        buildFromConstructionInfo(Info.Plugged_Items[i], BuildLocation, GameInstance, brick);

    }
}
