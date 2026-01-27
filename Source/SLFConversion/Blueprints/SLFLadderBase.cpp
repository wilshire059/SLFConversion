// SLFLadderBase.cpp
// C++ implementation for B_Ladder
//
// COMPONENT OWNERSHIP: Blueprint SCS owns all components.
// C++ only caches references at runtime. See CLAUDE.md for pattern.

#include "SLFLadderBase.h"
#include "Components/SceneComponent.h"
#include "Components/LadderManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

ASLFLadderBase::ASLFLadderBase()
{
	// DefaultSceneRoot is created by parent SLFInteractableBase
	// Components are created by Blueprint SCS - we cache references in BeginPlay

	// Initialize cached component pointers
	CachedBarz = nullptr;
	CachedPole = nullptr;
	CachedConnections = nullptr;
	CachedClimbingCollision = nullptr;
	CachedBottomCollision = nullptr;
	CachedTopCollision = nullptr;
	CachedTopdownCollision = nullptr;

	// Initialize basic config
	LadderHeight = 400.0;
	BarOffset = 40.0;
	OvershootCount = 2;
	ConnectionCount = 2;
	bBarAtZero = true;
	bAddFinishBar = true;

	// Initialize top collision
	TopCollisionExtent = FVector(50.0, 30.0, 30.0);
	TopCollisionOffsetX = 50.0;
	TopCollisionOffsetZ = 0.0;

	// Initialize bottom collision
	BottomCollisionExtent = FVector(50.0, 30.0, 30.0);
	BottomCollisionOffsetX = 50.0;
	BottomCollisionOffsetZ = 50.0;

	// Initialize topdown collision
	TopdownCollisionExtent = FVector(30.0, 30.0, 50.0);
	TopdownCollisionOffsetX = 30.0;
	TopdownCollisionOffsetZ = -30.0;

	// Initialize runtime
	Cache_LastBarIndex = 0;

	// Update interaction prompt (inherited from SLFInteractableBase)
	InteractionPrompt = FText::FromString(TEXT("Climb"));
}

void ASLFLadderBase::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Cache component references from Blueprint SCS for editor-time visibility
	const TSet<UActorComponent*>& AllComponents = GetComponents();
	for (UActorComponent* Component : AllComponents)
	{
		if (!Component) continue;
		FString CompName = Component->GetName();

		// Cache instanced static mesh components
		if (UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Component))
		{
			if (CompName.Contains(TEXT("Barz")) && !CachedBarz)
			{
				CachedBarz = ISM;
			}
			else if (CompName.Contains(TEXT("Pole")) && !CachedPole)
			{
				CachedPole = ISM;
			}
			else if (CompName.Contains(TEXT("Connection")) && !CachedConnections)
			{
				CachedConnections = ISM;
			}
		}
		// Cache box collision components (for editor positioning)
		else if (UBoxComponent* Box = Cast<UBoxComponent>(Component))
		{
			if (CompName.Contains(TEXT("Climbing")) && !CachedClimbingCollision)
			{
				CachedClimbingCollision = Box;
			}
			else if (CompName.Contains(TEXT("Bottom")) && !CachedBottomCollision)
			{
				CachedBottomCollision = Box;
			}
			else if (CompName.Contains(TEXT("Topdown")) && !CachedTopdownCollision)
			{
				CachedTopdownCollision = Box;
			}
			else if (CompName.Contains(TEXT("Top")) && !CachedTopCollision)
			{
				CachedTopCollision = Box;
			}
		}
	}

	// Generate the ladder mesh instances for editor visibility
	// This is equivalent to bp_only's UserConstructionScript calling CreateLadder
	if (CachedBarz || CachedPole)
	{
		// Log mesh status BEFORE calling CreateLadder
		UStaticMesh* BarzMesh = CachedBarz ? CachedBarz->GetStaticMesh() : nullptr;
		UStaticMesh* PoleMesh = CachedPole ? CachedPole->GetStaticMesh() : nullptr;

		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnConstruction - PRE CreateLadder: Barz=%s (Mesh=%s), Pole=%s (Mesh=%s)"),
			CachedBarz ? TEXT("Yes") : TEXT("No"),
			BarzMesh ? *BarzMesh->GetName() : TEXT("NULL"),
			CachedPole ? TEXT("Yes") : TEXT("No"),
			PoleMesh ? *PoleMesh->GetName() : TEXT("NULL"));

		// Check visibility settings BEFORE CreateLadder
		if (CachedBarz)
		{
			UE_LOG(LogTemp, Log, TEXT("[Ladder] Barz VISIBILITY: Visible=%s, HiddenInGame=%s, bRenderInMainPass=%s"),
				CachedBarz->IsVisible() ? TEXT("Yes") : TEXT("No"),
				CachedBarz->bHiddenInGame ? TEXT("Yes") : TEXT("No"),
				CachedBarz->bRenderInMainPass ? TEXT("Yes") : TEXT("No"));
		}

		CreateLadder();

		// Log instance count AFTER calling CreateLadder
		int32 BarzInstances = CachedBarz ? CachedBarz->GetInstanceCount() : 0;
		int32 PoleInstances = CachedPole ? CachedPole->GetInstanceCount() : 0;

		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnConstruction - POST CreateLadder: Barz instances=%d, Pole instances=%d"),
			BarzInstances, PoleInstances);

		// Force visibility ON for ISM components
		if (CachedBarz)
		{
			CachedBarz->SetVisibility(true);
			CachedBarz->SetHiddenInGame(false);
		}
		if (CachedPole)
		{
			CachedPole->SetVisibility(true);
			CachedPole->SetHiddenInGame(false);
		}
		if (CachedConnections)
		{
			CachedConnections->SetVisibility(true);
			CachedConnections->SetHiddenInGame(false);
		}
	}
}

void ASLFLadderBase::BeginPlay()
{
	Super::BeginPlay();

	// Cache component references from Blueprint SCS
	const TSet<UActorComponent*>& AllComponents = GetComponents();
	for (UActorComponent* Component : AllComponents)
	{
		if (!Component) continue;
		FString CompName = Component->GetName();

		// Cache instanced static mesh components
		if (UInstancedStaticMeshComponent* ISM = Cast<UInstancedStaticMeshComponent>(Component))
		{
			if (CompName.Contains(TEXT("Barz")) && !CachedBarz)
			{
				CachedBarz = ISM;
			}
			else if (CompName.Contains(TEXT("Pole")) && !CachedPole)
			{
				CachedPole = ISM;
			}
			else if (CompName.Contains(TEXT("Connection")) && !CachedConnections)
			{
				CachedConnections = ISM;
			}
		}
		// Cache box collision components
		else if (UBoxComponent* Box = Cast<UBoxComponent>(Component))
		{
			if (CompName.Contains(TEXT("Climbing")) && !CachedClimbingCollision)
			{
				CachedClimbingCollision = Box;
			}
			else if (CompName.Contains(TEXT("Bottom")) && !CachedBottomCollision)
			{
				CachedBottomCollision = Box;
			}
			else if (CompName.Contains(TEXT("Topdown")) && !CachedTopdownCollision)
			{
				CachedTopdownCollision = Box;
			}
			else if (CompName.Contains(TEXT("Top")) && !CachedTopCollision)
			{
				CachedTopCollision = Box;
			}
		}
	}

	// Bind overlap events for ladder collision zones
	if (CachedClimbingCollision)
	{
		CachedClimbingCollision->OnComponentEndOverlap.AddDynamic(this, &ASLFLadderBase::OnClimbingCollisionEndOverlap);
	}

	if (CachedBottomCollision)
	{
		CachedBottomCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLFLadderBase::OnBottomCollisionBeginOverlap);
		CachedBottomCollision->OnComponentEndOverlap.AddDynamic(this, &ASLFLadderBase::OnBottomCollisionEndOverlap);
	}

	if (CachedTopCollision)
	{
		CachedTopCollision->OnComponentBeginOverlap.AddDynamic(this, &ASLFLadderBase::OnTopCollisionBeginOverlap);
		CachedTopCollision->OnComponentEndOverlap.AddDynamic(this, &ASLFLadderBase::OnTopCollisionEndOverlap);
	}

	if (CachedTopdownCollision)
	{
		CachedTopdownCollision->OnComponentEndOverlap.AddDynamic(this, &ASLFLadderBase::OnTopdownCollisionEndOverlap);
	}

	UE_LOG(LogTemp, Log, TEXT("[Ladder] BeginPlay - Height: %.2f, Components cached: Barz=%s, Pole=%s, Climbing=%s, Bottom=%s, Top=%s"),
		LadderHeight,
		CachedBarz ? TEXT("Yes") : TEXT("No"),
		CachedPole ? TEXT("Yes") : TEXT("No"),
		CachedClimbingCollision ? TEXT("Yes") : TEXT("No"),
		CachedBottomCollision ? TEXT("Yes") : TEXT("No"),
		CachedTopCollision ? TEXT("Yes") : TEXT("No"));

	// Check mesh assignments for visibility debugging
	if (CachedBarz)
	{
		UStaticMesh* BarzMesh = CachedBarz->GetStaticMesh();
		UE_LOG(LogTemp, Log, TEXT("[Ladder] Barz ISM mesh: %s"), BarzMesh ? *BarzMesh->GetName() : TEXT("NULL - LADDER WILL BE INVISIBLE!"));
	}
	if (CachedPole)
	{
		UStaticMesh* PoleMesh = CachedPole->GetStaticMesh();
		UE_LOG(LogTemp, Log, TEXT("[Ladder] Pole ISM mesh: %s"), PoleMesh ? *PoleMesh->GetName() : TEXT("NULL - LADDER WILL BE INVISIBLE!"));
	}

	// Log ClimbingCollision extents from Blueprint SCS (we DON'T override these anymore)
	if (CachedClimbingCollision)
	{
		FVector Extent = CachedClimbingCollision->GetUnscaledBoxExtent();
		FVector Location = CachedClimbingCollision->GetRelativeLocation();
		UE_LOG(LogTemp, Log, TEXT("[Ladder] ClimbingCollision from Blueprint SCS - Extent: (%.1f, %.1f, %.1f), Location: (%.1f, %.1f, %.1f)"),
			Extent.X, Extent.Y, Extent.Z, Location.X, Location.Y, Location.Z);
	}

	// Generate the ladder mesh instances (equivalent to Blueprint's UserConstructionScript calling CreateLadder)
	CreateLadder();
}

double ASLFLadderBase::GetPoleHeight_Implementation()
{
	// Get pole height from mesh bounds if available
	if (CachedPole && CachedPole->GetStaticMesh())
	{
		FBoxSphereBounds Bounds = CachedPole->GetStaticMesh()->GetBounds();
		double PoleZ = Bounds.BoxExtent.Z * 2.0;
		UE_LOG(LogTemp, Verbose, TEXT("[Ladder] GetPoleHeight - PoleZ from mesh: %f"), PoleZ);
		return PoleZ;
	}

	// Fallback calculation
	return LadderHeight + (OvershootCount * BarOffset);
}

void ASLFLadderBase::CreateLadder_Implementation()
{
	// Calculate number of bars
	int32 NumBars = FMath::FloorToInt(LadderHeight / BarOffset);
	if (bBarAtZero) NumBars++;
	if (bAddFinishBar) NumBars++;
	Cache_LastBarIndex = NumBars;

	UE_LOG(LogTemp, Log, TEXT("[Ladder] CreateLadder - NumBars: %d, LadderHeight: %.2f, BarOffset: %.2f"),
		NumBars, LadderHeight, BarOffset);

	// Clear existing instances
	if (CachedBarz)
	{
		CachedBarz->ClearInstances();
	}
	if (CachedPole)
	{
		CachedPole->ClearInstances();
	}
	if (CachedConnections)
	{
		CachedConnections->ClearInstances();
	}

	// Add bar instances - one at each BarOffset interval
	if (CachedBarz)
	{
		UStaticMesh* BarzMesh = CachedBarz->GetStaticMesh();
		if (!BarzMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("[Ladder] CreateLadder - Barz has NO MESH ASSIGNED! Ladder will be invisible."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[Ladder] CreateLadder - Barz mesh: %s"), *BarzMesh->GetName());
		}

		int32 StartIndex = bBarAtZero ? 0 : 1;
		int32 EndIndex = NumBars;

		for (int32 i = StartIndex; i < EndIndex; i++)
		{
			// Calculate Z position: Index * BarOffset
			double ZPos = static_cast<double>(i) * BarOffset;
			FTransform BarTransform;
			BarTransform.SetLocation(FVector(0.0, 0.0, ZPos));
			BarTransform.SetRotation(FQuat::Identity);
			BarTransform.SetScale3D(FVector::OneVector);
			CachedBarz->AddInstance(BarTransform, false);  // false = local space, transforms are relative to component
		}

		UE_LOG(LogTemp, Log, TEXT("[Ladder] Added %d bar instances to Barz (total now: %d)"),
			EndIndex - StartIndex, CachedBarz->GetInstanceCount());
	}

	// Add pole instances - stacked vertically to match ladder height
	// Blueprint stacks pole segments at Z = Index * PoleHeight
	if (CachedPole)
	{
		UStaticMesh* PoleMesh = CachedPole->GetStaticMesh();
		if (!PoleMesh)
		{
			UE_LOG(LogTemp, Error, TEXT("[Ladder] CreateLadder - Pole has NO MESH ASSIGNED! Ladder will be invisible."));
		}
		else
		{
			// Get pole mesh height from bounds
			FBoxSphereBounds PoleBounds = PoleMesh->GetBounds();
			double PoleHeight = PoleBounds.BoxExtent.Z * 2.0;  // Full height (extents are half-size)

			UE_LOG(LogTemp, Log, TEXT("[Ladder] CreateLadder - Pole mesh: %s, PoleHeight: %.2f"),
				*PoleMesh->GetName(), PoleHeight);

			if (PoleHeight > 0.0)
			{
				// Calculate how many pole segments needed to cover ladder height + overshoot
				double TotalPoleHeight = LadderHeight + (OvershootCount * BarOffset);
				int32 NumPoleSegments = FMath::FloorToInt(TotalPoleHeight / PoleHeight) + OvershootCount + 1;

				// Add pole instances stacked vertically
				for (int32 i = 0; i < NumPoleSegments; i++)
				{
					double ZPos = static_cast<double>(i) * PoleHeight;
					FTransform PoleTransform;
					PoleTransform.SetLocation(FVector(0.0, 0.0, ZPos));
					PoleTransform.SetRotation(FQuat::Identity);
					PoleTransform.SetScale3D(FVector::OneVector);
					CachedPole->AddInstance(PoleTransform, false);
				}

				UE_LOG(LogTemp, Log, TEXT("[Ladder] Added %d pole instances (total now: %d)"),
					NumPoleSegments, CachedPole->GetInstanceCount());
			}
			else
			{
				// Fallback: single pole instance at origin
				FTransform PoleTransform;
				PoleTransform.SetLocation(FVector::ZeroVector);
				PoleTransform.SetRotation(FQuat::Identity);
				PoleTransform.SetScale3D(FVector::OneVector);
				CachedPole->AddInstance(PoleTransform, false);

				UE_LOG(LogTemp, Warning, TEXT("[Ladder] PoleHeight is 0, added single pole instance"));
			}
		}
	}

	// Add connection instances (wall brackets)
	if (CachedConnections && ConnectionCount > 0)
	{
		// Space connections evenly along the ladder height
		double ConnectionSpacing = LadderHeight / static_cast<double>(ConnectionCount + 1);

		for (int32 i = 1; i <= ConnectionCount; i++)
		{
			double ZPos = ConnectionSpacing * static_cast<double>(i);
			FTransform ConnectionTransform;
			ConnectionTransform.SetLocation(FVector(0.0, 0.0, ZPos));
			ConnectionTransform.SetRotation(FQuat::Identity);
			ConnectionTransform.SetScale3D(FVector::OneVector);
			CachedConnections->AddInstance(ConnectionTransform, false);  // false = local space
		}

		UE_LOG(LogTemp, Log, TEXT("[Ladder] Added %d connection instances"), ConnectionCount);
	}

	// Force render state update to ensure instances are visible
	if (CachedBarz)
	{
		CachedBarz->MarkRenderStateDirty();
	}
	if (CachedPole)
	{
		CachedPole->MarkRenderStateDirty();
	}
	if (CachedConnections)
	{
		CachedConnections->MarkRenderStateDirty();
	}

	// Position collision zones based on ladder height
	// The Blueprint SCS collision components need to be positioned dynamically

	// TopCollision - at the TOP of the ladder
	if (CachedTopCollision)
	{
		// Position at top of ladder, slightly forward
		double TopZ = LadderHeight + (OvershootCount * BarOffset) + TopCollisionOffsetZ;
		CachedTopCollision->SetRelativeLocation(FVector(TopCollisionOffsetX, 0.0, TopZ));

		// CRITICAL FIX: Force reasonable collision extent
		// Blueprint CDO may have tiny Z extent (2.9) that character passes through
		// Use at least 50 units in Z to reliably detect the character
		FVector ForcedExtent = TopCollisionExtent;
		if (ForcedExtent.Z < 50.0)
		{
			ForcedExtent.Z = 50.0;  // Minimum 50 units tall
		}
		if (ForcedExtent.X < 50.0)
		{
			ForcedExtent.X = 50.0;  // Minimum 50 units deep
		}
		CachedTopCollision->SetBoxExtent(ForcedExtent);

		UE_LOG(LogTemp, Log, TEXT("[Ladder] TopCollision positioned at Z=%.1f, Extent=(%.1f, %.1f, %.1f) [forced from %.1f, %.1f, %.1f]"),
			TopZ, ForcedExtent.X, ForcedExtent.Y, ForcedExtent.Z,
			TopCollisionExtent.X, TopCollisionExtent.Y, TopCollisionExtent.Z);
	}

	// BottomCollision - at the BOTTOM of the ladder
	if (CachedBottomCollision)
	{
		// Position at bottom, slightly forward
		CachedBottomCollision->SetRelativeLocation(FVector(BottomCollisionOffsetX, 0.0, BottomCollisionOffsetZ));

		// CRITICAL FIX: Force reasonable collision extent
		// Blueprint CDO may have tiny extents that character passes through
		FVector ForcedExtent = BottomCollisionExtent;
		if (ForcedExtent.Z < 50.0)
		{
			ForcedExtent.Z = 50.0;  // Minimum 50 units tall
		}
		if (ForcedExtent.X < 50.0)
		{
			ForcedExtent.X = 50.0;  // Minimum 50 units deep
		}
		CachedBottomCollision->SetBoxExtent(ForcedExtent);

		UE_LOG(LogTemp, Log, TEXT("[Ladder] BottomCollision positioned at Z=%.1f, Extent=(%.1f, %.1f, %.1f) [forced from %.1f, %.1f, %.1f]"),
			BottomCollisionOffsetZ, ForcedExtent.X, ForcedExtent.Y, ForcedExtent.Z,
			BottomCollisionExtent.X, BottomCollisionExtent.Y, BottomCollisionExtent.Z);
	}

	// TopdownCollision - above the ladder top (for approaching from above)
	if (CachedTopdownCollision)
	{
		double TopdownZ = LadderHeight + (OvershootCount * BarOffset) + TopdownCollisionOffsetZ;
		CachedTopdownCollision->SetRelativeLocation(FVector(TopdownCollisionOffsetX, 0.0, TopdownZ));
		CachedTopdownCollision->SetBoxExtent(TopdownCollisionExtent);
		UE_LOG(LogTemp, Log, TEXT("[Ladder] TopdownCollision positioned at Z=%.1f"), TopdownZ);
	}

	// ClimbingCollision - covers the entire climbable area
	if (CachedClimbingCollision)
	{
		// Don't override climbing collision - it's set correctly in Blueprint SCS
		// Just log its current position for debugging
		FVector Extent = CachedClimbingCollision->GetUnscaledBoxExtent();
		FVector Location = CachedClimbingCollision->GetRelativeLocation();
		UE_LOG(LogTemp, Log, TEXT("[Ladder] ClimbingCollision - Location: (%.1f, %.1f, %.1f), Extent: (%.1f, %.1f, %.1f)"),
			Location.X, Location.Y, Location.Z, Extent.X, Extent.Y, Extent.Z);
	}
}

void ASLFLadderBase::OnInteract_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[Ladder] OnInteract by %s"), Interactor ? *Interactor->GetName() : TEXT("None"));

	if (!Interactor)
	{
		return;
	}

	// GUARD: Check if already climbing - prevent re-mounting
	ULadderManagerComponent* LadderManager = Interactor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager && LadderManager->GetIsClimbing())
	{
		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnInteract BLOCKED - Already climbing (IsClimbing=true)"));
		return;
	}

	// Check if interacting actor implements BPI_GenericCharacter interface
	if (!Interactor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Ladder] Interactor does not implement BPI_GenericCharacter"));
		return;
	}

	// Calculate if this is a topdown approach (actor is above ladder midpoint)
	FVector ActorLocation = Interactor->GetActorLocation();
	FVector LadderTopLocation = GetActorLocation() + FVector(0.0, 0.0, LadderHeight);
	double DistanceToTop = FVector::Dist(ActorLocation, LadderTopLocation);

	// If actor is closer to top than to bottom (distance to top < half ladder height), it's topdown
	bool bIsTopdown = DistanceToTop < (LadderHeight * 0.5);

	UE_LOG(LogTemp, Log, TEXT("[Ladder] TryClimbLadder - IsTopdown: %s, DistanceToTop: %f"),
		bIsTopdown ? TEXT("true") : TEXT("false"), DistanceToTop);

	// Call TryClimbLadder on the interacting actor via interface
	IBPI_GenericCharacter::Execute_TryClimbLadder(Interactor, this, bIsTopdown);
}

// ═══════════════════════════════════════════════════════════════════════════════
// OVERLAP EVENT HANDLERS
// ═══════════════════════════════════════════════════════════════════════════════

void ASLFLadderBase::OnClimbingCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	// Get ladder manager from other actor
	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		// GUARD: Check if the character is currently climbing on THIS ladder
		// During mounting/orientation, the character may briefly exit the collision
		// Only reset climbing if they're not actively climbing on this ladder
		AActor* CurrentLadder = LadderManager->GetCurrentLadder();
		bool bIsClimbing = LadderManager->GetIsClimbing();

		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnClimbingCollisionEndOverlap - Actor: %s, IsClimbing: %s, CurrentLadder: %s, ThisLadder: %s"),
			*OtherActor->GetName(),
			bIsClimbing ? TEXT("true") : TEXT("false"),
			CurrentLadder ? *CurrentLadder->GetName() : TEXT("null"),
			*GetName());

		// If the character is currently climbing THIS ladder, don't reset
		// This prevents the premature reset during mounting/orientation
		if (bIsClimbing && CurrentLadder == this)
		{
			UE_LOG(LogTemp, Log, TEXT("[Ladder] OnClimbingCollisionEndOverlap - GUARD: Character is actively climbing this ladder, NOT resetting"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnClimbingCollisionEndOverlap - Resetting climbing state"));
		LadderManager->SetIsClimbing(false);
		LadderManager->SetCurrentLadder(nullptr);
	}
}

void ASLFLadderBase::OnBottomCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnBottomCollisionBeginOverlap - Setting IsOnGround=true, CurrentLadder=%s"), *GetName());
		LadderManager->SetIsOnGround(true);
		LadderManager->SetCurrentLadder(this);
	}
}

void ASLFLadderBase::OnBottomCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		// bp_only: Always clear the flag when exiting the zone
		// The zone check happens in input handling, not after animation
		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnBottomCollisionEndOverlap - Setting IsOnGround=false"));
		LadderManager->SetIsOnGround(false);
	}
}

void ASLFLadderBase::OnTopCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnTopCollisionBeginOverlap - Setting IsClimbingOffTop=true"));
		LadderManager->SetIsClimbingOffTop(true);
	}
}

void ASLFLadderBase::OnTopCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		// bp_only: Always clear the flag when exiting the zone
		// The zone check happens in input handling, not after animation
		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnTopCollisionEndOverlap - Setting IsClimbingOffTop=false"));
		LadderManager->SetIsClimbingOffTop(false);
	}
}

void ASLFLadderBase::OnTopdownCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Log, TEXT("[Ladder] OnTopdownCollisionEndOverlap - Setting IsClimbingDownFromTop=true, CurrentLadder=%s"), *GetName());
		LadderManager->SetIsClimbingDownFromTop(true);
		LadderManager->SetCurrentLadder(this);
	}
}
