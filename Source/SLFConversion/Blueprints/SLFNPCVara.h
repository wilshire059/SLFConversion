// SLFNPCVara.h
// Vara — Wandering guide and merchant NPC.
// Main quest giver who knows fragments of truth about the Convergence.
// Appears in the Hub and moves between zones as the player progresses.

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeNPC.h"
#include "SLFGameTypes.h"
#include "SLFNPCVara.generated.h"

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFNPCVara : public ASLFSoulslikeNPC
{
	GENERATED_BODY()

public:
	ASLFNPCVara();

protected:
	virtual void BeginPlay() override;

public:
	// ── NPC Config ──

	/** Vara's NPC configuration */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vara")
	FSLFNPCConfig NPCConfig;

	/** Default dialog asset */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vara|Dialog")
	TSoftObjectPtr<UPrimaryDataAsset> DefaultDialogAsset;

	/** Vendor data (Vara sells key items) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vara|Dialog")
	TSoftObjectPtr<UPrimaryDataAsset> VendorDataAsset;

	/** Default mesh for Vara */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Vara|Mesh")
	TSoftObjectPtr<USkeletalMesh> VaraMesh;

	// ── Wandering ──

	/** Current zone Vara is in (changes based on player progress) */
	UPROPERTY(BlueprintReadWrite, Category = "Vara|State")
	FGameplayTag CurrentLocationZone;

	/** Update Vara's location based on player quest progress */
	UFUNCTION(BlueprintCallable, Category = "Vara")
	void UpdateLocationFromProgress();

	// ── Quest Dialog ──

	/** Get dialog entries based on current game progress */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Vara")
	void GetContextualDialog(TArray<FSLFDialogEntry>& OutEntries);
	virtual void GetContextualDialog_Implementation(TArray<FSLFDialogEntry>& OutEntries);

	// ── Interface Overrides ──
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	virtual void OnTraced_Implementation(AActor* TracedBy) override;
};
