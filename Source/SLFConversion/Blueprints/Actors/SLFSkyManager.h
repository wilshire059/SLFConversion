// SLFSkyManager.h
// C++ base for B_SkyManager - Day/night cycle and weather controller
//
// COMPONENT PATTERN: CreateDefaultSubobject (like B_Container)
// Components are created in constructor with SAME NAMES as Blueprint SCS.
// When Blueprint is reparented, SCS properties OVERRIDE C++ defaults.
// UPROPERTY names are DIFFERENT to avoid compile conflicts with Blueprint variables.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/SkyLightComponent.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "Components/ExponentialHeightFogComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Curves/CurveFloat.h"
#include "SLFGameTypes.h"
#include "SLFSkyManager.generated.h"

// Forward declaration
class UPDA_DayNight;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTimeOfDayChanged, float, NormalizedTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeatherChanged, FName, NewWeather);

UENUM(BlueprintType)
enum class ESLFWeatherType : uint8
{
	Clear,
	Cloudy,
	Rain,
	Storm,
	Fog,
	Snow
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFSkyManager : public AActor
{
	GENERATED_BODY()

public:
	ASLFSkyManager();

	// ============================================================
	// SCS COMPONENTS - Created with CreateDefaultSubobject
	// Names match Blueprint SCS so properties are overridden
	// UPROPERTY names are different to avoid Blueprint variable conflicts
	// ============================================================

	/** Sun directional light - controls daytime lighting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDirectionalLightComponent* SunLightComponent;

	/** Moon directional light - controls nighttime lighting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDirectionalLightComponent* MoonLightComponent;

	/** Sky light for ambient lighting */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkyLightComponent* SkyLightComp;

	/** Sky atmosphere for realistic sky rendering */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkyAtmosphereComponent* SkyAtmosphereComp;

	/** Volumetric clouds */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UVolumetricCloudComponent* VolumetricCloudComp;

	/** Exponential height fog */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExponentialHeightFogComponent* HeightFogComp;

	/** Sky sphere mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* SkySphereComp;

	// ============================================================
	// VARIABLES
	// ============================================================

	/** Fog density curve - samples fog density based on time of day (0-24) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	UCurveFloat* FogDensityTrack;

	// Time Info Asset (PDA_DayNight with time entries for rest menu)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	UPrimaryDataAsset* TimeInfoAsset;

	// Current time entry info (cached from TimeInfoAsset)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	FSLFDayNightInfo TimeEntryInfo;

	// Time of Day - Primary time variable used by Blueprint visual updates
	// This is the main "Time" variable that B_SkyManager Blueprint uses for sun/sky rotation
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Time")
	double Time = 12.0;  // 0-24, matches Blueprint's Time variable

	// Legacy - kept for compatibility, synced with Time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float CurrentTimeOfDay = 12.0f;  // 0-24

	// Time dilation factor for day/night cycle speed
	// Higher = slower time progression (100.0 means time advances at 1/100th of real time)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config|Time")
	double TimeDilation = 100.0;  // bp_only default

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float DayLengthMinutes = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	bool bTimePaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float SunriseTime = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float SunsetTime = 18.0f;  // Matches pitch calculation: (18-6)/12*180 = 180 degrees

	// Weather
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	ESLFWeatherType CurrentWeather = ESLFWeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float WeatherIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float WeatherTransitionTime = 5.0f;

	// Optional external actor references (for backwards compatibility with level-placed lights)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	class ADirectionalLight* SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	class ASkyAtmosphere* SkyAtmosphereActor;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Sky|Events")
	FOnTimeOfDayChanged OnTimeOfDayChanged;

	UPROPERTY(BlueprintAssignable, Category = "Sky|Events")
	FOnWeatherChanged OnWeatherChanged;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	void SetTimeOfDay(float NewTime);
	virtual void SetTimeOfDay_Implementation(float NewTime);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Time")
	void AdvanceTime(float Hours);
	virtual void AdvanceTime_Implementation(float Hours);

	UFUNCTION(BlueprintPure, Category = "Time")
	bool IsDaytime() const;

	UFUNCTION(BlueprintPure, Category = "Time")
	float GetNormalizedTimeOfDay() const { return CurrentTimeOfDay / 24.0f; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weather")
	void SetWeather(ESLFWeatherType NewWeather, float Intensity);
	virtual void SetWeather_Implementation(ESLFWeatherType NewWeather, float Intensity);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Sky")
	void UpdateSky();
	virtual void UpdateSky_Implementation();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
