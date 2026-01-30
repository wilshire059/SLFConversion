// B_SequenceActor.h
// C++ class for Blueprint B_SequenceActor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_SequenceActor.json
// Parent: Actor -> AActor

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Components/BillboardComponent.h"
#include "B_SequenceActor.generated.h"

// Forward declarations
class UInputAction;
class UInputMappingContext;

// Event Dispatchers
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FB_SequenceActor_OnSequenceFinished);

/**
 * Sequence Actor - Plays a level sequence (cinematic) when spawned
 * Supports skipping via input
 */
UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_SequenceActor : public AActor
{
	GENERATED_BODY()

public:
	AB_SequenceActor();

	// ═══════════════════════════════════════════════════════════════════════
	// COMPONENTS
	// ═══════════════════════════════════════════════════════════════════════

	// COMPONENT OWNERSHIP: Blueprint SCS owns all components.
	// C++ only caches references at runtime. See CLAUDE.md for pattern.
	// Named "CachedBillboard" to avoid conflict with Blueprint SCS component "Billboard"
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Components")
	UBillboardComponent* CachedBillboard;

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES
	// ═══════════════════════════════════════════════════════════════════════

	// The level sequence to play
	// ExposeOnSpawn allows setting this from SpawnActor nodes in Level Blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (ExposeOnSpawn = "true", DisplayName = "Sequence to Play"))
	ULevelSequence* SequenceToPlay;

	// Playback settings
	// ExposeOnSpawn allows setting this from SpawnActor nodes in Level Blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence", meta = (ExposeOnSpawn = "true"))
	FMovieSceneSequencePlaybackSettings Settings;

	// The active sequence player (created at runtime)
	UPROPERTY(BlueprintReadWrite, Category = "Sequence")
	ULevelSequencePlayer* ActiveSequencePlayer;

	// Whether this sequence can be skipped by player input
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool CanBeSkipped;

	// Input action for showing skip prompt (any key press)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_AnyKey;

	// Input action for skipping (game menu)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* IA_GameMenu;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FB_SequenceActor_OnSequenceFinished OnSequenceFinished;

	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS
	// ═══════════════════════════════════════════════════════════════════════

	// Called when the sequence finishes naturally
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void EventFinished();

	// Called to destroy this actor and cleanup
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void EventDestroy();

	// Skip the sequence
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void SkipSequence();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Setup input bindings
	void SetupInputBindings();

	// Sequence finished callback
	UFUNCTION()
	void OnSequenceFinishedCallback();

private:
	// Whether skip prompt is shown
	bool bSkipPromptVisible;

	// Handle AnyKey input
	void OnAnyKeyInput();

	// Handle GameMenu input
	void OnGameMenuInput();
};
