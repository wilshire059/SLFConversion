// SetupGameAnimCommandlet.h
// Migrates animations from GameAnimationSample project and creates montages for:
// - Resting point sit-down/stand-up
// - Slide (start/loop/exit)
// - Traversal (vault/mantle/hurdle/climb)
// - Landing reactions (light/heavy/roll/stumble)
// - Dodge avoidance

#pragma once

#include "CoreMinimal.h"
#include "Commandlets/Commandlet.h"
#include "SetupGameAnimCommandlet.generated.h"

UCLASS()
class USetupGameAnimCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	virtual int32 Main(const FString& Params) override;

private:
	/** Source project content path */
	FString SourceContentDir;

	/** Our project content path */
	FString DestContentDir;

	/** Copy a .uasset from source project to destination */
	bool CopyAnimAsset(const FString& RelativePath, const FString& DestFolder);

	/** Create an AnimMontage from an AnimSequence at given game path */
	UAnimMontage* CreateMontageFromSequence(const FString& SequencePath, const FString& MontageName, const FString& SaveFolder);

	/** Set up resting point animations (bench sit/stand) */
	void SetupRestingPointAnims();

	/** Set up slide animations */
	void SetupSlideAnims();

	/** Set up traversal animations (vault/mantle/hurdle/climb) */
	void SetupTraversalAnims();

	/** Set up landing reaction animations */
	void SetupLandingAnims();

	/** Set up dodge/avoidance animations */
	void SetupAvoidanceAnims();

	/** Create all montages (called with -montages flag after files are copied) */
	void CreateMontageBatch();
};
