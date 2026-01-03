// SLFSkyManager.cpp
#include "SLFSkyManager.h"
#include "Engine/DirectionalLight.h"
#include "Components/LightComponent.h"

ASLFSkyManager::ASLFSkyManager()
{
	PrimaryActorTick.bCanEverTick = true;

	UE_LOG(LogTemp, Log, TEXT("[SkyManager] Created"));
}

void ASLFSkyManager::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Log, TEXT("[SkyManager] BeginPlay - Time: %.1f, DayLength: %.1f min"),
		CurrentTimeOfDay, DayLengthMinutes);
	UpdateSky();
}

void ASLFSkyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bTimePaused)
	{
		// Calculate hours per real second
		float HoursPerSecond = 24.0f / (DayLengthMinutes * 60.0f);
		float PreviousTime = CurrentTimeOfDay;

		CurrentTimeOfDay += HoursPerSecond * DeltaTime;
		if (CurrentTimeOfDay >= 24.0f)
		{
			CurrentTimeOfDay -= 24.0f;
		}

		// Update sky every second of game time (not real time)
		if (FMath::FloorToInt(PreviousTime * 60) != FMath::FloorToInt(CurrentTimeOfDay * 60))
		{
			UpdateSky();
		}
	}
}

void ASLFSkyManager::SetTimeOfDay_Implementation(float NewTime)
{
	CurrentTimeOfDay = FMath::Fmod(NewTime, 24.0f);
	if (CurrentTimeOfDay < 0.0f)
	{
		CurrentTimeOfDay += 24.0f;
	}

	UE_LOG(LogTemp, Log, TEXT("[SkyManager] Time set to: %.2f"), CurrentTimeOfDay);
	OnTimeOfDayChanged.Broadcast(GetNormalizedTimeOfDay());
	UpdateSky();
}

void ASLFSkyManager::AdvanceTime_Implementation(float Hours)
{
	SetTimeOfDay(CurrentTimeOfDay + Hours);
}

bool ASLFSkyManager::IsDaytime() const
{
	return CurrentTimeOfDay >= SunriseTime && CurrentTimeOfDay < SunsetTime;
}

void ASLFSkyManager::SetWeather_Implementation(ESLFWeatherType NewWeather, float Intensity)
{
	CurrentWeather = NewWeather;
	WeatherIntensity = FMath::Clamp(Intensity, 0.0f, 1.0f);

	FName WeatherName;
	switch (NewWeather)
	{
		case ESLFWeatherType::Clear: WeatherName = FName("Clear"); break;
		case ESLFWeatherType::Cloudy: WeatherName = FName("Cloudy"); break;
		case ESLFWeatherType::Rain: WeatherName = FName("Rain"); break;
		case ESLFWeatherType::Storm: WeatherName = FName("Storm"); break;
		case ESLFWeatherType::Fog: WeatherName = FName("Fog"); break;
		case ESLFWeatherType::Snow: WeatherName = FName("Snow"); break;
	}

	UE_LOG(LogTemp, Log, TEXT("[SkyManager] Weather changed to: %s (%.0f%%)"), *WeatherName.ToString(), WeatherIntensity * 100.0f);
	OnWeatherChanged.Broadcast(WeatherName);
}

void ASLFSkyManager::UpdateSky_Implementation()
{
	if (SunLight)
	{
		// Calculate sun angle based on time
		float SunAngle = ((CurrentTimeOfDay - 6.0f) / 12.0f) * 180.0f;
		FRotator SunRotation(SunAngle, 0.0f, 0.0f);
		SunLight->SetActorRotation(SunRotation);

		// Adjust intensity based on day/night
		float Intensity = IsDaytime() ? 1.0f : 0.05f;
		if (SunLight->GetLightComponent())
		{
			SunLight->GetLightComponent()->SetIntensity(Intensity * 10.0f);
		}
	}
}
