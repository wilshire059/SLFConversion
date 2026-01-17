// SLFBossDoor.h
// C++ base for B_BossDoor - Boss arena fog gate
//
// CRITICAL: This must match the original B_BossDoor Blueprint structure:
// - Interactable SM (Static Mesh) with scale 3.25, 0.1, 4.25 at Z=200, Yaw=90
// - Portal_Front and Portal_Back Niagara components attached to Interactable SM
// - Implements ISLFInteractableInterface for interaction detection

#pragma once

#include "CoreMinimal.h"
#include "SLFDoorBase.h"
#include "NiagaraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"
#include "Interfaces/SLFInteractableInterface.h"
#include "Animation/AnimMontage.h"
#include "SLFBossDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDoorSealed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDoorUnlocked);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBossDoor : public ASLFDoorBase, public ISLFInteractableInterface
{
	GENERATED_BODY()

public:
	ASLFBossDoor();

	// ============================================================
	// COMPONENTS - Match original B_BossDoor Blueprint structure
	// ============================================================

	/** Fog gate mesh - the collision plane that blocks players
	 * Scale: 3.25, 0.1, 4.25 | Location: Z=200 | Rotation: Yaw=90
	 * Mesh: /Engine/BasicShapes/Cube.Cube */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* FogGateMesh;

	/** Front portal fog effect - attached to FogGateMesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* PortalEffectFront;

	/** Back portal fog effect - attached to FogGateMesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* PortalEffectBack;

	/** Death currency spawn point when boss is defeated */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* CurrencySpawnLocation;

	/** MoveTo - Target location where player moves to after passing through fog gate
	 * Positioned on the OTHER side of the gate from player's approach */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* MoveTo;

	/** Door Frame - Decorative arch/border around the fog gate
	 * Mesh: SM_PrisonDoorArch - provides visual framing for the fog gate
	 * Inherited from B_Door Blueprint - must exist in C++ for boss doors */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DoorFrame;

	// ============================================================
	// VARIABLES - From original B_BossDoor Blueprint
	// ============================================================

	/** Animation montage to play when player approaches from the right/front */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door|Interaction")
	TSoftObjectPtr<UAnimMontage> InteractMontage_RH;

	/** Animation montage to play when player approaches from the left/back */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door|Interaction")
	TSoftObjectPtr<UAnimMontage> InteractMontage_LH;

	/** Distance from door origin to MoveTo point (how far player moves through)
	 * bp_only default is 100 units - enough to clear the fog gate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door|Interaction")
	double MoveToDistance = 100.0;

	/** Cache: Which montage was selected based on player approach direction */
	UPROPERTY(BlueprintReadOnly, Category = "Boss Door|Runtime")
	UAnimMontage* SelectedMontage = nullptr;

	/** Cache: Whether player is approaching from the front/forwards */
	UPROPERTY(BlueprintReadOnly, Category = "Boss Door|Runtime")
	bool bOpenForwards = true;

	/** Whether this door can be traced/interacted with */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bCanBeTraced = true;

	/** Text shown when player can interact with the fog gate */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	FText InteractionText;

	/** Whether the boss fight has started (fog gate active) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bIsActivated = false;

	/** Whether the fog wall effect is active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bIsFogWall = true;

	/** Whether the door is sealed (locked during boss fight) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bSealed = false;

	/** Boss class for this arena */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	TSubclassOf<AActor> BossClass;

	/** Tag identifying the boss arena */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	FName BossArenaTag;

	/** Whether to destroy fog wall when boss is defeated */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bDestroyOnBossDefeated = true;

	/** Niagara system asset for portal effects */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door|VFX")
	TSoftObjectPtr<UNiagaraSystem> PortalNiagaraSystem;

	/** Scale of the fog gate mesh - adjust to fit doorway
	 * Default: (3.25, 0.1, 4.25) = 325x10x425 units
	 * Adjust per-instance if doorway is larger/smaller */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door|Mesh")
	FVector FogGateScale = FVector(3.25f, 0.1f, 4.25f);

	/** Z offset of the fog gate mesh from actor origin
	 * Default: 200 units (center at half height) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door|Mesh")
	float FogGateZOffset = 200.0f;

	// ============================================================
	// EVENT DISPATCHERS
	// ============================================================

	UPROPERTY(BlueprintAssignable, Category = "Boss Door|Events")
	FOnBossDoorSealed OnBossDoorSealed;

	UPROPERTY(BlueprintAssignable, Category = "Boss Door|Events")
	FOnBossDoorUnlocked OnBossDoorUnlocked;

	// ============================================================
	// FUNCTIONS
	// ============================================================

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void SealDoor();
	virtual void SealDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void UnsealDoor();
	virtual void UnsealDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void OnBossDefeated();
	virtual void OnBossDefeated_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void OnPlayerEnterArena(AActor* Player);
	virtual void OnPlayerEnterArena_Implementation(AActor* Player);

	/** Unlock the boss door (called when boss is defeated) */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void UnlockBossDoor();
	virtual void UnlockBossDoor_Implementation();

	/** Get the death currency spawn location */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	USceneComponent* GetDeathCurrencySpawnPoint();
	virtual USceneComponent* GetDeathCurrencySpawnPoint_Implementation();

	/** Initialize door from save data */
	UFUNCTION(BlueprintCallable, Category = "Boss Door")
	void InitializeLoadedStates(bool bInCanBeTraced, bool bInIsActivated);

	// ============================================================
	// ISLFInteractableInterface Implementation
	// ============================================================

	virtual void OnTraced_Implementation(AActor* TracedBy) override;
	virtual void OnInteract_Implementation(AActor* InteractingActor) override;
	virtual void OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData) override;
	virtual FSLFItemInfo TryGetItemInfo_Implementation() override;

protected:
	virtual void BeginPlay() override;

	/** Set collision on the fog gate mesh */
	void SetFogGateCollision(bool bEnableCollision);
};
