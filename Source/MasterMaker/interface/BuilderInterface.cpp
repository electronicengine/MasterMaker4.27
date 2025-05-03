// Fill out your copyright notice in the Description page of Project Settings.


#include "BuilderInterface.h"
#include "../brick/Brick.h"
#include "Kismet/GameplayStatics.h"
#include "../brick/MechanicBrick.h"
#include "Engine/World.h"
#include "../widget/GamePlayWidget.h"
#include "../MasterMakerGameInstance.h"
#include "../character/UserPlayerState.h"

IBuilderInterface::IBuilderInterface()
{



    Brick_Plugable = true;
    Brick_Just_Plug = false;
    Keeping_Bricks = false;
    Building = false;
    OffSet_Rotation.Roll = 0;
    OffSet_Rotation.Yaw = 0;
    OffSet_Rotation.Pitch = 0;
    OffSet_Location = FVector(0, 0, 0);
    Offset_Step = 0;

}

// Add default functionality here for any IBuilderInterface functions that are not pure virtual.
void IBuilderInterface::buildStart()
{
    Building = true;

}

void IBuilderInterface::buildEnd()
{

    Building = false;
    Ghost->SetVisibility(false);
    Target_Plugable_Item = nullptr;
    Brick_Just_Plug = false;
    Tick_Count = 0;


}

void IBuilderInterface::grabObject(ABrick* Object)
{

    if (Object && Cast<ACharacter>(this))
    {

        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);
        Object->enablePhysics(false);
        Object->setCollisionProfile("OverlapAll");


        Object->SetActorLocation(Cast<AUserCharacter>(this)->GetMesh()->GetSocketLocation(FName("grab_socket")),
            false, nullptr, ETeleportType::ResetPhysics);

        Object->AttachToComponent(Cast<AUserCharacter>(this)->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget,
            EAttachmentRule::SnapToTarget,
            EAttachmentRule::KeepWorld, false), TEXT("grab_socket"));

        Keeping_Bricks = true;

        FVector Origin;
        FVector BoxExtent;
        float SphereRadius;

        UKismetSystemLibrary::GetComponentBounds(Ghost, Origin, BoxExtent, SphereRadius);

        if (BoxExtent.Y >= BoxExtent.X)
            Pivot_Width = (BoxExtent.Y / 12.5);
        else
            Pivot_Width = (BoxExtent.X / 12.5);

        Object->Grabbed = true;

        if (Object->Owner_Item) {
            Object->notifyToOwnerItemDestroyed(Object->Item_Index);
        }

        Ghost->SetVisibility(false);
        Ghost->SetStaticMesh(Object->Brick_Mesh);
    }

}



void IBuilderInterface::dropObject(ABrick* Object)
{

    if (Object != nullptr)
    {
        const FDetachmentTransformRules& attachment_rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld,
            EDetachmentRule::KeepWorld,
            EDetachmentRule::KeepWorld, true);

        Object->DetachFromActor(attachment_rules);
        Object->enablePhysics(true);
        Object->setCollisionProfile("BlockAll");
        //        Object->SetActorEnableCollision(true);

        Grabbable_Brick = nullptr;
        Object_NearBy = nullptr;
        Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Hidden);

        Ghost->SetVisibility(false);

    }

    Keeping_Bricks = false;
    Object->Grabbed = false;

}



void IBuilderInterface::plugObject(ABrick* GrabbedBrick, IPlugInterface* TargetPlugable, const FVector& TargetLocation, const FRotator& TargetRotation,
                                     const FRotator& OffsetRotation, const FVector& OffsetLocation)
{

    if (TargetPlugable != nullptr && GrabbedBrick != Cast<ABrick>(TargetPlugable))
    {
        TargetPlugable->plugTheItem(GrabbedBrick, TargetLocation, TargetRotation, OffsetRotation, OffsetLocation);
    }
    else {
        GrabbedBrick->putTheItem(TargetLocation, TargetRotation);
    }

}



void IBuilderInterface::giveOffsetRotation()
{
    if (OffSet_Location == FVector(0, 0, 0)) {
        if (OffSet_Rotation.Yaw == 360)
            OffSet_Rotation.Yaw = 0;

        OffSet_Rotation.Yaw += 90;
    }

}

void IBuilderInterface::giveOffsetLocation()
{

    int offset = 0;

    if (Ghost) {
        Offset_Step += 1;

        if (Pivot_Width != 0)
            offset = Offset_Step % Pivot_Width;
        else
            offset = 0;


        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Black, FString::FromInt(Pivot_Width));
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, FString::FromInt(Offset_Step));
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, FString::FromInt(offset));


        if (Ghost->IsVisible()) {
            OffSet_Location -= FVector(0, 25, 0);

        }


        if (offset == 0) {
            OffSet_Location = FVector(0, 0, 0);
            Offset_Step = 0;
        }


    }
}

void IBuilderInterface::lookForBuildingSpace(AActor* Target, FHitResult& OutHit)
{

    IPlugInterface* target_plugable = Cast<IPlugInterface>(Target);

    checkGhostItemOverlap();

    if (target_plugable)
    {
        if (Target_Plugable_Item != target_plugable) {
            OffSet_Location = FVector(0, 0, 0);
            OffSet_Rotation = FRotator(0, 0, 0);
        }

        Target_Plugable_Item = target_plugable;

        if (Grabbable_Brick) {
            Brick_Plugable = target_plugable->makeGuidePluginOnTargetActor(Ghost, OutHit, OffSet_Rotation, OffSet_Location);
        }

        if (!Keeping_Bricks && Game_Instance->GamePlay_Panel)
            Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Visible);
    }
    else
    {

        if (Grabbable_Brick && Ghost && OutHit.IsValidBlockingHit()) {
            if (Grabbable_Brick->getBrickMesh())
                Ghost->SetStaticMesh(Grabbable_Brick->getBrickMesh());

            target_plugable->makeGuidePluginOnWorld(Ghost, OutHit, OffSet_Rotation, OffSet_Location);

        }

        Target_Plugable_Item = nullptr;

        if(Game_Instance->GamePlay_Panel)
            Cast<UGamePlayWidget>(Game_Instance->GamePlay_Panel)->setInteractButtonVisibilty(ESlateVisibility::Hidden);

    }

}



float IBuilderInterface::getOverlapPercentage(UStaticMeshComponent * GhostComponent)
{
    // Get ghost's bounding box
    const FBox GhostBox = GhostComponent->Bounds.GetBox();
    const float GhostVolume = GhostBox.GetVolume();

    if (GhostVolume <= 0.0f)
    {
        return 0.0f;
    }

    // Get all overlapping components
    TArray<UPrimitiveComponent*> OverlappingComponents;
    GhostComponent->GetOverlappingComponents(OverlappingComponents);

    float TotalIntersectionVolume = 0.0f;

    for (UPrimitiveComponent* OverlappedComp : OverlappingComponents)
    {
        if (!OverlappedComp)
        {
            continue;
        }

        AActor* OwnerActor = OverlappedComp->GetOwner();
        if (!OwnerActor || Cast<AVehicleBase>(OwnerActor)) // Skip unwanted actors
        {
            continue;
        }

        const FBox OverlappedBox = OverlappedComp->Bounds.GetBox();
        const FBox IntersectionBox = GhostBox.Overlap(OverlappedBox);

        if (IntersectionBox.IsValid)
        {
            TotalIntersectionVolume += IntersectionBox.GetVolume();
        }
    }

    // Return the total overlapping percentage relative to the Ghost volume
    return (TotalIntersectionVolume / GhostVolume) * 100.0f;
}


void IBuilderInterface::checkGhostItemOverlap()
{
    if (!Ghost)
    {
        return;
    }

    float OverlapPercent = getOverlapPercentage(Ghost);
    GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::SanitizeFloat(OverlapPercent));
    UMaterial* MaterialToApply = (OverlapPercent >= 15.0f) ? Ghost_Imposible_Material : Ghost_Possible_Material;
    Ghost->SetMaterial(0, MaterialToApply);
    Ghost->SetMaterial(1, MaterialToApply);
}

