// SLFLevelStreamManager.h
// Actor placed at dungeon entrances/exits to handle level streaming transitions.
// On overlap: loads the target dungeon level, fades screen, teleports player.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFLevelStreamManager.generated.h"

class UBoxComponent;
class ULevelStreamingDynamic;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFLevelStreamManager : public AActor
{
	GENERATED_BODY()

public:
	ASLFLevelStreamManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	// ── Configuration ──

	/** Level name to stream in (e.g., "/Game/Maps/L_Dungeon_01") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	FString DungeonLevelName;

	/** Display name shown to player (e.g., "Forgotten Catacombs") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	FString DungeonDisplayName;

	/** Offset applied to the streamed level (dungeons go underground) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	FVector DungeonWorldOffset = FVector(0, 0, -5000.0f);

	/** Location inside the dungeon where the player spawns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	FVector DungeonSpawnOffset = FVector(0, 0, 100.0f);

	/** Duration of screen fade transition (seconds) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	float FadeDuration = 1.0f;

	/** Whether this is a dungeon exit (teleports back to overworld) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	bool bIsDungeonExit = false;

	/** The overworld return location (set on entrance actors, read by exit actors) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	FVector OverworldReturnLocation = FVector::ZeroVector;

	// ── Runtime State ──

	UPROPERTY(BlueprintReadOnly, Category = "Level Streaming")
	bool bDungeonLoaded = false;

	UPROPERTY(BlueprintReadOnly, Category = "Level Streaming")
	bool bTransitionInProgress = false;

	// ── Functions ──

	/** Initiate transition into dungeon */
	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
	void EnterDungeon();

	/** Initiate transition back to overworld */
	UFUNCTION(BlueprintCallable, Category = "Level Streaming")
	void ExitDungeon();

	/** Called when the dungeon level finishes loading */
	UFUNCTION()
	void OnDungeonLevelLoaded();

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBoxComponent* TriggerVolume;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void ExecuteTransition(FVector TargetLocation);
	void OnFadeOutComplete();
	void OnFadeInComplete();

	UPROPERTY()
	ULevelStreamingDynamic* StreamedLevel = nullptr;

	FVector PendingTeleportLocation;
	FTimerHandle FadeTimerHandle;

	UPROPERTY()
	ACharacter* CachedPlayerChar = nullptr;
};
