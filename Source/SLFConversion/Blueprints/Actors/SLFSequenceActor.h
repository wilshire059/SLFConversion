// SLFSequenceActor.h
// C++ base for B_SequenceActor - Cinematic/sequence trigger
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFSequenceActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSequenceStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSequenceFinished);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSequenceActor : public AActor
{
	GENERATED_BODY()

public:
	ASLFSequenceActor();

	// ============================================================
	// MIGRATION SUMMARY: B_SequenceActor
	// Variables: 6 | Functions: 4 | Dispatchers: 2
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* TriggerVolume;

	// Sequence Settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	class ULevelSequence* LevelSequence;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bPlayOnEnter = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bPlayOnce = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bHasPlayed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bDisablePlayerInput = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sequence")
	bool bSkippable = true;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Sequence|Events")
	FOnSequenceStarted OnSequenceStarted;

	UPROPERTY(BlueprintAssignable, Category = "Sequence|Events")
	FOnSequenceFinished OnSequenceFinished;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sequence")
	void PlaySequence();
	virtual void PlaySequence_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sequence")
	void StopSequence();
	virtual void StopSequence_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sequence")
	void SkipSequence();
	virtual void SkipSequence_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sequence")
	void OnPlayerEnter(AActor* Player);
	virtual void OnPlayerEnter_Implementation(AActor* Player);

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	class ALevelSequenceActor* SequenceActor;

	bool bIsPlaying = false;
};
