// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/StaticMeshComponent.h"

#include "ThumbnailGenerator.generated.h"

UCLASS()
class MASTERMAKER_API AThumbnailGenerator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThumbnailGenerator();


	UPROPERTY(EditAnywhere)
	USceneCaptureComponent2D* SceneCapture;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* TargetMesh;


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void delayedFunction();


	UTextureRenderTarget2D* RenderTarget;




public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



    void takeScreenShot();
    bool exportRenderTargetToFile(UTextureRenderTarget2D* renderTarget, const FString& FilePath, const FString& FileName);
	void importPNGToEditor();
	bool generateThumbnail(UStaticMesh* mesh, const FString& OutputPath);
	void adjustViewAngle();
	

};
