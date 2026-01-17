// WeaponDebugLibrary.cpp
// Debug functions for SLFConversion weapon transform comparison

#include "WeaponDebugLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"

void UWeaponDebugLibrary::LogAllWeaponsInPIE()
{
    // Auto-save to JSON
    SaveWeaponDataToJSON(TEXT("C:/scripts/slfconversion/migration_cache/weapon_attachment_slfconversion.json"));
}

void UWeaponDebugLibrary::SaveWeaponDataToJSON(const FString& OutputPath)
{
    UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG] SaveWeaponDataToJSON"));

    UWorld* PIEWorld = nullptr;
    if (GEngine)
    {
        for (const FWorldContext& Context : GEngine->GetWorldContexts())
        {
            if (Context.WorldType == EWorldType::PIE)
            {
                PIEWorld = Context.World();
                UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG] Found PIE World: %s"), *PIEWorld->GetName());
                break;
            }
        }
    }

    if (!PIEWorld)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SLFConversion DEBUG] No PIE world found!"));
        return;
    }

    TSharedPtr<FJsonObject> RootObject = MakeShareable(new FJsonObject);
    RootObject->SetStringField(TEXT("project"), TEXT("SLFConversion"));
    TSharedPtr<FJsonObject> WeaponsObject = MakeShareable(new FJsonObject);

    int32 WeaponCount = 0;
    for (TActorIterator<AActor> It(PIEWorld); It; ++It)
    {
        AActor* Actor = *It;
        FString ClassName = Actor->GetClass()->GetName();

        if (ClassName.Contains(TEXT("B_Item_Weapon")))
        {
            WeaponCount++;
            TSharedPtr<FJsonObject> WeaponObj = MakeShareable(new FJsonObject);
            WeaponObj->SetStringField(TEXT("class"), ClassName);

            UStaticMeshComponent* WeaponMesh = Actor->FindComponentByClass<UStaticMeshComponent>();
            USceneComponent* Root = Actor->GetRootComponent();

            // Log to output
            UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG] === %s ==="), *ClassName);

            if (WeaponMesh)
            {
                FVector MeshRelLoc = WeaponMesh->GetRelativeLocation();
                FVector MeshWorldLoc = WeaponMesh->GetComponentLocation();

                UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Mesh Relative: %s"), *MeshRelLoc.ToString());
                UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Mesh World: %s"), *MeshWorldLoc.ToString());

                TSharedPtr<FJsonObject> RelLocObj = MakeShareable(new FJsonObject);
                RelLocObj->SetNumberField(TEXT("x"), MeshRelLoc.X);
                RelLocObj->SetNumberField(TEXT("y"), MeshRelLoc.Y);
                RelLocObj->SetNumberField(TEXT("z"), MeshRelLoc.Z);
                WeaponObj->SetObjectField(TEXT("mesh_relative_location"), RelLocObj);

                if (Root && Root->GetAttachParent())
                {
                    USkeletalMeshComponent* CharMesh = Cast<USkeletalMeshComponent>(Root->GetAttachParent());
                    if (CharMesh)
                    {
                        FName SocketName = Root->GetAttachSocketName();
                        FVector SocketLoc = CharMesh->GetSocketLocation(SocketName);
                        FVector Offset = MeshWorldLoc - SocketLoc;
                        float Magnitude = Offset.Size();

                        UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Socket: %s"), *SocketName.ToString());
                        UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Socket World: %s"), *SocketLoc.ToString());
                        UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Offset: %s"), *Offset.ToString());
                        UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Offset Magnitude: %.4f"), Magnitude);

                        WeaponObj->SetStringField(TEXT("attach_socket"), SocketName.ToString());

                        TSharedPtr<FJsonObject> OffsetObj = MakeShareable(new FJsonObject);
                        OffsetObj->SetNumberField(TEXT("x"), Offset.X);
                        OffsetObj->SetNumberField(TEXT("y"), Offset.Y);
                        OffsetObj->SetNumberField(TEXT("z"), Offset.Z);
                        WeaponObj->SetObjectField(TEXT("mesh_offset_from_socket"), OffsetObj);
                        WeaponObj->SetNumberField(TEXT("mesh_offset_magnitude"), Magnitude);
                    }
                }

                if (UStaticMesh* Mesh = WeaponMesh->GetStaticMesh())
                {
                    WeaponObj->SetStringField(TEXT("static_mesh"), Mesh->GetName());
                    UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG]   Static Mesh: %s"), *Mesh->GetName());
                }
            }
            WeaponsObject->SetObjectField(ClassName, WeaponObj);
        }
    }

    RootObject->SetObjectField(TEXT("weapons"), WeaponsObject);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(RootObject.ToSharedRef(), Writer);

    if (FFileHelper::SaveStringToFile(OutputString, *OutputPath))
    {
        UE_LOG(LogTemp, Log, TEXT("[SLFConversion DEBUG] Saved %d weapons to: %s"), WeaponCount, *OutputPath);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[SLFConversion DEBUG] Failed to save to: %s"), *OutputPath);
    }
}
