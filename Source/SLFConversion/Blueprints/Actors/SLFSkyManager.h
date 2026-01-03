// SLFSkyManager.h
// C++ base for B_SkyManager - Day/night cycle and weather controller
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFSkyManager.generated.h"

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
	// MIGRATION SUMMARY: B_SkyManager
	// Variables: 10 | Functions: 6 | Dispatchers: 2
	// ============================================================

	// Time of Day
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float CurrentTimeOfDay = 12.0f;  // 0-24

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float DayLengthMinutes = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	bool bTimePaused = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float SunriseTime = 6.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
	float SunsetTime = 20.0f;

	// Weather
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	ESLFWeatherType CurrentWeather = ESLFWeatherType::Clear;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float WeatherIntensity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weather")
	float WeatherTransitionTime = 5.0f;

	// References
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	class ADirectionalLight* SunLight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sky")
	class ASkyAtmosphere* SkyAtmosphere;

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
