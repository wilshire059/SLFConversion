// B_Ladder.cpp
// C++ implementation for Blueprint B_Ladder
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/B_Ladder.json
// Logic traced from JSON node-by-node

#include "Blueprints/B_Ladder.h"
#include "Engine/StaticMesh.h"
#include "Components/LadderManagerComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

AB_Ladder::AB_Ladder()
{
	// Initialize default values from Blueprint
	LadderHeight = 500.0;
	BarOffset = 50.0;
	OvershootCount = 2;
	ConnectionCount = 3;
	bBarAtZero = true;
	bAddFinishBar = true;
	TopCollisionExtent = FVector(50.0, 50.0, 50.0);
	TopCollisionOffsetX = 0.0;
	TopCollisionOffsetZ = 0.0;
	BottomCollisionExtent = FVector(50.0, 50.0, 50.0);
	BottomCollisionOffsetX = 0.0;
	BottomCollisionOffsetZ = 0.0;
	TopdownCollisionExtent = FVector(50.0, 50.0, 50.0);
	TopdownCollisionOffsetX = 0.0;
	TopdownCollisionOffsetZ = 0.0;
	Cache_LastBarIndex = 0;

	// Create components
	Barz = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Barz"));
	Pole = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Pole"));
	Connections = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Connections"));
	ClimbingCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ClimbingCollision"));
	BottomCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BottomCollision"));
	TopCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TopCollision"));
	TopdownCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("TopdownCollision"));

	// Attach components to root
	if (RootComponent)
	{
		Barz->SetupAttachment(RootComponent);
		Pole->SetupAttachment(RootComponent);
		Connections->SetupAttachment(RootComponent);
		ClimbingCollision->SetupAttachment(RootComponent);
		BottomCollision->SetupAttachment(RootComponent);
		TopCollision->SetupAttachment(RootComponent);
		TopdownCollision->SetupAttachment(RootComponent);
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::Constructor - Components created"));
}

void AB_Ladder::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Blueprint calls CreateLadder in construction script
	CreateLadder();
}

double AB_Ladder::GetPoleHeight_Implementation()
{
	// Logic from JSON GetPoleHeight graph:
	// 1. Get Pole component
	// 2. Get StaticMesh from Pole
	// 3. Call GetBounds() on StaticMesh
	// 4. Break BoxSphereBounds to get BoxExtent_Z
	// 5. Multiply by 2.0 (extent is half-size, we need full height)
	// 6. Return result

	if (Pole && Pole->GetStaticMesh())
	{
		FBoxSphereBounds Bounds = Pole->GetStaticMesh()->GetBounds();
		double PoleZ = Bounds.BoxExtent.Z * 2.0;

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::GetPoleHeight - PoleZ: %f"), PoleZ);
		return PoleZ;
	}

	UE_LOG(LogTemp, Warning, TEXT("AB_Ladder::GetPoleHeight - Pole or StaticMesh is null, returning 0"));
	return 0.0;
}

void AB_Ladder::CreateLadder_Implementation()
{
	// Logic from JSON CreateLadder graph:
	// Sequence of operations:
	// 1. Clear all ISM instances (Barz, Pole, Connections)
	// 2. Calculate pole height and bar count
	// 3. For loop to add bar instances
	// 4. For loop to add pole instances
	// 5. For loop to add connection instances
	// 6. Set collision box extents and positions

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Starting ladder creation"));

	// Step 1: Clear existing instances
	if (Barz)
	{
		Barz->ClearInstances();
	}
	if (Pole)
	{
		Pole->ClearInstances();
	}
	if (Connections)
	{
		Connections->ClearInstances();
	}

	// Step 2: Calculate pole height
	double PoleHeight = GetPoleHeight();
	if (PoleHeight <= 0.0)
	{
		// Fallback: Use a default pole segment height if mesh isn't set
		// This ensures ladder bars are still created even without a pole mesh
		PoleHeight = LadderHeight;
		UE_LOG(LogTemp, Warning, TEXT("AB_Ladder::CreateLadder - PoleHeight is 0 (no mesh?), using LadderHeight=%f as fallback"), LadderHeight);
	}

	// Calculate number of bars based on ladder height and bar offset
	int32 BarCount = 0;
	if (BarOffset > 0.0)
	{
		BarCount = FMath::FloorToInt(LadderHeight / BarOffset);
	}

	// Add overshoot bars
	int32 TotalBars = BarCount + OvershootCount;

	// Store last bar index for caching
	Cache_LastBarIndex = TotalBars;

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - PoleHeight: %f, BarCount: %d, TotalBars: %d"),
		PoleHeight, BarCount, TotalBars);

	// Step 3: Add bar instances
	if (Barz)
	{
		int32 StartIndex = bBarAtZero ? 0 : 1;
		int32 EndIndex = TotalBars;

		// Add finish bar if enabled
		if (bAddFinishBar)
		{
			EndIndex++;
		}

		for (int32 i = StartIndex; i <= EndIndex; i++)
		{
			double BarZ = static_cast<double>(i) * BarOffset;
			FTransform BarTransform;
			BarTransform.SetLocation(FVector(0.0, 0.0, BarZ));
			BarTransform.SetRotation(FQuat::Identity);
			BarTransform.SetScale3D(FVector::OneVector);

			Barz->AddInstance(BarTransform, true);
		}

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Added %d bar instances"), EndIndex - StartIndex + 1);
	}

	// Step 4: Add pole instances
	// Calculate how many poles needed based on ladder height and pole height
	if (Pole && PoleHeight > 0.0)
	{
		double LastPoleZ = 0.0;
		int32 PoleCount = FMath::CeilToInt(LadderHeight / PoleHeight);

		for (int32 i = 0; i < PoleCount; i++)
		{
			FTransform PoleTransform;
			PoleTransform.SetLocation(FVector(0.0, 0.0, LastPoleZ));
			PoleTransform.SetRotation(FQuat::Identity);
			PoleTransform.SetScale3D(FVector::OneVector);

			Pole->AddInstance(PoleTransform, true);
			LastPoleZ += PoleHeight;
		}

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Added %d pole instances"), PoleCount);
	}

	// Step 5: Add connection instances
	if (Connections && ConnectionCount > 0)
	{
		double ConnectionSpacing = LadderHeight / static_cast<double>(ConnectionCount + 1);

		for (int32 i = 1; i <= ConnectionCount; i++)
		{
			double ConnectionZ = static_cast<double>(i) * ConnectionSpacing;
			FTransform ConnectionTransform;
			ConnectionTransform.SetLocation(FVector(0.0, 0.0, ConnectionZ));
			ConnectionTransform.SetRotation(FQuat::Identity);
			ConnectionTransform.SetScale3D(FVector::OneVector);

			Connections->AddInstance(ConnectionTransform, true);
		}

		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Added %d connection instances"), ConnectionCount);
	}

	// Step 6: Set collision box positions and extents
	// Climbing collision - spans the full ladder height
	if (ClimbingCollision)
	{
		ClimbingCollision->SetBoxExtent(FVector(50.0, 50.0, LadderHeight / 2.0));
		ClimbingCollision->SetRelativeLocation(FVector(0.0, 0.0, LadderHeight / 2.0));
	}

	// Bottom collision
	if (BottomCollision)
	{
		BottomCollision->SetBoxExtent(BottomCollisionExtent);
		BottomCollision->SetRelativeLocation(FVector(BottomCollisionOffsetX, 0.0, BottomCollisionOffsetZ));
	}

	// Top collision
	if (TopCollision)
	{
		TopCollision->SetBoxExtent(TopCollisionExtent);
		TopCollision->SetRelativeLocation(FVector(TopCollisionOffsetX, 0.0, LadderHeight + TopCollisionOffsetZ));
	}

	// Topdown collision
	if (TopdownCollision)
	{
		TopdownCollision->SetBoxExtent(TopdownCollisionExtent);
		TopdownCollision->SetRelativeLocation(FVector(TopdownCollisionOffsetX, 0.0, LadderHeight + TopdownCollisionOffsetZ));
	}

	UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::CreateLadder - Ladder creation complete"));
}

void AB_Ladder::BeginPlay()
{
	Super::BeginPlay();

	// Log mesh status for visibility debugging
	UE_LOG(LogTemp, Log, TEXT("[B_Ladder] BeginPlay - LadderHeight: %.2f, BarOffset: %.2f"), LadderHeight, BarOffset);

	if (Barz)
	{
		UStaticMesh* BarzMesh = Barz->GetStaticMesh();
		UE_LOG(LogTemp, Log, TEXT("[B_Ladder] Barz mesh: %s, InstanceCount: %d"),
			BarzMesh ? *BarzMesh->GetName() : TEXT("NULL - BARS INVISIBLE!"),
			Barz->GetInstanceCount());
	}
	if (Pole)
	{
		UStaticMesh* PoleMesh = Pole->GetStaticMesh();
		UE_LOG(LogTemp, Log, TEXT("[B_Ladder] Pole mesh: %s, InstanceCount: %d"),
			PoleMesh ? *PoleMesh->GetName() : TEXT("NULL - POLES INVISIBLE!"),
			Pole->GetInstanceCount());
	}
	if (ClimbingCollision)
	{
		FVector Extent = ClimbingCollision->GetUnscaledBoxExtent();
		FVector Location = ClimbingCollision->GetRelativeLocation();
		UE_LOG(LogTemp, Log, TEXT("[B_Ladder] ClimbingCollision - Extent: (%.1f, %.1f, %.1f), Location: (%.1f, %.1f, %.1f)"),
			Extent.X, Extent.Y, Extent.Z, Location.X, Location.Y, Location.Z);
	}

	// Bind overlap events for ladder collision zones
	if (ClimbingCollision)
	{
		ClimbingCollision->OnComponentEndOverlap.AddDynamic(this, &AB_Ladder::OnClimbingCollisionEndOverlap);
	}

	if (BottomCollision)
	{
		BottomCollision->OnComponentBeginOverlap.AddDynamic(this, &AB_Ladder::OnBottomCollisionBeginOverlap);
		BottomCollision->OnComponentEndOverlap.AddDynamic(this, &AB_Ladder::OnBottomCollisionEndOverlap);
	}

	if (TopCollision)
	{
		TopCollision->OnComponentBeginOverlap.AddDynamic(this, &AB_Ladder::OnTopCollisionBeginOverlap);
		TopCollision->OnComponentEndOverlap.AddDynamic(this, &AB_Ladder::OnTopCollisionEndOverlap);
	}

	if (TopdownCollision)
	{
		TopdownCollision->OnComponentEndOverlap.AddDynamic(this, &AB_Ladder::OnTopdownCollisionEndOverlap);
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Ladder] BeginPlay - Overlap events bound"));
}

// ═══════════════════════════════════════════════════════════════════════════════
// INTERFACE IMPLEMENTATION
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Ladder::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("[B_Ladder] OnInteract by %s"), InteractingActor ? *InteractingActor->GetName() : TEXT("None"));

	if (!InteractingActor)
	{
		return;
	}

	// Check if interacting actor implements BPI_GenericCharacter interface
	if (!InteractingActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Ladder] InteractingActor does not implement BPI_GenericCharacter"));
		return;
	}

	// Calculate if this is a topdown approach (actor is above ladder midpoint)
	// Blueprint compares distance from actor to top vs distance threshold
	FVector ActorLocation = InteractingActor->GetActorLocation();
	FVector LadderTopLocation = GetActorLocation() + FVector(0.0, 0.0, LadderHeight);
	double DistanceToTop = FVector::Dist(ActorLocation, LadderTopLocation);

	// If actor is closer to top than to bottom (distance to top < half ladder height), it's topdown
	bool bIsTopdown = DistanceToTop < (LadderHeight * 0.5);

	UE_LOG(LogTemp, Log, TEXT("[B_Ladder] TryClimbLadder - IsTopdown: %s, DistanceToTop: %f"),
		bIsTopdown ? TEXT("true") : TEXT("false"), DistanceToTop);

	// Call TryClimbLadder on the interacting actor via interface
	// Note: IBPI_GenericCharacter::TryClimbLadder takes AActor* for the ladder parameter
	IBPI_GenericCharacter::Execute_TryClimbLadder(InteractingActor, this, bIsTopdown);
}

// ═══════════════════════════════════════════════════════════════════════════════
// OVERLAP EVENT HANDLERS
// ═══════════════════════════════════════════════════════════════════════════════

void AB_Ladder::OnClimbingCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
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

		UE_LOG(LogTemp, Log, TEXT("[B_Ladder] OnClimbingCollisionEndOverlap - Actor: %s, IsClimbing: %s, CurrentLadder: %s, ThisLadder: %s"),
			*OtherActor->GetName(),
			bIsClimbing ? TEXT("true") : TEXT("false"),
			CurrentLadder ? *CurrentLadder->GetName() : TEXT("null"),
			*GetName());

		// If the character is currently climbing THIS ladder, don't reset
		// This prevents the premature reset during mounting/orientation
		if (bIsClimbing && CurrentLadder == this)
		{
			UE_LOG(LogTemp, Log, TEXT("[B_Ladder] OnClimbingCollisionEndOverlap - GUARD: Character is actively climbing this ladder, NOT resetting"));
			return;
		}

		UE_LOG(LogTemp, Log, TEXT("[B_Ladder] OnClimbingCollisionEndOverlap - Resetting climbing state"));
		LadderManager->SetIsClimbing(false);
		LadderManager->SetCurrentLadder(nullptr);
	}
}

void AB_Ladder::OnBottomCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::OnBottomCollisionBeginOverlap - Setting IsOnGround=true, CurrentLadder=%s"), *GetName());
		LadderManager->SetIsOnGround(true);
		LadderManager->SetCurrentLadder(this);
	}
}

void AB_Ladder::OnBottomCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::OnBottomCollisionEndOverlap - Setting IsOnGround=false"));
		LadderManager->SetIsOnGround(false);
	}
}

void AB_Ladder::OnTopCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::OnTopCollisionBeginOverlap - Setting IsClimbingOffTop=true"));
		LadderManager->SetIsClimbingOffTop(true);
	}
}

void AB_Ladder::OnTopCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::OnTopCollisionEndOverlap - Setting IsClimbingOffTop=false"));
		LadderManager->SetIsClimbingOffTop(false);
	}
}

void AB_Ladder::OnTopdownCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor)
	{
		return;
	}

	ULadderManagerComponent* LadderManager = OtherActor->FindComponentByClass<ULadderManagerComponent>();
	if (LadderManager)
	{
		UE_LOG(LogTemp, Verbose, TEXT("AB_Ladder::OnTopdownCollisionEndOverlap - Setting IsClimbingDownFromTop=true, CurrentLadder=%s"), *GetName());
		LadderManager->SetIsClimbingDownFromTop(true);
		LadderManager->SetCurrentLadder(this);
	}
}
