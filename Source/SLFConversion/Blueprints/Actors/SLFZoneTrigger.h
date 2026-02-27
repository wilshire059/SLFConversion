// SLFZoneTrigger.h
// Volume placed at zone boundaries. When player enters, updates the ZoneManager
// with the new zone and optionally triggers loading screen / ambient changes.
// Extends ASLFLevelStreamManager for zones that need level streaming.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFZoneTrigger.generated.h"

class UBoxComponent;

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFZoneTrigger : public AActor
{
	GENERATED_BODY()

public:
	ASLFZoneTrigger();

protected:
	virtual void BeginPlay() override;

public:
	/** Zone tag this trigger activates */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	FGameplayTag ZoneTag;

	/** Display name shown during zone transition (e.g., "Entering The Ashfields") */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	FText TransitionText;

	/** Whether to show a zone name popup when entering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	bool bShowZoneNameOnEntry = true;

	/** Whether this zone transition requires a loading screen */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	bool bRequiresLoadingScreen = false;

	/** Level to stream in (if this zone has its own level) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Streaming")
	FString StreamLevelName;

	/** Level to stream out when entering this zone */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone|Streaming")
	FString UnloadLevelName;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerVolume;

	UFUNCTION()
	void OnPlayerEnterZone(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
