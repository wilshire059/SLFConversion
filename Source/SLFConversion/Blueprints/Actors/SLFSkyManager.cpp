// SLFSkyManager.cpp
//
// COMPONENT PATTERN: CreateDefaultSubobject (like B_Container)
// Components are created with the SAME NAMES as Blueprint SCS.
// When the Blueprint is reparented to this C++ class, the Blueprint SCS
// properties OVERRIDE the C++ component defaults (mesh, intensity, etc.)

#include "SLFSkyManager.h"
#include "Engine/DirectionalLight.h"

ASLFSkyManager::ASLFSkyManager()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create root component
	USceneComponent* DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultSceneRoot);

	// Create components with SAME NAMES as Blueprint SCS
	// Blueprint SCS properties will OVERRIDE these defaults when reparented

	// Sun directional light - values from bp_only
	SunLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight_Sun"));
	SunLightComponent->SetupAttachment(RootComponent);
	SunLightComponent->SetMobility(EComponentMobility::Movable);
	SunLightComponent->SetAtmosphereSunLight(true);
	SunLightComponent->SetAtmosphereSunLightIndex(0);
	SunLightComponent->SetForwardShadingPriority(0);
	SunLightComponent->SetIntensity(4.0f);  // bp_only uses 4.0
	SunLightComponent->LightColor = FColor(255, 255, 255);  // Pure white
	SunLightComponent->SetRelativeLocation(FVector(-1.8e-12, -50.0, 1.3e-07));  // bp_only offset

	// Moon directional light - values from bp_only
	MoonLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLight_Moon"));
	MoonLightComponent->SetupAttachment(RootComponent);
	MoonLightComponent->SetMobility(EComponentMobility::Movable);
	MoonLightComponent->SetAtmosphereSunLight(true);  // bp_only: Moon ALSO has this true!
	MoonLightComponent->SetAtmosphereSunLightIndex(1);  // Index 1 for moon
	MoonLightComponent->SetForwardShadingPriority(1);
	MoonLightComponent->SetIntensity(1.0f);  // bp_only uses 1.0
	MoonLightComponent->LightColor = FColor(178, 200, 255);  // bp_only color
	MoonLightComponent->SetRelativeLocation(FVector(0.0, 50.0, 0.0));  // bp_only offset

	// Sky light - values from bp_only
	SkyLightComp = CreateDefaultSubobject<USkyLightComponent>(TEXT("SkyLight"));
	SkyLightComp->SetupAttachment(RootComponent);
	SkyLightComp->SetMobility(EComponentMobility::Movable);
	SkyLightComp->SetRealTimeCapture(true);  // CRITICAL: Must be true for dynamic sky updates
	SkyLightComp->SetRelativeLocation(FVector(50.0, 0.0, 0.0));  // bp_only offset

	// Sky atmosphere
	SkyAtmosphereComp = CreateDefaultSubobject<USkyAtmosphereComponent>(TEXT("SkyAtmosphere"));
	SkyAtmosphereComp->SetupAttachment(RootComponent);

	// Volumetric cloud
	VolumetricCloudComp = CreateDefaultSubobject<UVolumetricCloudComponent>(TEXT("VolumetricCloud"));
	VolumetricCloudComp->SetupAttachment(RootComponent);

	// Exponential height fog
	HeightFogComp = CreateDefaultSubobject<UExponentialHeightFogComponent>(TEXT("ExponentialHeightFog"));
	HeightFogComp->SetupAttachment(RootComponent);

	// Sky sphere mesh
	SkySphereComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_SkySphere"));
	SkySphereComp->SetupAttachment(RootComponent);
}

void ASLFSkyManager::BeginPlay()
{
	Super::BeginPlay();

	// From bp_only: BOTH Sun and Moon have AtmosphereSunLight=True, but different indices
	// Sun = Index 0, Moon = Index 1
	// This allows the sky atmosphere to blend both light sources properly
	if (SunLightComponent)
	{
		SunLightComponent->SetAtmosphereSunLight(true);
		SunLightComponent->SetAtmosphereSunLightIndex(0);
		SunLightComponent->SetForwardShadingPriority(0);
	}
	if (MoonLightComponent)
	{
		MoonLightComponent->SetAtmosphereSunLight(true);
		MoonLightComponent->SetAtmosphereSunLightIndex(1);
		MoonLightComponent->SetForwardShadingPriority(1);
	}

	// Log component status
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager] BeginPlay"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   SunLightComponent: %s"), SunLightComponent ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   MoonLightComponent: %s"), MoonLightComponent ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   SkyAtmosphereComp: %s"), SkyAtmosphereComp ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   SkyLightComp: %s"), SkyLightComp ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   HeightFogComp: %s"), HeightFogComp ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   FogDensityTrack: %s"), FogDensityTrack ? *FogDensityTrack->GetName() : TEXT("null"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   Time: %.2f"), Time);

	// Initial sky update
	UpdateSky();
}

void ASLFSkyManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bTimePaused)
	{
		// FORMULA FROM BP_ONLY:
		// Time = Time + (DeltaSeconds / TimeDilation)
		// With TimeDilation=100, time advances at 1/100th of real time
		double PreviousTime = Time;

		Time += static_cast<double>(DeltaTime) / TimeDilation;
		if (Time >= 24.0)
		{
			Time -= 24.0;
		}

		// Keep CurrentTimeOfDay in sync
		CurrentTimeOfDay = static_cast<float>(Time);

		// Update sky every game-minute (more frequent updates for smoother transitions)
		if (FMath::FloorToInt(PreviousTime * 60) != FMath::FloorToInt(Time * 60))
		{
			UpdateSky();
		}
	}
}

void ASLFSkyManager::SetTimeOfDay_Implementation(float NewTime)
{
	// Normalize the time to 0-24 range
	double NormalizedTime = FMath::Fmod(static_cast<double>(NewTime), 24.0);
	if (NormalizedTime < 0.0)
	{
		NormalizedTime += 24.0;
	}

	// Set both Time (used by Blueprint) and CurrentTimeOfDay (legacy)
	Time = NormalizedTime;
	CurrentTimeOfDay = static_cast<float>(NormalizedTime);

	UE_LOG(LogTemp, Warning, TEXT("[SkyManager] SetTimeOfDay called - NewTime: %.2f, NormalizedTime: %.2f"), NewTime, NormalizedTime);
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   SunLightComponent: %s"), SunLightComponent ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   MoonLightComponent: %s"), MoonLightComponent ? TEXT("VALID") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("[SkyManager]   FogDensityTrack: %s"), FogDensityTrack ? TEXT("VALID") : TEXT("NULL"));

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
	// FORMULA FROM BP_ONLY BLUEPRINT:
	// SunPitch = (Time / 24.0) * 360.0
	// MoonPitch = SunPitch + 180.0
	// Uses SetRelativeRotation with Roll=0, Yaw=0

	float SunPitch = (static_cast<float>(Time) / 24.0f) * 360.0f;
	float MoonPitch = SunPitch + 180.0f;

	// Update sun light component using RELATIVE rotation (not world!)
	if (SunLightComponent)
	{
		SunLightComponent->SetRelativeRotation(FRotator(SunPitch, 0.0f, 0.0f));

		UE_LOG(LogTemp, Warning, TEXT("[SkyManager] UpdateSky - Time: %.2f, Sun Pitch: %.1f"),
			Time, SunPitch);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[SkyManager] UpdateSky - SunLightComponent is NULL!"));
	}

	// Update moon light component using RELATIVE rotation
	if (MoonLightComponent)
	{
		MoonLightComponent->SetRelativeRotation(FRotator(MoonPitch, 0.0f, 0.0f));

		UE_LOG(LogTemp, Log, TEXT("[SkyManager] UpdateSky - Moon Pitch: %.1f"),
			MoonPitch);
	}

	// Also update external SunLight actor if set (backwards compatibility with level-placed lights)
	if (SunLight)
	{
		FRotator SunRotation(SunPitch, 0.0f, 0.0f);
		SunLight->SetActorRotation(SunRotation);
	}

	// FROM BP_ONLY: Sample FogDensityTrack curve and set fog density
	// The Blueprint uses FogDensityTrack->GetFloatValue(Time) to get fog density
	if (FogDensityTrack && HeightFogComp)
	{
		float FogDensity = FogDensityTrack->GetFloatValue(static_cast<float>(Time));
		HeightFogComp->SetFogDensity(FogDensity);

		UE_LOG(LogTemp, Log, TEXT("[SkyManager] UpdateSky - FogDensity: %.4f"),
			FogDensity);
	}

	// Force SkyLight to recapture after lighting changes
	// This ensures ambient lighting updates to match the new sun/moon position
	if (SkyLightComp && SkyLightComp->bRealTimeCapture)
	{
		SkyLightComp->RecaptureSky();
	}
}
