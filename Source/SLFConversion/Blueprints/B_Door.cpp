// B_Door.cpp
// C++ implementation for Blueprint B_Door
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_Door.json

#include "Blueprints/B_Door.h"
#include "Components/InventoryManagerComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

AB_Door::AB_Door()
{
	// ============================================================
	// COMPONENT SETUP
	// Creates fallback components for C++ instantiation.
	// BeginPlay will re-cache to Blueprint SCS components if they exist.
	// ============================================================

	MoveToDistance = 100.0;
	YawRotationAmount = 90.0;
	PlayRate = 1.0;
	OpenForwards = true;
	DoorInterpAlpha = 0.0f;

	// Create root component
	USceneComponent* DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(DefaultRoot);

	// Create door mesh component (the actual door that rotates)
	// Transforms match bp_only: Location Y=-75, Rotation Yaw=90
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Interactable SM"));
	DoorMesh->SetupAttachment(DefaultRoot);
	DoorMesh->SetRelativeLocation(FVector(0.0f, -75.0f, 0.0f));
	DoorMesh->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DoorMesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	DoorMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	// Load default door mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoor.SM_PrisonDoor"));
	if (DoorMeshFinder.Succeeded())
	{
		DoorMesh->SetStaticMesh(DoorMeshFinder.Object);
	}

	// Create door frame component (static, does NOT rotate)
	// Transforms match bp_only: Rotation Yaw=90, NO mesh (child blueprints add mesh)
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame"));
	DoorFrame->SetupAttachment(DefaultRoot);
	DoorFrame->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	DoorFrame->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
	DoorFrame->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// NOTE: No default mesh - child blueprints can set their own door frame mesh

	// Create MoveTo billboard
	// Transforms match bp_only: Location X=100, Z=60, Scale=0.5
	MoveTo = CreateDefaultSubobject<UBillboardComponent>(TEXT("MoveTo"));
	MoveTo->SetupAttachment(DefaultRoot);
	MoveTo->SetRelativeLocation(FVector(100.0f, 0.0f, 60.0f));
	MoveTo->SetRelativeScale3D(FVector(0.5f, 0.5f, 0.5f));
}

void AB_Door::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Apply DefaultDoorMesh if set (allows Blueprint CDO override)
	if (DoorMesh && !DefaultDoorMesh.IsNull())
	{
		if (UStaticMesh* MeshToApply = DefaultDoorMesh.LoadSynchronous())
		{
			DoorMesh->SetStaticMesh(MeshToApply);
		}
	}

	// Apply DefaultDoorFrameMesh if set
	if (DoorFrame && !DefaultDoorFrameMesh.IsNull())
	{
		if (UStaticMesh* MeshToApply = DefaultDoorFrameMesh.LoadSynchronous())
		{
			DoorFrame->SetStaticMesh(MeshToApply);
		}
	}
}

void AB_Door::BeginPlay()
{
	Super::BeginPlay();

	// Log component state for debugging
	UE_LOG(LogTemp, Log, TEXT("[B_Door] BeginPlay - %s"), *GetName());

	if (DoorMesh)
	{
		UE_LOG(LogTemp, Log, TEXT("[B_Door]   DoorMesh: %s, Mesh: %s, Rotation: %s"),
			*DoorMesh->GetName(),
			DoorMesh->GetStaticMesh() ? *DoorMesh->GetStaticMesh()->GetName() : TEXT("None"),
			*DoorMesh->GetRelativeRotation().ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Door]   DoorMesh is NULL!"));
	}

	if (DoorFrame)
	{
		UE_LOG(LogTemp, Log, TEXT("[B_Door]   DoorFrame: %s, Mesh: %s"),
			*DoorFrame->GetName(),
			DoorFrame->GetStaticMesh() ? *DoorFrame->GetStaticMesh()->GetName() : TEXT("None"));
	}
}

void AB_Door::OnInteract_Implementation(AActor* InteractingActor)
{
	if (!IsValid(InteractingActor))
	{
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[B_Door] OnInteract by: %s"), *InteractingActor->GetName());

	// Determine if player is in front of door (for direction)
	FVector DoorForward = GetActorForwardVector();
	FVector ToPlayer = InteractingActor->GetActorLocation() - GetActorLocation();
	ToPlayer.Normalize();

	// Dot product: positive = in front, negative = behind
	float DotProduct = FVector::DotProduct(DoorForward, ToPlayer);
	OpenForwards = (DotProduct >= 0.0f);

	// Select appropriate montage based on which side player is on
	if (OpenForwards)
	{
		SelectedMontage = InteractMontage_RH.LoadSynchronous();
	}
	else
	{
		SelectedMontage = InteractMontage_LH.LoadSynchronous();
	}

	// Check if door is locked
	if (LockInfo.bIsLocked)
	{
		// Validate unlock requirements
		bool bSuccess1, bSuccess2, bSuccess3, bSuccess4;
		ValidateUnlockRequirements(InteractingActor, bSuccess1, bSuccess2, bSuccess3, bSuccess4);

		if (!bSuccess1)
		{
			// Cannot unlock - show message or play locked sound
			UE_LOG(LogTemp, Warning, TEXT("[B_Door] Door is locked and player does not have required items"));
			return;
		}

		// Successfully unlocked - clear lock
		LockInfo.bIsLocked = false;
	}

	// Update MoveTo location based on player approach direction
	// The player will be teleported to this location after passing through
	if (MoveTo)
	{
		// Position MoveTo on the OTHER side of the door from where player is coming
		FVector TargetOffset = OpenForwards ?
			(DoorForward * MoveToDistance) :
			(-DoorForward * MoveToDistance);
		MoveTo->SetRelativeLocation(TargetOffset);
		UE_LOG(LogTemp, Log, TEXT("[B_Door] MoveTo position set to: %s"), *TargetOffset.ToString());
	}

	// CRITICAL: Call OpenDoor on the PLAYER via BPI_GenericCharacter interface
	// This triggers the player's door-opening montage and movement through the door
	if (InteractingActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("[B_Door] Calling OpenDoor on player: %s, Montage: %s"),
			*InteractingActor->GetName(),
			SelectedMontage ? *SelectedMontage->GetName() : TEXT("None"));
		IBPI_GenericCharacter::Execute_OpenDoor(InteractingActor, SelectedMontage, this);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Door] InteractingActor does not implement BPI_GenericCharacter interface"));
	}

	// Start the door animation (rotation)
	OpenDoor();
}

void AB_Door::ValidateUnlockRequirements_Implementation(AActor* Actor, bool& OutSuccess, bool& OutSuccess1, bool& OutSuccess2, bool& OutSuccess3)
{
	// Default to fail
	OutSuccess = false;
	OutSuccess1 = false;
	OutSuccess2 = false;
	OutSuccess3 = false;

	// If door is not locked, always succeed
	if (!LockInfo.bIsLocked)
	{
		OutSuccess = true;
		OutSuccess1 = true;
		OutSuccess2 = true;
		OutSuccess3 = true;
		return;
	}

	// If no required items, succeed
	if (LockInfo.RequiredItemAmount.Num() == 0)
	{
		OutSuccess = true;
		OutSuccess1 = true;
		OutSuccess2 = true;
		OutSuccess3 = true;
		return;
	}

	// Get inventory manager from actor
	APawn* Pawn = Cast<APawn>(Actor);
	if (!IsValid(Pawn))
	{
		return;
	}

	AController* Controller = Pawn->GetController();
	if (!IsValid(Controller))
	{
		return;
	}

	UInventoryManagerComponent* InventoryMgr = Controller->FindComponentByClass<UInventoryManagerComponent>();
	if (!IsValid(InventoryMgr))
	{
		return;
	}

	// Check all required items
	bool bCanUnlock = true;
	TArray<FGameplayTag> RequiredItemTags;
	LockInfo.RequiredItemAmount.GetKeys(RequiredItemTags);

	for (const FGameplayTag& ItemTag : RequiredItemTags)
	{
		int32 RequiredAmount = LockInfo.RequiredItemAmount[ItemTag];
		int32 PlayerAmount = 0;
		bool bFound = false;
		InventoryMgr->GetAmountOfItemWithTag(ItemTag, PlayerAmount, bFound);

		if (!bFound || PlayerAmount < RequiredAmount)
		{
			bCanUnlock = false;
			break;
		}
	}

	OutSuccess = bCanUnlock;
	OutSuccess1 = bCanUnlock;
	OutSuccess2 = bCanUnlock;
	OutSuccess3 = bCanUnlock;
}

FText AB_Door::GetRequiredItemsParsed_Implementation()
{
	// Return a formatted text of required items for unlocking
	if (!LockInfo.bIsLocked)
	{
		return FText::GetEmpty();
	}

	// Build text from required items
	if (LockInfo.RequiredItemAmount.Num() > 0)
	{
		FString Result;
		bool bFirst = true;

		TArray<FGameplayTag> RequiredItemTags;
		LockInfo.RequiredItemAmount.GetKeys(RequiredItemTags);

		for (const FGameplayTag& ItemTag : RequiredItemTags)
		{
			if (!bFirst) Result += TEXT(", ");
			int32 RequiredAmount = LockInfo.RequiredItemAmount[ItemTag];
			// Display the tag name as item name
			FString ItemName = ItemTag.IsValid() ? ItemTag.ToString() : TEXT("Unknown");
			Result += FString::Printf(TEXT("%s x%d"), *ItemName, RequiredAmount);
			bFirst = false;
		}
		return FText::FromString(Result);
	}

	return FText::FromString(TEXT("This door is locked."));
}

void AB_Door::OpenDoor()
{
	// Cache COMPONENT's relative rotation (not actor rotation!)
	// This is critical - only the door mesh rotates, not the frame
	if (DoorMesh)
	{
		Cache_Rotation = DoorMesh->GetRelativeRotation();
	}
	else
	{
		Cache_Rotation = FRotator::ZeroRotator;
	}

	// Calculate target rotation based on direction
	float YawDirection = OpenForwards ? YawRotationAmount : -YawRotationAmount;
	TargetRotation = Cache_Rotation;
	TargetRotation.Yaw += YawDirection;

	UE_LOG(LogTemp, Log, TEXT("[B_Door] OpenDoor - From: %s To: %s, Direction: %s"),
		*Cache_Rotation.ToString(), *TargetRotation.ToString(),
		OpenForwards ? TEXT("Forwards") : TEXT("Backwards"));

	// Reset interpolation
	DoorInterpAlpha = 0.0f;

	// Start interpolation timer (tick every frame at ~60fps)
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DoorInterpTimerHandle,
			this,
			&AB_Door::DoorInterpTick,
			0.016f, // ~60fps
			true    // looping
		);
	}
}

void AB_Door::DoorInterpTick()
{
	// Safety check - DoorMesh must exist
	if (!DoorMesh)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(DoorInterpTimerHandle);
		}
		return;
	}

	// Increment alpha based on play rate
	DoorInterpAlpha += 0.016f * PlayRate;

	if (DoorInterpAlpha >= 1.0f)
	{
		// Finished interpolation
		DoorInterpAlpha = 1.0f;

		// Set COMPONENT rotation (only door mesh rotates, not the frame!)
		DoorMesh->SetRelativeRotation(TargetRotation);

		// Stop timer
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(DoorInterpTimerHandle);
		}

		// Broadcast door opened
		OnDoorOpened.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("[B_Door] Door finished opening at rotation: %s"), *TargetRotation.ToString());
	}
	else
	{
		// Interpolate rotation - only the door mesh rotates
		FRotator NewRotation = UKismetMathLibrary::RLerp(Cache_Rotation, TargetRotation, DoorInterpAlpha, true);
		DoorMesh->SetRelativeRotation(NewRotation);
	}
}
