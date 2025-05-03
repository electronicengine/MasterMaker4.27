// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "brick/weapon/WeaponBrick.h"
#include "Blueprint/UserWidget.h"
#include "brick/Brick.h"
#include "vehicle/EnemyVehicle.h"
#include "brick/MechanicBrick.h"
#include <map>
#include "character/EnemyCharacter.h"

#include "Http.h"
#include "Interfaces/IHttpResponse.h"
#include "LlamaService.h"

#include "MasterMakerGameInstance.generated.h"


#define WEAPON_APPENDIX             "weapon"
#define CLUSTER_WEAPON_APPENDIX     "cluster"
#define EXPLOSIVE_WEAPON_APPENDIX   "explosive"
#define FIRE_WEAPON_APPENDIX        "fire"
#define MELEE_WEAPON_APPENDIX       "melee"
#define VEHICLE_APPENDIX            "Vehicle"
#define CAR_APPENDIX                "CarChasis"
#define CHARACTER_APPENDIX          "Character"
#define MECHANIC_APPENDIX           "mechanic"
#define AI_APPENDIX                 "AI"
#define SIDED_APPENDIX              "sided"
#define LOW_APPENDIX                "Low"
#define HIGH_APPENDIX               "High"


enum MeshCategories {
    general,
    buildings,
    weapons,
    vehicles,
    characters,
    mechanics
};

enum MeshSubCategories {
    vehicle_skeleton,
    vehicle_piece,
    weapon_cluster,
    weapon_explosive,
    weapon_fire,
    weapon_melee,
};

static const std::map<MeshCategories, FString> Mesh_Categories = {
    {MeshCategories::general, "general"},
    {MeshCategories::buildings, "buildings"},
    {MeshCategories::weapons, "weapons"},
    {MeshCategories::vehicles, "vehicles"},
    {MeshCategories::characters, "characters"},
    {MeshCategories::mechanics, "mechanics"}
};

static const std::map<MeshSubCategories, FString> Mesh_Sub_Categories = {
    {MeshSubCategories::vehicle_skeleton, "vehicle_skeleton"},
    {MeshSubCategories::vehicle_piece, "vehicle_piece"},
    {MeshSubCategories::weapon_cluster, "vehicle_piece"},
    {MeshSubCategories::weapon_explosive, "weapon_explosive"},
    {MeshSubCategories::weapon_fire, "weapon_fire"},
    {MeshSubCategories::weapon_melee, "weapon_melee"},
};


static FORCEINLINE void LoadBlueprintFromPath(const FName& Path, std::map<FString, UBlueprintGeneratedClass*>& Array)
{
    TArray<UObject*> tempArray;
    if (EngineUtils::FindOrLoadAssetsByPath(*Path.ToString(), tempArray, EngineUtils::ATL_Class))
    {
        for (int i = 0; i < tempArray.Num(); ++i)
        {
            UObject* temp = tempArray[i];

            if (temp == NULL || (!Cast<UBlueprintGeneratedClass>(temp)))
            {
                continue;
            }
            Array[temp->GetName()] = Cast<UBlueprintGeneratedClass>(temp);

        }
    }
}



/**
 *
 */
UCLASS()
class MASTERMAKER_API UMasterMakerGameInstance : public UGameInstance
{
    GENERATED_BODY()

        UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> Multiplayer_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> SandBox_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> SaveGame_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> GamePlay_Panel_Container;

    UPROPERTY(EditAnywhere, Category = "UI")
        TSubclassOf<class UUserWidget> ChatBox_Panel_Container;


    UBlueprintGeneratedClass* GeneratedClass;


    std::map<FString, UStaticMesh*> General_Brick_Meshes;
    std::map<FString, UStaticMesh*> Character_Meshes;
    std::map<FString, UStaticMesh*> Weapon_Meshes;
    std::map<FString, UStaticMesh*> Building_Meshes;
    std::map<FString, UStaticMesh*> Vehicle_Meshes;
    std::map<FString, UStaticMesh*> Mechanic_Meshes;


    std::map<FString, UBlueprintGeneratedClass*> Mechanic_Classes;
    std::map<FString, UBlueprintGeneratedClass*> Vehicle_Classes;
    std::map<FString, UBlueprintGeneratedClass*> Character_Classes;

    std::map<FString, FItemOptions> Item_Options;

    UMaterial* Default_Brick_Material;

public:
    UMasterMakerGameInstance();

    void initializeItemOptions();
    void initializeDestructedMeshes();
    void initializeThumbnails();

    FItemOptions Selected_Item;
    IComposerInterface* Saved_Item;
    FVector Impact_Point;
    std::map<FString, TArray<UStaticMesh*>> Destructed_Meshes;


    UUserWidget* Multiplayer_Panel;
    UUserWidget* SaveGame_Panel;
    UUserWidget* SandBox_Panel;
    UUserWidget* GamePlay_Panel;
    UUserWidget* ChatBox_Panel;

    UUserWidget* savePanel(IComposerInterface* Item, FVector ImpactPoint);

    UUserWidget* loadChatBoxPanel();
    UUserWidget* loadSandBoxPanel();
    UUserWidget* loadCharacterPanel();
    UUserWidget* loadMultiplayerPanel();

    ABrick* spawnBrick(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AVehicleBase* spawnVehicle(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AEnemyCharacter* spawnCharacter(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AWeaponBrick* spawnWeapon(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AMechanicBrick* spawnMechanic(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params = FActorSpawnParameters());
    AActor* spawnItem(const FVector& SpawnLocation, const FRotator& SpawnRotation, const FItemOptions& Item , const FVector& ImpactPoint = FVector(0, 0, 0), FActorSpawnParameters Params = FActorSpawnParameters());

    void selectCurrentProductItem(const FItemOptions& Item);
    void selectCurrentProductColor(FLinearColor Color);
    bool saveGame(FString Name);
    bool loadGame(FString Name);
    void setScreenMessage(const FString& Message, float TimeOut = 10.0f);
    void hideScreenMessage();
    AActor* findActorByName(const FString& ActorName);
    float findSuitableHeight(AActor* Actor);
    void DownloadFile(const FString& URL, const FString& SavePath);
    void OnDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString SavePath);

    UTexture2D* texture;
    UBlueprintGeneratedClass* Spawned;
    LlamaService Llama_Service;

};
