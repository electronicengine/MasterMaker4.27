// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "UObject/Object.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include <vector>
#include <map>
#include <list>
#include "PlugInterface.generated.h"

#define BRICK_LENGHT        50

class ABrick;
class AVehicleBase;
class AWeaponBrick;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlugInterface : public UInterface
{
    GENERATED_BODY()
};

/**
 *
 */
class MASTERMAKER_API IPlugInterface
{
    GENERATED_BODY()




        // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
    UStaticMeshComponent* Brick;

    std::map<int, ABrick*> Plugged_Items_OnIt;
    bool plugged;
    int Current_Plugin_Index;
    float Height_Offset;
    int Item_Index;
    int Weapon_Index;
    int Total_Plug_Item;
    AVehicleBase* Owner_Vehicle;
    IPlugInterface* Owner_Item;
    FRotator Offset_Rotation;
    FVector Offset_Location;
    AWeaponBrick* Attached_Weapon;
    int Id_;

    IPlugInterface();
    void setAttachedWeapon(AWeaponBrick* Object);

    FRotator getPlugWorldRotation(UStaticMeshComponent* ComponentToBePlugged, const FVector& ImpactPoint, const FVector& ImpactNormal);
    FVector getPlugWorldLocation(TWeakObjectPtr<AActor> HitActor, UStaticMeshComponent* ComponentToBePlugged, const FVector& ImpactPoint, const FVector& ImpactNormal);
    void plugTheItem(ABrick* BrickToBePlugged, const FVector& PlugLocation, const FRotator& PlugRotation,
        const FRotator& OffsetRotation, const FVector& OffsetLocation);
    void putTheItem(const FVector& Location, const FRotator& Rotation);
    FVector getPluginRelativeLocation();
    FRotator getPluginRelativeRotation();

    bool makeGuidePluginOnTargetActor(UStaticMeshComponent* Ghost_Brick, const FHitResult& OutHit, const FRotator& OffsetRotation, const FVector& OffsetLocation);
    bool makeGuidePluginOnWorld(UStaticMeshComponent* Ghost_Brick, const FHitResult& OutHit, const FRotator& OffsetRotation, const FVector& OffsetLocation);
    void autoPlugin(AActor* TargetBrick);
    void detachItemsOnIt();
    void attachItem(ABrick* BrickToBePlugged, const FVector& Location, const FRotator& Rotation);
    void setPlugableOwnerToThisBrick(IPlugInterface* BrickToBePlugged);
    void addWeapon(ABrick* Object);
    void addSeat(ABrick* Object);
    void addPluggedItemList(ABrick* Object, const FVector& OffsetLocation, const FRotator& OffsetRotation);
    void notifyToOwnerItemDestroyed(int Id);
    void setId(int Id);
    int getId() { return Id_; }
};
