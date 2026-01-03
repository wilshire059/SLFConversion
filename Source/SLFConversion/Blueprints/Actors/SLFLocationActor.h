// SLFLocationActor.h
// C++ base for B_LocationActor - Named location for spawn points, teleports, etc.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFLocationActor.generated.h"

UENUM(BlueprintType)
enum class ESLFLocationType : uint8
{
	SpawnPoint,
	Checkpoint,
	Bonfire,
	Teleporter,
	BossArena,
	SafeZone
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFLocationActor : public AActor
{
	GENERATED_BODY()

public:
	ASLFLocationActor();

	// ============================================================
	// MIGRATION SUMMARY: B_LocationActor
	// Variables: 6 | Functions: 3 | Dispatchers: 0
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBillboardComponent* LocationIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UArrowComponent* SpawnDirection;

	// Location Info
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FText LocationName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FGameplayTag LocationTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	ESLFLocationType LocationType = ESLFLocationType::SpawnPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	bool bIsUnlocked = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	bool bIsDefaultSpawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Location")
	FName LinkedLevelName;

	// Functions
	UFUNCTION(BlueprintPure, Category = "Location")
	FTransform GetSpawnTransform() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Location")
	void UnlockLocation();
	virtual void UnlockLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Location")
	void TeleportActorHere(AActor* ActorToTeleport);
	virtual void TeleportActorHere_Implementation(AActor* ActorToTeleport);

protected:
	virtual void BeginPlay() override;
};
