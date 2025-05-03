// Fill out your copyright notice in the Description page of Project Settings.


#include "ThumbnailGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "ImageUtils.h"


// Sets default values
AThumbnailGenerator::AThumbnailGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    


    TargetMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("target"));
    SetRootComponent(TargetMesh);


    SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("scene capture"));
    SceneCapture->SetupAttachment(RootComponent);
    SceneCapture->CaptureSource = ESceneCaptureSource::SCS_FinalColorHDR;
    SceneCapture->ShowFlags.Atmosphere = false;
    SceneCapture->ShowFlags.SkyLighting = false;
    SceneCapture->ShowFlags.Fog = false;
    SceneCapture->ShowFlags.VolumetricFog = false;
    SceneCapture->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
    SceneCapture->FOVAngle = 50;
}

// Called when the game starts or when spawned
void AThumbnailGenerator::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AThumbnailGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AThumbnailGenerator::generateThumbnail(UStaticMesh* mesh, const FString &OutputPath)
{
    TargetMesh->SetStaticMesh(mesh);

    FTimerHandle MyTimerHandle1;
    GetWorld()->GetTimerManager().SetTimer(
        MyTimerHandle1,
        [this]() {
            adjustViewAngle();
            takeScreenShot();
        },
        0.4f,
        false
    );
    //"widgets/thumbnails/generated_thumbnails"

    FTimerHandle MyTimerHandle2;
    GetWorld()->GetTimerManager().SetTimer(
        MyTimerHandle2,
        [this, OutputPath]() {
            FString name = TargetMesh->GetStaticMesh()->GetName();
            FString path = FPaths::ProjectContentDir() + OutputPath;
            exportRenderTargetToFile(RenderTarget, path, name);
            Destroy();
        },
        0.8f,
        false
    );

    return false;
}

void AThumbnailGenerator::adjustViewAngle()
{
    if (TargetMesh)
    {
        FBoxSphereBounds Bounds = TargetMesh->CalcBounds(TargetMesh->GetComponentTransform());

        FVector Center = Bounds.Origin;
        float Radius = Bounds.SphereRadius;

        // Angle offset in degrees
        float AngleDegrees = 30.0f;
        float AngleRadians = FMath::DegreesToRadians(AngleDegrees);

        // Polar to Cartesian offset: angle around Z axis (yaw), with radius distance
        float Distance = Radius * 2.5f;
        float HeightOffset = Radius * 0.5f;

        FVector CameraOffset;
        CameraOffset.X = -Distance * FMath::Cos(AngleRadians); // X axis (forward/back)
        CameraOffset.Y = Distance * FMath::Sin(AngleRadians);  // Y axis (sideways)
        CameraOffset.Z = HeightOffset;

        FVector CameraLocation = Center + CameraOffset;

        SceneCapture->SetWorldLocation(CameraLocation);

        // Make the camera look at the mesh center
        FRotator LookAtRotation = (Center - CameraLocation).Rotation();
        SceneCapture->SetWorldRotation(LookAtRotation);
    }
}


void AThumbnailGenerator::takeScreenShot()
{

    // Create a new Render Target
    RenderTarget = NewObject<UTextureRenderTarget2D>();
    RenderTarget->InitAutoFormat(2048, 2048);
    // Set the render target properties

    RenderTarget->TargetGamma = 2.2f;
    RenderTarget->ClearColor = FLinearColor::Black;
    RenderTarget->bAutoGenerateMips = false;
    RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA8;

    RenderTarget->UpdateResource();
    SceneCapture->TextureTarget = RenderTarget;
    
    // Show Only the selected component
    SceneCapture->ShowOnlyComponent(TargetMesh);
}

void AThumbnailGenerator::delayedFunction()
{


}

bool AThumbnailGenerator::exportRenderTargetToFile(UTextureRenderTarget2D* renderTarget, const FString& FilePath, const FString& FileName)
{
    if (!renderTarget) return false;

    FTextureRenderTargetResource* RTResource = renderTarget->GameThread_GetRenderTargetResource();
    if (!RTResource) return false;

    // Read pixels from render target
    TArray<FColor> OutPixels;
    FIntRect Rect(0, 0, renderTarget->SizeX, renderTarget->SizeY);

    if (!RTResource->ReadPixels(OutPixels, FReadSurfaceDataFlags(), Rect))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to read pixels from render target"));
        return false;
    }

    // Process pixels to make black transparent
    for (FColor& Pixel : OutPixels)
    {
        // You can adjust this threshold to match what you consider "black"
        const uint8 BlackThreshold = 10;

        if (Pixel.R <= BlackThreshold && Pixel.G <= BlackThreshold && Pixel.B <= BlackThreshold)
        {
            Pixel.A = 0; // Set alpha to fully transparent
        }
        else
        {
            Pixel.A = 255; // Set alpha to fully opaque
        }
    }

    // Create bitmap and save
    FIntPoint DestSize(renderTarget->SizeX, renderTarget->SizeY);
    TArray<uint8> CompressedPNG;

    // Make sure to use the PNG format with alpha channel
    FImageUtils::CompressImageArray(DestSize.X, DestSize.Y, OutPixels, CompressedPNG);

    FString FullPath = FPaths::Combine(FilePath, FileName + ".png");

    if (!FFileHelper::SaveArrayToFile(CompressedPNG, *FullPath))
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to save image to %s"), *FullPath);
        return false;
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully exported render target to %s"), *FullPath);

    return true;
}


void AThumbnailGenerator::importPNGToEditor()
{

}

