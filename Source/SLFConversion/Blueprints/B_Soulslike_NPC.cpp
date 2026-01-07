// B_Soulslike_NPC.cpp
// C++ implementation for Blueprint B_Soulslike_NPC
//
// 20-PASS VALIDATION: 2026-01-06 - Full interface implementation
// Source: BlueprintDNA/Blueprint/B_Soulslike_NPC.json

#include "Blueprints/B_Soulslike_NPC.h"
#include "Components/AIInteractionManagerComponent.h"
#include "Components/SphereComponent.h"

AB_Soulslike_NPC::AB_Soulslike_NPC()
{
	// Initialize variables
	bCanBeTraced = true;
	LookAtComponent = nullptr;
	NpcId = FGuid::NewGuid();

	// Create components
	AC_AI_InteractionManager = CreateDefaultSubobject<UAIInteractionManagerComponent>(TEXT("AC_AI_InteractionManager"));

	LookAtRadius = CreateDefaultSubobject<USphereComponent>(TEXT("LookAtRadius"));
	if (LookAtRadius)
	{
		LookAtRadius->SetupAttachment(RootComponent);
		LookAtRadius->SetSphereRadius(300.0f);
		LookAtRadius->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		LookAtRadius->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void AB_Soulslike_NPC::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_NPC] BeginPlay - %s"), *GetName());

	// Set LookAtComponent to LookAtRadius if not set
	if (!LookAtComponent && LookAtRadius)
	{
		LookAtComponent = LookAtRadius;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// LOCAL FUNCTIONS
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Soulslike_NPC::GetLookAtLocation(FVector& Location)
{
	// JSON Logic: IsValid(LookAtComponent) -> GetComponentLocation, else return zero
	if (IsValid(LookAtComponent))
	{
		Location = LookAtComponent->GetComponentLocation();
	}
	else
	{
		Location = FVector::ZeroVector;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// BPI_Interactable INTERFACE IMPLEMENTATIONS
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Soulslike_NPC::TryGetItemInfo_Implementation(FSLFItemInfo& ItemInfo)
{
	// JSON Logic: Return empty ItemInfo (NPC has no item info)
	ItemInfo = FSLFItemInfo();

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_NPC] TryGetItemInfo - NPC has no item info"));
}

void AB_Soulslike_NPC::OnSpawnedFromSave_Implementation(FGuid Id, FInstancedStruct CustomData)
{
	// JSON Logic: Set NpcId from save data
	NpcId = Id;

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_NPC] OnSpawnedFromSave - Id: %s"), *Id.ToString());
}

void AB_Soulslike_NPC::OnInteract_Implementation(AActor* InteractingActor)
{
	// JSON Logic: Start dialog with the interacting actor
	// The actual dialog handling is done via AC_AI_InteractionManager->BeginDialog
	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_NPC] OnInteract - Actor: %s"),
		InteractingActor ? *InteractingActor->GetName() : TEXT("null"));

	// Dialog is started via the player controller calling BeginDialog on AC_AI_InteractionManager
}

void AB_Soulslike_NPC::OnTraced_Implementation(AActor* TracedBy)
{
	// JSON Logic: Check bCanBeTraced, show interaction prompt if traceable
	if (!bCanBeTraced)
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Soulslike_NPC] OnTraced - TracedBy: %s"),
		TracedBy ? *TracedBy->GetName() : TEXT("null"));

	// Trace highlighting/prompt is handled by the interaction system
}
