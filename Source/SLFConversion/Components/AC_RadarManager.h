// AC_RadarManager.h
// C++ component for AC_RadarManager
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Component/AC_RadarManager.json
// Variables: 13 | Functions: 10 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "Camera/CameraComponent.h"
#include "AC_RadarManager.generated.h"

// Forward declarations
class UAC_RadarElement;
class UAnimMontage;
class UDataTable;
class UPrimaryDataAsset;
class UW_Radar;
class UW_Radar_TrackedElement;

// Event Dispatcher Declarations


UCLASS(ClassGroup=(SoulslikeFramework), meta=(BlueprintSpawnableComponent))
class SLFCONVERSION_API UAC_RadarManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_RadarManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (13)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool Visible;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	TMap<FGameplayTag, FGameplayTag> CardinalData;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	bool ShouldUpdate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	FTimerHandle Timer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UW_Radar* RadarWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double RadarClampLength;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	UCameraComponent* PlayerCamera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config")
	double RefreshInterval;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Icon")
	TSoftObjectPtr<UTexture2D> PlayerIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Icon")
	FLinearColor IconTint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Config|Icon")
	FVector2D IconSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Runtime")
	TArray<UAC_RadarElement*> TrackedComponents;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	APawn* Pawn;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (10)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void SetupPlayerIcon();
	virtual void SetupPlayerIcon_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void SetupCardinals();
	virtual void SetupCardinals_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void GetDelta(double A, double B, bool IsClockwise, double& OutReturnValue, double& OutReturnValue_1, double& OutReturnValue_2);
	virtual void GetDelta_Implementation(double A, double B, bool IsClockwise, double& OutReturnValue, double& OutReturnValue_1, double& OutReturnValue_2);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	double ConvertToCircleDegrees(double In);
	virtual double ConvertToCircleDegrees_Implementation(double In);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void ConvertToTranslation(const FRotator& A, const FRotator& B, FVector2D& OutTranslation, double& OutTranslation_X, double& OutTranslation_Y, bool& OutIsClockwise, bool& OutIsInRadarLength, FVector2D& OutTranslation_3, double& OutTranslation_X_1, double& OutTranslation_Y_1, bool& OutIsClockwise_1, bool& OutIsInRadarLength_1);
	virtual void ConvertToTranslation_Implementation(const FRotator& A, const FRotator& B, FVector2D& OutTranslation, double& OutTranslation_X, double& OutTranslation_Y, bool& OutIsClockwise, bool& OutIsInRadarLength, FVector2D& OutTranslation_3, double& OutTranslation_X_1, double& OutTranslation_Y_1, bool& OutIsClockwise_1, bool& OutIsInRadarLength_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void ToggleUpdateState(bool InShouldUpdate);
	virtual void ToggleUpdateState_Implementation(bool InShouldUpdate);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void RefreshCardinals();
	virtual void RefreshCardinals_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void StartTrackElement(UAC_RadarElement* TrackedElement, UW_Radar_TrackedElement*& OutTrackedElementWidget, UW_Radar_TrackedElement*& OutTrackedElementWidget_1);
	virtual void StartTrackElement_Implementation(UAC_RadarElement* TrackedElement, UW_Radar_TrackedElement*& OutTrackedElementWidget, UW_Radar_TrackedElement*& OutTrackedElementWidget_1);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void RefreshTrackedElements();
	virtual void RefreshTrackedElements_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "AC_RadarManager")
	void StopTrackElement(UAC_RadarElement* Element);
	virtual void StopTrackElement_Implementation(UAC_RadarElement* Element);

	// Internal helpers
	void SetupUpdateTimer();
	void UpdateVisuals();
};
