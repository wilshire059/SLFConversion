// SLFNiagaraFactory.cpp
// Implementation of programmatic Niagara system creation
//
// NOTE: This demonstrates what's POSSIBLE in C++ - cloning and using Niagara systems.
// Production VFX should still be created in the Niagara Editor for quality.

#include "SLFNiagaraFactory.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

UNiagaraSystem* USLFNiagaraFactory::CreateMagicProjectileEffect(
	FLinearColor Color,
	float ParticleSize,
	float SpawnRate)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Creating Magic Projectile Effect - Color: %s, Size: %.1f, Rate: %.1f"),
		*Color.ToString(), ParticleSize, SpawnRate);

	// Try to find a suitable template system to clone
	// Using existing project assets gives the best results
	static const TCHAR* TemplatePaths[] = {
		TEXT("/Game/SoulslikeFramework/VFX/NS_MagicTrail.NS_MagicTrail"),
		TEXT("/Game/SoulslikeFramework/VFX/NS_Trail.NS_Trail"),
		TEXT("/Niagara/DefaultAssets/Templates/Systems/Simple_Sprite_Burst.Simple_Sprite_Burst"),
		TEXT("/Niagara/DefaultAssets/Templates/Systems/SimpleSpriteBurst.SimpleSpriteBurst"),
	};

	UNiagaraSystem* TemplateSystem = nullptr;
	for (const TCHAR* Path : TemplatePaths)
	{
		TemplateSystem = LoadObject<UNiagaraSystem>(nullptr, Path);
		if (TemplateSystem)
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Found template: %s"), Path);
			break;
		}
	}

	if (TemplateSystem)
	{
		// Clone the template as a transient system
		UNiagaraSystem* NewSystem = DuplicateObject<UNiagaraSystem>(
			TemplateSystem,
			GetTransientPackage(),
			MakeUniqueObjectName(GetTransientPackage(), UNiagaraSystem::StaticClass(), TEXT("GeneratedMagicEffect")));

		if (NewSystem)
		{
			NewSystem->SetFlags(RF_Transient);
			UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Created magic effect from template: %s"), *NewSystem->GetName());
			return NewSystem;
		}
	}

	// Fallback: Create a minimal system that may not render but won't crash
	UE_LOG(LogTemp, Warning, TEXT("[SLFNiagaraFactory] No template found - creating minimal system"));

	UNiagaraSystem* NewSystem = NewObject<UNiagaraSystem>(
		GetTransientPackage(),
		MakeUniqueObjectName(GetTransientPackage(), UNiagaraSystem::StaticClass(), TEXT("GeneratedMagicEffect")),
		RF_Transient);

	if (NewSystem)
	{
		NewSystem->SetFlags(RF_Transient);
		UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Created minimal system (may not render without proper modules)"));
	}

	return NewSystem;
}

UNiagaraSystem* USLFNiagaraFactory::CreateImpactBurstEffect(
	FLinearColor Color,
	float ParticleSize,
	int32 BurstCount)
{
	UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Creating Impact Burst Effect - Color: %s, Size: %.1f, Count: %d"),
		*Color.ToString(), ParticleSize, BurstCount);

	// Try to find burst/impact templates
	static const TCHAR* TemplatePaths[] = {
		TEXT("/Game/SoulslikeFramework/VFX/NS_Impact.NS_Impact"),
		TEXT("/Game/SoulslikeFramework/VFX/NS_Burst.NS_Burst"),
		TEXT("/Niagara/DefaultAssets/Templates/Systems/Simple_Sprite_Burst.Simple_Sprite_Burst"),
	};

	UNiagaraSystem* TemplateSystem = nullptr;
	for (const TCHAR* Path : TemplatePaths)
	{
		TemplateSystem = LoadObject<UNiagaraSystem>(nullptr, Path);
		if (TemplateSystem)
		{
			UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Found impact template: %s"), Path);
			break;
		}
	}

	if (TemplateSystem)
	{
		UNiagaraSystem* NewSystem = DuplicateObject<UNiagaraSystem>(
			TemplateSystem,
			GetTransientPackage(),
			MakeUniqueObjectName(GetTransientPackage(), UNiagaraSystem::StaticClass(), TEXT("GeneratedImpactEffect")));

		if (NewSystem)
		{
			NewSystem->SetFlags(RF_Transient);
			UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Created impact effect: %s"), *NewSystem->GetName());
			return NewSystem;
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[SLFNiagaraFactory] Could not find burst template, using magic effect as fallback"));
	return CreateMagicProjectileEffect(Color, ParticleSize, (float)BurstCount);
}

UNiagaraSystem* USLFNiagaraFactory::CreateFireSpellEffect()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Creating Fire Spell Effect"));

	// Try to load existing fire effect from project or engine
	UNiagaraSystem* FireTemplate = LoadObject<UNiagaraSystem>(nullptr,
		TEXT("/Game/SoulslikeFramework/VFX/NS_Fire.NS_Fire"));

	if (!FireTemplate)
	{
		// Try engine default
		FireTemplate = LoadObject<UNiagaraSystem>(nullptr,
			TEXT("/Niagara/DefaultAssets/Templates/Systems/Simple_Flames.Simple_Flames"));
	}

	if (FireTemplate)
	{
		UNiagaraSystem* NewSystem = DuplicateObject<UNiagaraSystem>(
			FireTemplate,
			GetTransientPackage(),
			MakeUniqueObjectName(GetTransientPackage(), UNiagaraSystem::StaticClass(), TEXT("GeneratedFireSpell")));

		if (NewSystem)
		{
			NewSystem->SetFlags(RF_Transient);
			UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Created fire spell from template"));
			return NewSystem;
		}
	}

	// Fallback to colored particle effect
	return CreateMagicProjectileEffect(
		FLinearColor(1.0f, 0.3f, 0.0f, 1.0f),  // Orange-red
		25.0f,
		100.0f);
}

UNiagaraSystem* USLFNiagaraFactory::CreateHealingSpellEffect()
{
	UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Creating Healing Spell Effect"));

	// Create a green/golden healing effect
	return CreateMagicProjectileEffect(
		FLinearColor(0.2f, 1.0f, 0.4f, 1.0f),  // Green-gold
		20.0f,
		40.0f);
}

void USLFNiagaraFactory::ApplyGeneratedEffect(UNiagaraComponent* Component, UNiagaraSystem* System)
{
	if (!Component || !System)
	{
		UE_LOG(LogTemp, Warning, TEXT("[SLFNiagaraFactory] ApplyGeneratedEffect - Invalid component or system!"));
		return;
	}

	Component->SetAsset(System);
	Component->Activate(true);

	UE_LOG(LogTemp, Log, TEXT("[SLFNiagaraFactory] Applied generated effect %s to component %s"),
		*System->GetName(), *Component->GetName());
}
