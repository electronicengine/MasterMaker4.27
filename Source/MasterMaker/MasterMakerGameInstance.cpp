// Fill out your copyright notice in the Description page of Project Settings.


#include "MasterMakerGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/Button.h"
#include "vehicle/EnemyVehicleAIController.h"
#include "widget/SaveGameWidget.h"
#include "widget/GamePlayWidget.h"
#include "level/MasterMakerSaveGame.h"
#include "character/UserCharacter.h"
#include "character/UserPlayerState.h"
#include "ThumbnailGenerator.h"



UMasterMakerGameInstance::UMasterMakerGameInstance()
{

    Selected_Item.Name = "High2x2";
    Selected_Item.CategoryName = Mesh_Categories.at(general);
    Selected_Item.Color = FColor::White;

    SandBox_Panel = nullptr;
    GamePlay_Panel = nullptr;

    //TArray<FString> MeshPaths = {
    //TEXT("/Game/bricks/meshes/general/Brick01.Brick01"),
    //TEXT("/Game/bricks/meshes/general/Brick02.Brick02"),
    //TEXT("/Game/bricks/meshes/general/Brick03.Brick03")
    //};

    //for (const FString& Path : MeshPaths)
    //{
    //    ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(*Path);
    //    if (MeshFinder.Succeeded())
    //    {
    //        UStaticMesh* Mesh = MeshFinder.Object;
    //        if (Mesh)
    //        {
    //            General_Brick_Meshes.emplace(std::make_pair(Mesh->GetName(), Mesh));
    //        }
    //    }
    //}

    TArray<UObject*> general_brick_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/general"), general_brick_assets, EngineUtils::ATL_Regular);
    for (auto asset : general_brick_assets)
    {
        FString path = asset->GetPathName();
        General_Brick_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> weapon_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/weapons"), weapon_assets, EngineUtils::ATL_Regular);
    for (auto asset : weapon_assets)
    {
        Weapon_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> vehicle_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/vehicles"), vehicle_assets, EngineUtils::ATL_Regular);
    for (auto asset : vehicle_assets)
    {
        Vehicle_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> building_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/buildings"), building_assets, EngineUtils::ATL_Regular);
    for (auto asset : building_assets)
    {
        Building_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> character_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/characters"), character_assets, EngineUtils::ATL_Regular);
    for (auto asset : character_assets)
    {
        Character_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }

    TArray<UObject*> mechanic_assets;
    EngineUtils::FindOrLoadAssetsByPath(FString("/Game/bricks/meshes/mechanics"), mechanic_assets, EngineUtils::ATL_Regular);
    for (auto asset : mechanic_assets)
    {
        Mechanic_Meshes[asset->GetName()] = Cast<UStaticMesh>(asset);
    }


    // default material
    static ConstructorHelpers::FObjectFinder<UMaterial>
        material_asset(TEXT("Material'/Game/bricks/materials/brick_material.brick_material'"));
    Default_Brick_Material = material_asset.Object;


    LoadBlueprintFromPath(FName("/Game/bricks/mechanic_bricks/"), Mechanic_Classes);
    LoadBlueprintFromPath(FName("/Game/characters/"), Character_Classes);
    LoadBlueprintFromPath(FName("/Game/vehicles/"), Vehicle_Classes);


    initializeItemOptions();
    initializeDestructedMeshes();

}

void UMasterMakerGameInstance::initializeItemOptions()
{
    Selected_Item.Mesh = General_Brick_Meshes["High2x2"];
}






void UMasterMakerGameInstance::initializeDestructedMeshes()
{

    for (std::pair<FString, UStaticMesh*> var : General_Brick_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/meshes/destructed/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

    for (std::pair<FString, UStaticMesh*> var : Building_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/meshes/destructed/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

    for (std::pair<FString, UStaticMesh*> var : Character_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/meshes/destructed/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

    for (std::pair<FString, UStaticMesh*> var : Mechanic_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/meshes/destructed/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

    for (std::pair<FString, UStaticMesh*> var : Vehicle_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/meshes/destructed/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

    for (std::pair<FString, UStaticMesh*> var : Weapon_Meshes) {
        TArray<UObject*> MeshAssets;
        FString meshes_path = "/Game/bricks/meshes/destructed/";
        meshes_path += var.first;

        EngineUtils::FindOrLoadAssetsByPath(meshes_path, MeshAssets, EngineUtils::ATL_Regular);
        int i = 0;
        for (auto asset : MeshAssets)
        {
            UStaticMesh* mesh = Cast<UStaticMesh>(asset);
            Destructed_Meshes[var.first].Push(mesh);
        }
    }

}


void UMasterMakerGameInstance::initializeThumbnails()
{
    
    for (auto& mesh : General_Brick_Meshes) {
        AThumbnailGenerator* thumbnail_generator = GetWorld()->SpawnActor<AThumbnailGenerator>(AThumbnailGenerator::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), FActorSpawnParameters());
        thumbnail_generator->generateThumbnail(mesh.second, "widgets/thumbnails/general");
    }
    
    for (auto& mesh : Vehicle_Meshes) {
        AThumbnailGenerator* thumbnail_generator = GetWorld()->SpawnActor<AThumbnailGenerator>(AThumbnailGenerator::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), FActorSpawnParameters());
        thumbnail_generator->generateThumbnail(mesh.second, "widgets/thumbnails/vehicles");
    }

    for (auto& mesh : Building_Meshes) {
        AThumbnailGenerator* thumbnail_generator = GetWorld()->SpawnActor<AThumbnailGenerator>(AThumbnailGenerator::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), FActorSpawnParameters());
        thumbnail_generator->generateThumbnail(mesh.second, "widgets/thumbnails/buildings");
    }
    
    for (auto& mesh : Character_Meshes) {
        AThumbnailGenerator* thumbnail_generator = GetWorld()->SpawnActor<AThumbnailGenerator>(AThumbnailGenerator::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), FActorSpawnParameters());
        thumbnail_generator->generateThumbnail(mesh.second, "widgets/thumbnails/characters");
    }

    for (auto& mesh : Mechanic_Meshes) {
        AThumbnailGenerator* thumbnail_generator = GetWorld()->SpawnActor<AThumbnailGenerator>(AThumbnailGenerator::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), FActorSpawnParameters());
        thumbnail_generator->generateThumbnail(mesh.second, "widgets/thumbnails/mechanics");
    }

    for (auto& mesh : Weapon_Meshes) {
        AThumbnailGenerator* thumbnail_generator = GetWorld()->SpawnActor<AThumbnailGenerator>(AThumbnailGenerator::StaticClass(), FVector(0, 0, 0), FRotator(0, 0, 0), FActorSpawnParameters());
        thumbnail_generator->generateThumbnail(mesh.second, "widgets/thumbnails/weapons");
    }
}



void UMasterMakerGameInstance::DownloadFile(const FString& URL, const FString& SavePath)
{
    // Specify the chunk size in bytes (e.g., 10MB chunks)
    const int32 ChunkSize = 10 * 1024 * 1024; // 10MB

    // Create the HTTP request
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(URL);  // URL to download
    HttpRequest->SetVerb(TEXT("GET"));
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UMasterMakerGameInstance::OnDownloadComplete, SavePath);
    HttpRequest->SetHeader(TEXT("Range"), FString::Printf(TEXT("bytes=%d-%d"), 0, ChunkSize - 1));  // Set the initial byte range for the first chunk

    HttpRequest->ProcessRequest();

}

void UMasterMakerGameInstance::OnDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString SavePath)
{
    if (bWasSuccessful)
    {
        // Get the file data
        const TArray<uint8>& ResponseData = Response->GetContent();

        // Save the data to a file
        if (FFileHelper::SaveArrayToFile(ResponseData, *SavePath))
        {
            UE_LOG(LogTemp, Log, TEXT("File downloaded and saved to %s"), *SavePath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to save file to %s"), *SavePath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to download file: %s"), *Response->GetURL());
    }
}

//void UMasterMakerGameInstance::OnDownloadComplete(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FString SavePath)
//{

//}

UUserWidget* UMasterMakerGameInstance::savePanel(IComposerInterface* Item, FVector ImpactPoint)
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));
    if (Item) {
        Saved_Item = Item;
    }
    Impact_Point = ImpactPoint;

    if (SaveGame_Panel == nullptr)
    {
        //if (GamePlay_Panel != nullptr)
        //{
        //    GamePlay_Panel->RemoveFromParent();
        //    GamePlay_Panel = nullptr;
        //}

        SaveGame_Panel = CreateWidget<UUserWidget>(this, SaveGame_Panel_Container);

        SaveGame_Panel->AddToViewport();


        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        SaveGame_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {

        if (SaveGame_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            SaveGame_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            SaveGame_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }

    }

    return SaveGame_Panel;
}

UUserWidget* UMasterMakerGameInstance::loadChatBoxPanel()
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);



    if (ChatBox_Panel == nullptr)
    {
        ChatBox_Panel = CreateWidget<UUserWidget>(this, ChatBox_Panel_Container);
        ChatBox_Panel->AddToViewport();

        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        ChatBox_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {

        if (ChatBox_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            ChatBox_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            ChatBox_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }

    }



    return ChatBox_Panel;
}


UUserWidget* UMasterMakerGameInstance::loadSandBoxPanel()
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);


    if (SandBox_Panel == nullptr)
    {
        /*     if(GamePlay_Panel != nullptr)
             {
                 GamePlay_Panel->RemoveFromParent();
                 GamePlay_Panel = nullptr;
             }*/

        SandBox_Panel = CreateWidget<UUserWidget>(this, SandBox_Panel_Container);

        SandBox_Panel->AddToViewport();


        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        SandBox_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {

        if (SandBox_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            SandBox_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            SandBox_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }

    }



    return SandBox_Panel;
}



UUserWidget* UMasterMakerGameInstance::loadCharacterPanel()
{
    //initializeThumbnails();

    if (GamePlay_Panel == nullptr)
    {
        if (SandBox_Panel != nullptr)
        {
            SandBox_Panel->RemoveFromParent();
            SandBox_Panel = nullptr;
        }

        GamePlay_Panel = CreateWidget<UUserWidget>(this, GamePlay_Panel_Container);

        if (GamePlay_Panel != nullptr)
            GamePlay_Panel->AddToViewport();

        APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        controller->bShowMouseCursor = false;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->SetInputMode(FInputModeGameOnly());

#ifdef DESTOP_BUILD
        controller->ActivateTouchInterface(0);
#endif // DES


    }
    else {

        APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

        if (!GamePlay_Panel->IsVisible())
            GamePlay_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->bShowMouseCursor = false;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        FInputModeGameAndUI InputMode;
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
        controller->SetInputMode(InputMode);
        controller->bShowMouseCursor = false; // Optionally hide the mouse cursor

    }

    return GamePlay_Panel;
}

UUserWidget* UMasterMakerGameInstance::loadMultiplayerPanel()
{
    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

    if (Multiplayer_Panel == nullptr)
    {
        /*     if(GamePlay_Panel != nullptr)
             {
                 GamePlay_Panel->RemoveFromParent();
                 GamePlay_Panel = nullptr;
             }*/

        Multiplayer_Panel = CreateWidget<UUserWidget>(this, Multiplayer_Panel_Container);

        Multiplayer_Panel->AddToViewport();


        controller->bShowMouseCursor = true;
        controller->bEnableClickEvents = true;
        controller->bEnableMouseOverEvents = true;
        controller->bEnableTouchEvents = true;

        Multiplayer_Panel->SetVisibility(ESlateVisibility::Visible);

        controller->SetInputMode(FInputModeUIOnly());
    }
    else {
        if (Multiplayer_Panel->GetVisibility() == ESlateVisibility::Visible)
        {
            Multiplayer_Panel->SetVisibility(ESlateVisibility::Collapsed);
            controller->SetInputMode(FInputModeGameOnly());
            controller->bShowMouseCursor = false;
        }
        else {
            Multiplayer_Panel->SetVisibility(ESlateVisibility::Visible);
            controller->SetInputMode(FInputModeUIOnly());

            controller->bShowMouseCursor = true;
        }
    }

    return Multiplayer_Panel;
}



ABrick* UMasterMakerGameInstance::spawnBrick(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{

    ABrick* spawn_brick;

    spawn_brick = GetWorld()->SpawnActor<ABrick>(ABrick::StaticClass(), SpawnLocation, SpawnRotation, Params);
    spawn_brick->setBrickTypeOptions(Item);



    return spawn_brick;

}


AVehicleBase* UMasterMakerGameInstance::spawnVehicle(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{
    AVehicleBase* vehicle;
    FString name;
    int index;
    Item.Name.FindChar('C', index);
    if (index >= 0) {
         name = Item.Name.Left(index + 1);
    }
    else {
        name = FString("BP_") + Item.Name + FString("_C");
    }

    vehicle = GetWorld()->SpawnActor<AVehicleBase>(Vehicle_Classes[name], SpawnLocation + FVector(0, 0, 100.0f), SpawnRotation, Params);
    vehicle->Item_Options = Item;
    vehicle->Item_Options.Mesh = Vehicle_Meshes[Item.Name];

    return vehicle;
}

AEnemyCharacter* UMasterMakerGameInstance::spawnCharacter(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{
    AEnemyCharacter* chars;

    chars = GetWorld()->SpawnActor<AEnemyCharacter>(Character_Classes[FString("BP_") + Item.Name + FString("_C")], SpawnLocation + FVector(0, 0, 100.0f), SpawnRotation, Params);
    chars->Item_Options = Item;
    return chars;
}

AWeaponBrick* UMasterMakerGameInstance::spawnWeapon(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{

    AWeaponBrick* weapon;
    weapon = GetWorld()->SpawnActor<AWeaponBrick>(AWeaponBrick::StaticClass(), SpawnLocation, SpawnRotation, Params);
    weapon->setBrickTypeOptions(Item);
    weapon->makePluginSettings();
    return weapon;
}

AMechanicBrick* UMasterMakerGameInstance::spawnMechanic(const FItemOptions& Item, const FVector& SpawnLocation, const FRotator& SpawnRotation, FActorSpawnParameters Params)
{
    AMechanicBrick* mechanic;

    mechanic = GetWorld()->SpawnActor<AMechanicBrick>(Mechanic_Classes[FString("BP_") + Item.Name + FString("_C")], SpawnLocation, SpawnRotation, Params);
    mechanic->setGhostComponent(Mechanic_Meshes[Item.Name]);
    mechanic->setMaterialColor(Item.Color);
    mechanic->Item_Options = Item;

    return mechanic;
}


float UMasterMakerGameInstance::findSuitableHeight(AActor* Actor)
{
    if (Actor) {
        FVector VehicleLocation = Actor->GetActorLocation();

        // Define the line trace start and end points
        FVector LineTraceStart = VehicleLocation;
        FVector LineTraceEnd = VehicleLocation + FVector(0.0f, 0.0f, -10000.0f); // Trace 10,000 units down
        FHitResult HitResult;
        float TerrainHeight = 0;
        bool bHit = GetWorld()->LineTraceSingleByChannel(
            HitResult,
            LineTraceStart,
            LineTraceEnd,
            ECC_Visibility // Trace against the visibility channel
        );

        if (bHit)
        {
            TerrainHeight = HitResult.Location.Z;
            return TerrainHeight + 40;

        }
        else {
            return 0;
        }


    }

    return 0;
}

AActor* UMasterMakerGameInstance::spawnItem(const FVector& SpawnLocation, const FRotator& SpawnRotation, const FItemOptions& Item, const FVector& ImpactPoint, FActorSpawnParameters Params)
{
    FString item_name;
    AWeaponBrick* weapon;
    AVehicleBase* vehicle;
    AMechanicBrick* machine;
    ABrick* brick;
    AEnemyCharacter* character;

    //weapon
    if (Item.CategoryName == Mesh_Categories.at(weapons)) {
       
        weapon = spawnWeapon(Item, SpawnLocation, SpawnRotation, Params);
        return weapon;
    }
    else if (Item.CategoryName == Mesh_Categories.at(vehicles) && Item.SubCategoryName == Mesh_Sub_Categories.at(vehicle_skeleton)) {
        vehicle = spawnVehicle(Item, ImpactPoint, SpawnRotation, Params);
        return vehicle;
    }
    else if (Item.CategoryName == Mesh_Categories.at(characters)) {
        character = spawnCharacter(Item, ImpactPoint, SpawnRotation, Params);
        return character;
    }
    else if (Item.CategoryName == Mesh_Categories.at(mechanics)) {
        machine = spawnMechanic(Item, SpawnLocation + FVector(100, 0, 0), FRotator(0, 0, 0), Params);
        return machine;
    }
    else {
        brick = spawnBrick(Item, SpawnLocation, SpawnRotation, Params);
        return brick;
    }

    return nullptr;
}



void UMasterMakerGameInstance::selectCurrentProductItem(const FItemOptions& Item)
{
    Selected_Item = Item;

    if (Item.CategoryName == Mesh_Categories.at(general)) {
        Selected_Item.Mesh = General_Brick_Meshes[Item.Name];
    }else if (Item.CategoryName == Mesh_Categories.at(vehicles)) {
        Selected_Item.Mesh = Vehicle_Meshes[Item.Name];
        if (Item.Name.Find(VEHICLE_APPENDIX) >= 0) {
            Selected_Item.SubCategoryName = Mesh_Sub_Categories.at(vehicle_skeleton);
        }
        else {
            Selected_Item.SubCategoryName = Mesh_Sub_Categories.at(vehicle_piece);
        }
    }else if (Item.CategoryName == Mesh_Categories.at(buildings)) {
        Selected_Item.Mesh = Building_Meshes[Item.Name];
    }else if (Item.CategoryName == Mesh_Categories.at(mechanics)) {
        Selected_Item.Mesh = Mechanic_Meshes[Item.Name];
    }else if (Item.CategoryName == Mesh_Categories.at(characters)) {
        Selected_Item.Mesh = Character_Meshes[Item.Name];
    }else if (Item.CategoryName == Mesh_Categories.at(weapons)) {
        Selected_Item.Mesh = Weapon_Meshes[Item.Name];

    }

}

void UMasterMakerGameInstance::selectCurrentProductColor(FLinearColor Color)
{
    Selected_Item.Color = Color;
}

bool UMasterMakerGameInstance::saveGame(FString Name)
{
    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));

    if (Saved_Item)
    {

        FConstructionInfo info = Saved_Item->compileConstructInfo(Saved_Item);

        TSharedPtr<FJsonObject> json = save_game->convertConstructionInfoToJson(info);

        save_game->SaveObject = save_game->serializeJsonObject(json);

        FString path = FPaths::ProjectContentDir() + FString("/json_data/") + Name + FString(".txt");
        FFileHelper::SaveStringToFile(save_game->SaveObject, *path);

        UGameplayStatics::SaveGameToSlot(save_game, Name, 0);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("saved"));
        return true;

    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("save failed"));

        return false;
    }

}

bool UMasterMakerGameInstance::loadGame(FString Name)
{

    APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    AUserCharacter* cha = Cast<AUserCharacter>(controller->GetPawn());
    if (cha) {
        Cast<AUserPlayerState>(cha->GetPlayerState())->load(Name, Impact_Point);
    }

    //    construction_info = save_game->Construction_Info;
   /* UMasterMakerGameInstance* game_instance = Cast<UMasterMakerGameInstance>(GetWorld()->GetGameInstance());

    UMasterMakerSaveGame* save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::CreateSaveGameObject(UMasterMakerSaveGame::StaticClass()));
    save_game = Cast<UMasterMakerSaveGame>(UGameplayStatics::LoadGameFromSlot(Name, 0));
    if (save_game) {
        TSharedPtr<FJsonObject> de_json = save_game->deserializeJsonObject(save_game->SaveObject);
        FConstructionInfo de_info = save_game->convertJsonToConstructionInfo(de_json);

        IComposerInterface::buildFromConstructionInfo(de_info, Impact_Point, game_instance);

        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("loaded"));
    }
    else {
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, Name);
        GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, "save game non");

    }*/


    return false;
}

void UMasterMakerGameInstance::setScreenMessage(const FString& Message, float TimeOut)
{
    if (Cast<UGamePlayWidget>(GamePlay_Panel)) {
        Cast<UGamePlayWidget>(GamePlay_Panel)->setMessage(Message, TimeOut);
    }

}

void UMasterMakerGameInstance::hideScreenMessage()
{

}


AActor* UMasterMakerGameInstance::findActorByName(const FString& ActorName)
{
    // Iterate through all actors in the level and find the one with the specified name
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), FoundActors);

    for (int i = 0; i < FoundActors.Num(); i++)
    {
        AActor* Actor = FoundActors[i];
        if (Actor->GetName() == ActorName) {
            UE_LOG(LogTemp, Warning, TEXT("Found"));
            return Actor;
        }
    }

    // Actor with the specified name was not found
    return nullptr;
}