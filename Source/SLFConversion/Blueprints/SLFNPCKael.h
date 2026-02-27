// SLFNPCKael.h
// Kael — Blacksmith and weapon upgrader NPC.
// Found in the Hub safe zone. Upgrades weapons using rare materials from each zone.

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeNPC.h"
#include "SLFGameTypes.h"
#include "SLFNPCKael.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnWeaponUpgraded, UDataAsset*, Weapon, int32, NewLevel);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFNPCKael : public ASLFSoulslikeNPC
{
	GENERATED_BODY()

public:
	ASLFNPCKael();

protected:
	virtual void BeginPlay() override;

public:
	// ── NPC Config ──

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Kael")
	FSLFNPCConfig NPCConfig;

	/** Default dialog asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Kael|Dialog")
	TSoftObjectPtr<UPrimaryDataAsset> DefaultDialogAsset;

	/** Default mesh */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Kael|Mesh")
	TSoftObjectPtr<USkeletalMesh> KaelMesh;

	// ── Weapon Upgrade System ──

	/** Maximum weapon upgrade level */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Kael|Upgrade")
	int32 MaxUpgradeLevel = 10;

	/** Base currency cost per upgrade level (multiplied by level) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Kael|Upgrade")
	int32 BaseUpgradeCost = 500;

	/** Calculate upgrade cost for a specific weapon level */
	UFUNCTION(BlueprintPure, Category = "Kael|Upgrade")
	int32 GetUpgradeCost(int32 CurrentLevel) const;

	/** Check if player has required materials for upgrade */
	UFUNCTION(BlueprintCallable, Category = "Kael|Upgrade")
	bool CanUpgradeWeapon(AActor* Player, UDataAsset* Weapon, int32 CurrentLevel) const;

	// ── Events ──

	UPROPERTY(BlueprintAssignable, Category = "Kael|Events")
	FOnWeaponUpgraded OnWeaponUpgraded;

	// ── Interface Overrides ──
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	virtual void OnTraced_Implementation(AActor* TracedBy) override;
};
