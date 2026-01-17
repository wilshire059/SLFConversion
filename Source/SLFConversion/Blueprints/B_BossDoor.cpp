// B_BossDoor.cpp
// C++ implementation for Blueprint B_BossDoor
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_BossDoor.json

#include "Blueprints/B_BossDoor.h"
#include "Components/PrimitiveComponent.h"

AB_BossDoor::AB_BossDoor()
{
	bCanBeTraced = true;
	bIsActivated = false;

	// Create portal niagara components
	Portal_Front = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Portal_Front"));
	Portal_Back = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Portal_Back"));

	// Create death currency spawn point
	DeathCurrencyDropLocation = CreateDefaultSubobject<UBillboardComponent>(TEXT("DeathCurrencyDropLocation"));
	DeathCurrencyDropLocation->SetupAttachment(RootComponent);
}

void AB_BossDoor::BeginPlay()
{
	Super::BeginPlay();

	// Bind to parent's OnDoorOpened dispatcher to re-enable collision after passing through
	OnDoorOpened.AddDynamic(this, &AB_BossDoor::HandleDoorOpened);

	// Debug: Log the state of the boss door
	UE_LOG(LogTemp, Warning, TEXT("AB_BossDoor::BeginPlay - %s"), *GetName());
	UE_LOG(LogTemp, Warning, TEXT("  Class: %s"), *GetClass()->GetName());
	UE_LOG(LogTemp, Warning, TEXT("  bCanBeTraced: %s"), bCanBeTraced ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("  bIsActivated: %s"), bIsActivated ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("  Portal_Front: %s"), IsValid(Portal_Front) ? TEXT("valid") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("  Portal_Back: %s"), IsValid(Portal_Back) ? TEXT("valid") : TEXT("NULL"));

	// Debug: List all components and their collision state
	TArray<UActorComponent*> Components;
	GetComponents(Components);
	UE_LOG(LogTemp, Warning, TEXT("  Components (%d):"), Components.Num());
	for (UActorComponent* Comp : Components)
	{
		if (!IsValid(Comp)) continue;

		FString CollisionInfo = TEXT("");
		if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp))
		{
			ECollisionEnabled::Type CollisionType = PrimComp->GetCollisionEnabled();
			const TCHAR* CollisionStr = TEXT("Unknown");
			switch (CollisionType)
			{
				case ECollisionEnabled::NoCollision: CollisionStr = TEXT("NoCollision"); break;
				case ECollisionEnabled::QueryOnly: CollisionStr = TEXT("QueryOnly"); break;
				case ECollisionEnabled::PhysicsOnly: CollisionStr = TEXT("PhysicsOnly"); break;
				case ECollisionEnabled::QueryAndPhysics: CollisionStr = TEXT("QueryAndPhysics"); break;
				case ECollisionEnabled::ProbeOnly: CollisionStr = TEXT("ProbeOnly"); break;
				case ECollisionEnabled::QueryAndProbe: CollisionStr = TEXT("QueryAndProbe"); break;
			}

			// Get object type
			EObjectTypeQuery ObjectType = UEngineTypes::ConvertToObjectType(PrimComp->GetCollisionObjectType());
			CollisionInfo = FString::Printf(TEXT(" [Collision: %s, ObjectType: %d]"), CollisionStr, (int32)ObjectType);
		}

		UE_LOG(LogTemp, Warning, TEXT("    - %s (%s)%s"),
			*Comp->GetName(), *Comp->GetClass()->GetName(), *CollisionInfo);
	}

	// Ensure collision is enabled on the interactable mesh at startup
	// (unless door is activated - fog gate should be passable)
	if (!bIsActivated)
	{
		SetInteractableMeshCollision(true);
		UE_LOG(LogTemp, Warning, TEXT("  -> Enabled collision on Interactable SM (not activated)"));
	}
}

void AB_BossDoor::OnInteract_Implementation(AActor* InteractingActor)
{
	// From Blueprint EventGraph:
	// 1. Call parent OnInteract (handles door opening logic)
	// 2. If IsActivated (fog gate active), disable collision so player can walk through

	UE_LOG(LogTemp, Log, TEXT("AB_BossDoor::OnInteract - IsActivated: %s"),
		bIsActivated ? TEXT("true") : TEXT("false"));

	// Call parent to handle door opening animation/logic
	Super::OnInteract_Implementation(InteractingActor);

	// If fog gate is active, disable collision to allow player to pass through
	if (bIsActivated)
	{
		SetInteractableMeshCollision(false);
		UE_LOG(LogTemp, Log, TEXT("AB_BossDoor: Fog gate active, disabled collision for pass-through"));
	}
}

USceneComponent* AB_BossDoor::GetDeathCurrencySpawnPoint_Implementation()
{
	return DeathCurrencyDropLocation;
}

void AB_BossDoor::UnlockBossDoor_Implementation()
{
	// Called when boss is defeated (from AC_AI_Boss)
	// Deactivate portal effects (fog gates) and unlock door

	UE_LOG(LogTemp, Log, TEXT("AB_BossDoor::UnlockBossDoor - Destroying fog gates"));

	// Deactivate portal effects (fog gates)
	if (IsValid(Portal_Front))
	{
		Portal_Front->DestroyComponent();
		Portal_Front = nullptr;
	}

	if (IsValid(Portal_Back))
	{
		Portal_Back->DestroyComponent();
		Portal_Back = nullptr;
	}

	// Mark as can be traced/interacted, no longer activated
	bCanBeTraced = true;
	bIsActivated = false;

	// Unlock the door itself
	LockInfo.bIsLocked = false;

	UE_LOG(LogTemp, Log, TEXT("AB_BossDoor: Boss door unlocked, fog gates destroyed"));
}

void AB_BossDoor::InitializeLoadedStates(bool bInCanBeTraced, bool bInIsActivated)
{
	bCanBeTraced = bInCanBeTraced;
	bIsActivated = bInIsActivated;

	UE_LOG(LogTemp, Log, TEXT("AB_BossDoor::InitializeLoadedStates - CanBeTraced: %s, IsActivated: %s"),
		bInCanBeTraced ? TEXT("true") : TEXT("false"),
		bInIsActivated ? TEXT("true") : TEXT("false"));

	// If activated, run activation logic (show fog gates)
	if (bIsActivated)
	{
		if (IsValid(Portal_Front))
		{
			Portal_Front->SetVisibility(true);
			Portal_Front->Activate();
		}
		if (IsValid(Portal_Back))
		{
			Portal_Back->SetVisibility(true);
			Portal_Back->Activate();
		}
	}
	else
	{
		// Not activated - hide the portal effects
		if (IsValid(Portal_Front))
		{
			Portal_Front->SetVisibility(false);
			Portal_Front->Deactivate();
		}
		if (IsValid(Portal_Back))
		{
			Portal_Back->SetVisibility(false);
			Portal_Back->Deactivate();
		}
	}
}

void AB_BossDoor::HandleDoorOpened()
{
	// Called when door animation finishes (player has passed through)
	// Re-enable collision on the mesh so the door blocks again

	UE_LOG(LogTemp, Log, TEXT("AB_BossDoor::HandleDoorOpened - Re-enabling collision"));

	SetInteractableMeshCollision(true);
}

void AB_BossDoor::SetInteractableMeshCollision(bool bEnableCollision)
{
	// Find the "Interactable SM" component by name (Blueprint-defined static mesh)
	// This is the fog gate mesh that blocks the player

	TArray<UActorComponent*> Components;
	GetComponents(Components);

	for (UActorComponent* Comp : Components)
	{
		// Check for "Interactable SM" or similar mesh component
		if (Comp && (Comp->GetName().Contains(TEXT("Interactable")) || Comp->GetName().Contains(TEXT("SM"))))
		{
			if (UPrimitiveComponent* PrimComp = Cast<UPrimitiveComponent>(Comp))
			{
				if (bEnableCollision)
				{
					PrimComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
					UE_LOG(LogTemp, Log, TEXT("  Enabled collision on: %s"), *Comp->GetName());
				}
				else
				{
					PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					UE_LOG(LogTemp, Log, TEXT("  Disabled collision on: %s"), *Comp->GetName());
				}
			}
		}
	}
}

