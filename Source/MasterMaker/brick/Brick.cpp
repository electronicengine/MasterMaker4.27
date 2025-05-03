// Fill out your copyright notice in the Description page of Project Settings.


#include "Brick.h"
#include <vector>
#include "Engine/Engine.h"
#include "../vehicle/VehicleBase.h"
#include "../MasterMakerGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Containers/UnrealString.h"
#include "Engine/StaticMeshSocket.h"
#include "../character/UserPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInstanceDynamic.h"



// Sets default values
ABrick::ABrick()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;
    Brick = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Brick"));
    SetRootComponent(Brick);
    Brick->SetSimulatePhysics(true);
    Brick->SetCollisionProfileName(FName("BlockAll"));
    //Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
    Brick->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_Yes;

    Breaked_ = false;


}


// Called when the game starts or when spawned
void ABrick::BeginPlay()
{
    Super::BeginPlay();
    First_Hit = true;

}



void ABrick::enablePhysics(bool Value)
{
    if (Brick)
        Brick->SetSimulatePhysics(Value);

}

void ABrick::setCollisionProfile(FString Profile)
{
    if (Brick){
        Brick->SetCollisionProfileName(FName(Profile));
    }

    //if (Cast<AMechanicBrick>(this)) {
    //    Cast<AMechanicBrick>(this)->Add->SetCollisionProfileName(FName(Profile));
    //}

}




UStaticMesh* ABrick::getBrickMesh()
{
    return Ghost_Mesh;
}



void ABrick::setBrickTypeOptions(const FItemOptions& Options)
{

    Brick->SetStaticMesh(Options.Mesh);
    Ghost_Mesh = Options.Mesh;
    Brick_Mesh = Options.Mesh;
    Brick->SetMaterialByName(FName("main"), Options.Material);
    Material = Options.Material;

    Mass_ = Brick->GetMass();

    if (Options.Name.Find("Plate") >= 0) {
        Brick->SetMassOverrideInKg(NAME_None, 1000000, true);
        Brick->SetMassScale(NAME_None, 100000);
        Brick->bIgnoreRadialForce = true;
        Brick->bIgnoreRadialImpulse = true;

    }

    Current_Plugin_Index = 0;

    Brick->CreateDynamicMaterialInstance(0);
    Brick->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Options.Color));
    Item_Options = Options;

    if (Options.CategoryName == Mesh_Categories.at(MeshCategories::mechanics)) {
        Brick->CreateDynamicMaterialInstance(0);
        Brick->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Options.Color));

        Cast<AMechanicBrick>(this)->Add->CreateDynamicMaterialInstance(0);
        Cast<AMechanicBrick>(this)->Add->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Options.Color));
    }
    else {
        Brick->CreateDynamicMaterialInstance(0);
        Brick->SetVectorParameterValueOnMaterials(FName("BaseColor"), FVector(Options.Color));
    }

}


void ABrick::setLegoCarOwner(AVehicleBase* Vehicle)
{
    if (Vehicle != nullptr)
        Owner_Vehicle = Vehicle;
}

void ABrick::applyForceToOwner(FVector Force, FVector Location)
{
    if (Owner_Item) {
        if (Cast<AMechanicBrick>(Owner_Item)) {
            if (Cast<AMechanicBrick>(Owner_Item)->ApplyForce_ToAdd) {
                Cast<AMechanicBrick>(Owner_Item)->Add->AddForceAtLocation(Force / 10, Location);
                Cast<AMechanicBrick>(Owner_Item)->applyForceToOwner(Force / 100, Location);
            }
            else {
                Cast<AMechanicBrick>(Owner_Item)->applyForceToOwner(Force, Location);

            }

        }
        else {
            Cast<ABrick>(Owner_Item)->applyForceToOwner(Force, Location);

        }
    }
    else {
        Brick->AddForceAtLocation(Force, Location);
    }
}




