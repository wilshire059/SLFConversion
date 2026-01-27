// B_StatusEffectArea.cpp
// C++ implementation for Blueprint B_StatusEffectArea
//
// 20-PASS VALIDATION: 2026-01-19
// Source: BlueprintDNA_v2/Blueprint/B_StatusEffectArea.json
//
// Blueprint Logic:
// OnComponentBeginOverlap(Box):
//   1. Check OtherActor.ActorHasTag("Player")
//   2. If yes: GetComponentByClass<AC_StatusEffectManager>
//   3. If valid: StartBuildup(StatusEffectToApply, EffectRank)
//
// OnComponentEndOverlap(Box):
//   1. Check OtherActor.ActorHasTag("Player")
//   2. If yes: GetComponentByClass<AC_StatusEffectManager>
//   3. If valid: StopBuildup(StatusEffectToApply, true)

#include "Blueprints/B_StatusEffectArea.h"
#include "Components/AC_StatusEffectManager.h"
#include "Components/BoxComponent.h"

AB_StatusEffectArea::AB_StatusEffectArea()
{
	StatusEffectToApply = nullptr;
	EffectRank = 1;
	CachedBoxComponent = nullptr;
}

void AB_StatusEffectArea::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("AB_StatusEffectArea::BeginPlay - %s"), *GetName());

	// Cache reference to Box component (owned by Blueprint SCS)
	TArray<UBoxComponent*> BoxComponents;
	GetComponents<UBoxComponent>(BoxComponents);
	for (UBoxComponent* BC : BoxComponents)
	{
		if (BC && BC->GetName().Contains(TEXT("Box")))
		{
			CachedBoxComponent = BC;
			UE_LOG(LogTemp, Log, TEXT("  Found Box component: %s"), *BC->GetName());
			break;
		}
	}

	// Fallback: get any box component
	if (!CachedBoxComponent && BoxComponents.Num() > 0)
	{
		CachedBoxComponent = BoxComponents[0];
		UE_LOG(LogTemp, Log, TEXT("  Using first Box component: %s"), *CachedBoxComponent->GetName());
	}

	// Bind overlap events
	if (CachedBoxComponent)
	{
		CachedBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AB_StatusEffectArea::OnBoxBeginOverlap);
		CachedBoxComponent->OnComponentEndOverlap.AddDynamic(this, &AB_StatusEffectArea::OnBoxEndOverlap);
		UE_LOG(LogTemp, Log, TEXT("  Bound overlap events to Box component"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No Box component found - status effect area won't work!"));
	}

	// Log configuration
	if (StatusEffectToApply)
	{
		UE_LOG(LogTemp, Log, TEXT("  StatusEffectToApply: %s, Rank: %d"),
			*StatusEffectToApply->GetName(), EffectRank);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No StatusEffectToApply configured!"));
	}
}

void AB_StatusEffectArea::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("AB_StatusEffectArea::OnBoxBeginOverlap - Actor: %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("None"));

	if (!OtherActor)
	{
		return;
	}

	// Check if OtherActor has "Player" tag (Blueprint logic)
	if (!OtherActor->ActorHasTag(FName("Player")))
	{
		UE_LOG(LogTemp, Verbose, TEXT("  Actor does not have Player tag, ignoring"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("  Actor has Player tag, looking for StatusEffectManager"));

	// Get AC_StatusEffectManager from the actor
	UAC_StatusEffectManager* StatusEffectMgr = OtherActor->FindComponentByClass<UAC_StatusEffectManager>();
	if (!IsValid(StatusEffectMgr))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No AC_StatusEffectManager found on actor!"));
		return;
	}

	// Call StartBuildup
	if (StatusEffectToApply)
	{
		UE_LOG(LogTemp, Log, TEXT("  Calling StartBuildup for %s (Rank %d)"),
			*StatusEffectToApply->GetName(), EffectRank);
		StatusEffectMgr->StartBuildup(StatusEffectToApply, EffectRank);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("  No StatusEffectToApply configured!"));
	}
}

void AB_StatusEffectArea::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("AB_StatusEffectArea::OnBoxEndOverlap - Actor: %s"),
		OtherActor ? *OtherActor->GetName() : TEXT("None"));

	if (!OtherActor)
	{
		return;
	}

	// Check if OtherActor has "Player" tag (Blueprint logic)
	if (!OtherActor->ActorHasTag(FName("Player")))
	{
		UE_LOG(LogTemp, Verbose, TEXT("  Actor does not have Player tag, ignoring"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("  Actor has Player tag, looking for StatusEffectManager"));

	// Get AC_StatusEffectManager from the actor
	UAC_StatusEffectManager* StatusEffectMgr = OtherActor->FindComponentByClass<UAC_StatusEffectManager>();
	if (!IsValid(StatusEffectMgr))
	{
		UE_LOG(LogTemp, Warning, TEXT("  No AC_StatusEffectManager found on actor!"));
		return;
	}

	// Call StopBuildup (with ApplyDecayDelay = true, as per Blueprint)
	if (StatusEffectToApply)
	{
		UE_LOG(LogTemp, Log, TEXT("  Calling StopBuildup for %s"),
			*StatusEffectToApply->GetName());
		StatusEffectMgr->StopBuildup(StatusEffectToApply, true);
	}
}
