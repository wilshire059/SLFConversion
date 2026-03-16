// SLFEnemyGeneric.h
// Data-driven enemy class that configures itself from an enemy name parameter.
// Loads mesh, AnimBP, montages, and data assets from /Game/CustomEnemies/<PascalName>/.
// Used for all Ashborne batch enemies (Withered Wanderer, Bloated Spitter, etc.)

#pragma once

#include "CoreMinimal.h"
#include "SLFSoulslikeEnemy.h"
#include "SLFEnemyGeneric.generated.h"

class UNavigationInvokerComponent;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFEnemyGeneric : public ASLFSoulslikeEnemy
{
	GENERATED_BODY()

public:
	ASLFEnemyGeneric();

	virtual void Tick(float DeltaTime) override;

	// The PascalCase enemy name (e.g., "WitheredWanderer")
	// Set this in the Blueprint CDO or per-instance to configure which enemy assets to load.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config")
	FString EnemyTypeName;

	// Mesh scale (Meshy AI meshes are meter-scale, need ~139x to match UE5 mannequin)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config")
	// Interchange importer (UE5.7) auto-converts meters→cm, so scale 1.0 = correct size
	// (Sentinel used 139x because it was imported with old FBX importer, no auto-conversion)
	float MeshScale = 1.0f;

	// Mesh Z offset (to align feet with capsule bottom)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Config")
	float MeshZOffset = -90.0f;

	// Call after setting EnemyTypeName at runtime to load mesh/anims/materials
	void ApplyEnemyConfig();

protected:
	virtual void BeginPlay() override;

private:
	void ConfigureAbilities();

	UPROPERTY()
	TSubclassOf<UAnimInstance> EnemyAnimBPClass;

	UPROPERTY()
	UNavigationInvokerComponent* NavInvoker = nullptr;

	// Montage-based locomotion (Bug #16: BlendSpacePlayer collapses bones)
	UPROPERTY()
	UAnimMontage* IdleMontage = nullptr;

	UPROPERTY()
	UAnimMontage* WalkMontage = nullptr;

	UPROPERTY()
	UAnimMontage* RunMontage = nullptr;

	UAnimMontage* ActiveLocomotionMontage = nullptr;

	// Full-body particle aura — deferred spawn (mesh must be fully initialized first)
	UPROPERTY()
	TArray<UNiagaraComponent*> BodyVFXComponents;
	bool bBodyVFXSpawned = false;
	void TrySpawnBodyVFX();

	bool bAnimBPApplied = false;
	int32 TicksAfterBeginPlay = 0;
};
