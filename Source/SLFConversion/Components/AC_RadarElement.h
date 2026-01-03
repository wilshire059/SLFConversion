// AC_RadarElement.h
// C++ component for AC_RadarElement
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_RadarElement.json
// Variables: 4 | Functions: 0 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"

#include "AC_RadarElement.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UW_Radar_TrackedElement;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_RadarElement : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_RadarElement();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Helper functions
	void RegisterWithRadarManager();
	void UnregisterFromRadarManager();


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (4)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Icon")
	TSoftObjectPtr<UTexture2D> Icon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Icon")
	FLinearColor IconTint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Icon")
	FVector2D IconSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UW_Radar_TrackedElement* MarkerWidget;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (0)
	// ═══════════════════════════════════════════════════════════════════════

};
