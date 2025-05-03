// Fill out your copyright notice in the Description page of Project Settings.


#include "PlugInterface.h"
#include "../brick/Brick.h"
#include "../brick/MechanicBrick.h"
#include "../brick/weapon/WeaponBrick.h"
#include "../vehicle/VehicleBase.h"
#include <vector>
#include "Engine/Engine.h"
#include "UObject/Object.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Landscape.h"


IPlugInterface::IPlugInterface()
{

    Owner_Vehicle = nullptr;
    Owner_Item = nullptr;
    Total_Plug_Item = 0;

}


bool IPlugInterface::makeGuidePluginOnTargetActor(UStaticMeshComponent* Ghost_Brick,const FHitResult& OutHit, const FRotator& OffsetRotation, const FVector& OffsetLocation)
{
    if (!Ghost_Brick)
    {
        return false;
    }

    FVector PlugWorldLocation = getPlugWorldLocation(OutHit.Actor, Ghost_Brick, OutHit.ImpactPoint, OutHit.ImpactNormal);
    FRotator PlugWorldRotation = getPlugWorldRotation(Ghost_Brick, OutHit.ImpactPoint, OutHit.ImpactNormal);

    Ghost_Brick->SetRelativeRotation(PlugWorldRotation);

    if (!PlugWorldLocation.IsZero())
    {
        Ghost_Brick->SetVisibility(true);
        Ghost_Brick->SetWorldLocation(PlugWorldLocation);
        Ghost_Brick->AddLocalRotation(OffsetRotation);
        Ghost_Brick->AddLocalOffset(OffsetLocation);

        return true;
    }
    else
    {
        Ghost_Brick->SetVisibility(false);
        return false;
    }
}

bool IPlugInterface::makeGuidePluginOnWorld(UStaticMeshComponent* Ghost_Brick, const FHitResult& OutHit, const FRotator& OffsetRotation, const FVector& OffsetLocation)
{
    int loc_grid_interval = 10;

    int x = OutHit.ImpactPoint.X / loc_grid_interval;
    x = x * loc_grid_interval;

    int y = OutHit.ImpactPoint.Y / loc_grid_interval;
    y = y * loc_grid_interval;

    FVector putting_location = FVector(x, y, OutHit.ImpactPoint.Z);
    FBox meshBounds = Ghost_Brick->Bounds.GetBox();
    FVector bottom_to_pivot = Ghost_Brick->GetComponentLocation() - meshBounds.Min;
    putting_location = putting_location + bottom_to_pivot.Z * OutHit.ImpactNormal;

    FRotator putting_rotation = getPlugWorldRotation(Ghost_Brick, OutHit.ImpactPoint, OutHit.ImpactNormal);

    Ghost_Brick->SetVisibility(true);

    Ghost_Brick->SetWorldLocationAndRotation(putting_location, putting_rotation);

    return true;
}



FRotator IPlugInterface::getPlugWorldRotation(UStaticMeshComponent* ComponentToBePlugged, const FVector& ImpactPoint, const FVector& ImpactNormal)
{
    FVector SurfaceNormal = ImpactNormal.GetSafeNormal();

    // Choose a "forward" direction.  
    // It must not be parallel to SurfaceNormal, otherwise cross products will break.
    // Let's pick a random "forward" (for example, X-axis) and fix it if necessary:
    FVector ForwardVector = FVector::ForwardVector;

    // If forward is almost parallel to normal, pick another axis
    if (FMath::Abs(FVector::DotProduct(SurfaceNormal, ForwardVector)) > 0.99f)
    {
        ForwardVector = FVector::RightVector;
    }

    // Now create rotation
    FVector RightVector = FVector::CrossProduct(SurfaceNormal, ForwardVector).GetSafeNormal();
    ForwardVector = FVector::CrossProduct(RightVector, SurfaceNormal).GetSafeNormal();

    // Build a rotation matrix
    FMatrix RotationMatrix(
        ForwardVector, // X-axis
        RightVector,   // Y-axis
        SurfaceNormal, // Z-axis
        FVector::ZeroVector // Origin
    );

    // Convert to FRotator
    FRotator NewRotation = RotationMatrix.Rotator();
    return NewRotation;
}

FVector IPlugInterface::getPlugWorldLocation(TWeakObjectPtr<AActor> HitActor, UStaticMeshComponent* ComponentToBePlugged, const FVector& ImpactPoint, const FVector& ImpactNormal)
{
    if (!ComponentToBePlugged)
        return FVector(0, 0, 0);

    ALandscape* landScape;
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(Cast<AActor>(this)->GetWorld(), ALandscape::StaticClass(), FoundActors);

    if (FoundActors.Num() > 0)
    {
        landScape = Cast<ALandscape>(FoundActors[0]);
    }
    AActor* act = Cast<AActor>(this);

    FVector hitMeshExtends;
    FVector hitBoundMax;
    FVector hitBoundMin;
    FTransform hitActorTransform;

    if (Cast<AVehicleBase>(HitActor)) {
        Cast<AVehicleBase>(HitActor)->plugMesh->GetLocalBounds(hitBoundMin, hitBoundMax);
        hitMeshExtends = hitBoundMax - hitBoundMin;
        hitActorTransform = Cast<AVehicleBase>(HitActor)->plugMesh->GetComponentTransform();
    }
    else if (Cast<IPlugInterface>(HitActor)) {
        Cast<ABrick>(HitActor)->Brick->GetLocalBounds(hitBoundMin, hitBoundMax);
        hitMeshExtends = hitBoundMax - hitBoundMin;
        hitActorTransform = HitActor->GetActorTransform();
    }

    //int interval = 4;
    float loc_grid_interval_x = 12.5;
    float loc_grid_interval_y = 12.5;
    float loc_grid_interval_z = 12.5;

    // Transform the impact point to local space relative to the HitActor
    FVector localImpactPoint = hitActorTransform.InverseTransformPosition(ImpactPoint);
    FVector localImpactNormal = hitActorTransform.InverseTransformVectorNoScale(ImpactNormal);

    // Snap the local impact point to the grid
    float snappedLocalX = localImpactPoint.X - FMath::Fmod(localImpactPoint.X, loc_grid_interval_x);
    float snappedLocalY = localImpactPoint.Y - FMath::Fmod(localImpactPoint.Y, loc_grid_interval_y);
    float snappedLocalZ = localImpactPoint.Z - FMath::Fmod(localImpactPoint.Z, loc_grid_interval_z);

    FVector snappedLocalPoint(snappedLocalX, snappedLocalY, snappedLocalZ);

    // Check if the point is inside the local bounds (properly accounts for rotation)
    while (snappedLocalPoint.X >= hitBoundMin.X && snappedLocalPoint.X <= hitBoundMax.X &&
        snappedLocalPoint.Y >= hitBoundMin.Y && snappedLocalPoint.Y <= hitBoundMax.Y &&
        snappedLocalPoint.Z >= hitBoundMin.Z && snappedLocalPoint.Z <= hitBoundMax.Z)
    {
        // Move outward along the normal in local space
        snappedLocalPoint += localImpactNormal * 0.1f;
    }

    // Transform the snapped local point back to world space
    FVector putting_location = hitActorTransform.TransformPosition(snappedLocalPoint);

    FVector meshBoundsMin;
    FVector meshBoundsMax;
    ComponentToBePlugged->GetLocalBounds(meshBoundsMin, meshBoundsMax);

    // Adjust for mesh pivot (now in world space)
    putting_location = putting_location - (meshBoundsMin.Z) * ImpactNormal;

    return putting_location;
}


void IPlugInterface::plugTheItem(ABrick* BrickToBePlugged, const FVector& PlugLocation, const FRotator& PlugRotation,
    const FRotator& OffsetRotation, const FVector& OffsetLocation)
{

    attachItem(BrickToBePlugged, PlugLocation, PlugRotation);
    setPlugableOwnerToThisBrick(BrickToBePlugged);

    if (Cast<AWeaponBrick>(BrickToBePlugged)) {
        addWeapon(BrickToBePlugged);

    }

    addPluggedItemList(BrickToBePlugged, OffsetLocation, OffsetRotation);

    
}

void IPlugInterface::putTheItem(const FVector& Location, const FRotator& Rotation)
{
    FVector mechanic_addcomp_location; 
    if (Cast<AMechanicBrick>(this)) {
        mechanic_addcomp_location = Cast<AMechanicBrick>(this)->Add->GetRelativeLocation();
    }

    Cast<ABrick>(this)->enablePhysics(false);
    Cast<ABrick>(this)->setCollisionProfile("OverlapAll");
    const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld, false);
    Cast<ABrick>(this)->DetachFromActor(attachment_rules);
    Cast<ABrick>(this)->enablePhysics(false);
    Cast<ABrick>(this)->setCollisionProfile("OverlapAll");

    Cast<ABrick>(this)->SetActorLocationAndRotation(Location, Rotation,
        false, 0, ETeleportType::ResetPhysics);

    Cast<ABrick>(this)->enablePhysics(true);
    Cast<ABrick>(this)->setCollisionProfile("BlockAll");
    Cast<ABrick>(this)->SetActorEnableCollision(true);

}

FVector IPlugInterface::getPluginRelativeLocation()
{
    FVector relative_location = FVector(0,0,0);
    if (Owner_Item)
        relative_location = Cast<AActor>(Owner_Item)->GetActorLocation() - Cast<AActor>(this)->GetActorLocation();

    return relative_location;
}

FRotator IPlugInterface::getPluginRelativeRotation()
{
    FRotator rotation = FRotator(0,0,0);

    if (Owner_Item)
        rotation = Cast<AActor>(Owner_Item)->GetActorRotation() - Cast<AActor>(this)->GetActorRotation();

    return rotation;
}



void IPlugInterface::autoPlugin(AActor* BelowBrick)
{
    //if (Cast<ABrick>(BelowBrick))
    //{
    //    ABrick* below_brick = Cast<ABrick>(BelowBrick);
    //    FVector plugin = below_brick->getPlugin(Cast<ABrick>(this)->GetActorLocation());
    //    FRotator plugin_rotation = Cast<ABrick>(this)->GetActorRotation();

    //    //    SetActorLocationAndRotation(plugin, plugin_rotation);

    //    //below_brick->plugTheItem(Cast<ABrick>(this), -1, plugin_rotation, FVector(0, 0, 0));
    //}
    //else if (Cast<AVehicleBase>(BelowBrick))
    //{
    //    AVehicleBase* below_vehicle = Cast<AVehicleBase>(BelowBrick);
    //    FVector plugin = below_vehicle->getPlugin(Cast<ABrick>(this)->GetActorLocation());
    //    FRotator plugin_rotation = Cast<ABrick>(this)->GetActorRotation();


    //    //    SetActorLocationAndRotation(plugin, plugin_rotation);

    //    //below_vehicle->plugTheItem(Cast<ABrick>(this), -1, plugin_rotation, FVector(0, 0, 0));
    //}
}


void IPlugInterface::detachItemsOnIt()
{
    AActor* item = Cast<AActor>(this);
    if (item) {
        if (Cast<ABrick>(item) || Cast<AVehicleBase>(item)) {
            for (auto child_item : Plugged_Items_OnIt) {
                if (child_item.second) {
                    if (Cast<AMechanicBrick>(child_item.second)) {
                        Cast<AMechanicBrick>(child_item.second)->Add->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,
                            EDetachmentRule::KeepWorld,
                            EDetachmentRule::KeepWorld, true));

                        Cast<AMechanicBrick>(child_item.second)->enablePhysics(true);
                        Cast<AMechanicBrick>(child_item.second)->setCollisionProfile("BlockAll");

                    }

                    if (Cast<AWeaponBrick>(child_item.second)) {
                        Cast<AWeaponBrick>(child_item.second)->removeOwner();
                    }

                    if (Cast<AVehicleBase>(item)) {
                        child_item.second->Owner_Vehicle = nullptr;
                        child_item.second->Owner_Item = nullptr;

                    }
                    else {
                        child_item.second->Owner_Item = nullptr;
                    }
                }
            }
        }

        if (Cast<ABrick>(item)) {
            const FDetachmentTransformRules& detachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
                EDetachmentRule::KeepWorld,
                EDetachmentRule::KeepWorld, true);
            Cast<AActor>(item)->DetachFromActor(detachment_rules);


            Cast<ABrick>(item)->enablePhysics(true);
            Cast<ABrick>(item)->setCollisionProfile("BlockAll");

            Cast<ABrick>(item)->Brick->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
        }
    }
}


void IPlugInterface::attachItem(ABrick* Object, const FVector& Location, const FRotator& Rotation)
{

    const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld,
        EDetachmentRule::KeepWorld, false);
    Object->DetachFromActor(attachment_rules);
    Object->enablePhysics(false);


    Object->SetActorLocationAndRotation(Location, Rotation,
        false, NULL, ETeleportType::ResetPhysics);

   // Object->AddActorLocalRotation(OffsetRotation);
   // Object->AddActorLocalOffset(OffsetLocation);

    if (Cast<AWeaponBrick>(Object)) {
        Cast<AWeaponBrick>(Object)->makePluginSettings();
    }
    else if (Cast<AMechanicBrick>(Object)) {
        if (Cast<ABrick>(this))
            Cast<AMechanicBrick>(Object)->AttachedComponent = Cast<ABrick>(this)->Brick;
    

    }
    else
    {
        Object->enablePhysics(true);
        Object->setCollisionProfile("BlockAll");
        Object->SetActorEnableCollision(true);
    }

    if (Cast<AMechanicBrick>(this)) {
        Object->AttachToComponent(Cast<AMechanicBrick>(this)->Add, FAttachmentTransformRules(EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld, true));


    }
    else {

        Object->AttachToComponent(Cast<AActor>(this)->GetRootComponent(), FAttachmentTransformRules(EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld,
            EAttachmentRule::KeepWorld, true));


    }
}


void IPlugInterface::setAttachedWeapon(AWeaponBrick* Object) {

    Attached_Weapon = Object;

    if(Owner_Item)
        Owner_Item->setAttachedWeapon(Object);

}

void IPlugInterface::setPlugableOwnerToThisBrick(IPlugInterface* BrickToBePlugged)
{

    if (Owner_Vehicle)
        BrickToBePlugged->Owner_Vehicle = Cast<AVehicleBase>(Owner_Vehicle);
    else if (Cast<AVehicleBase>(this))
        BrickToBePlugged->Owner_Vehicle = Cast<AVehicleBase>(this);

    if (Cast<IPlugInterface>(this))
        BrickToBePlugged->Owner_Item = this;
    else
        BrickToBePlugged->Owner_Item = nullptr;

}

void IPlugInterface::addWeapon(ABrick* Object)
{
    int weapon_index = -1;
    if (Owner_Vehicle) {
        weapon_index = Owner_Vehicle->addWeaponToInventory(Cast<AWeaponBrick>(Object));
        Object->Weapon_Index = weapon_index;
        Cast<AWeaponBrick>(Object)->addedToVehicle(Owner_Vehicle);
    }
    else if (Cast<AVehicleBase>(this)) {
        weapon_index = Cast<AVehicleBase>(this)->addWeaponToInventory(Cast<AWeaponBrick>(Object));
        Object->Weapon_Index = weapon_index;
        Cast<AWeaponBrick>(Object)->addedToVehicle(Cast<AVehicleBase>(this));
    }

    setAttachedWeapon(Cast<AWeaponBrick>(Object));
    

}

void IPlugInterface::addSeat(ABrick* Object)
{
    if (Object->GetName() == "CarSeat2x2")
    {
        //if (Owner_Car != nullptr)
        //    Owner_Car->addSeatToCar(Cast<ACarSeat>(Object));
        //else if (Cast<AVehicleBase>(this))
        //    Cast<AVehicleBase>(this)->addSeatToCar(Cast<ACarSeat>(Object));
    }
}

void IPlugInterface::addPluggedItemList(ABrick* Object, const FVector& OffsetLocation, const FRotator& OffsetRotation)
{

    Object->Item_Index = Total_Plug_Item;
    Object->Offset_Rotation = OffsetRotation;
    Object->Offset_Location = OffsetLocation;
    Plugged_Items_OnIt.insert(std::make_pair(Total_Plug_Item, Object));
    Total_Plug_Item++;
}

void IPlugInterface::notifyToOwnerItemDestroyed(int Id)
{
    if (Owner_Vehicle) {
        Owner_Item->Plugged_Items_OnIt.erase(Id);
    }

    if (Owner_Vehicle && Attached_Weapon) {
        if(!Attached_Weapon->IsActorBeingDestroyed())
            Attached_Weapon->notifyVehicleDetached();
    }

}

void IPlugInterface::setId(int Id)
{
    Id_ = Id;
    if (Cast<IBreakableInterface>(this)) {
        Cast<IBreakableInterface>(this)->Destroyed_Id = Id;
    }
}




