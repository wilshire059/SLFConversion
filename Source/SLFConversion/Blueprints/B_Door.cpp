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
	MoveToDistance = 100.0;
	YawRotationAmount = 90.0;
	PlayRate = 1.0;
	OpenForwards = true;
	DoorInterpAlpha = 0.0f;

	// ============================================================
	// CREATE DOOR FRAME COMPONENT
	// Decorative arch/frame around the door opening
	// Uses SM_PrisonDoorArch by default (can be changed per Blueprint/instance)
	// ============================================================
	DoorFrame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Door Frame"));
	DoorFrame->SetupAttachment(RootComponent);
	DoorFrame->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	DoorFrame->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f)); // Rotated to face forward
	DoorFrame->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Load default door frame mesh (SM_PrisonDoorArch)
	static ConstructorHelpers::FObjectFinder<UStaticMesh> DoorFrameMeshFinder(
		TEXT("/Game/SoulslikeFramework/Meshes/SM/PrisonDoor/SM_PrisonDoorArch.SM_PrisonDoorArch"));
	if (DoorFrameMeshFinder.Succeeded())
	{
		DoorFrame->SetStaticMesh(DoorFrameMeshFinder.Object);
		UE_LOG(LogTemp, Log, TEXT("[B_Door] Door Frame mesh set to SM_PrisonDoorArch"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_Door] Could not find SM_PrisonDoorArch mesh"));
	}

	// Create MoveTo component - marks where player teleports to after passing through door
	MoveTo = CreateDefaultSubobject<UBillboardComponent>(TEXT("MoveTo"));
	MoveTo->SetupAttachment(RootComponent);
	MoveTo->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f)); // Position updated in OnInteract based on direction
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
	// Cache current rotation
	Cache_Rotation = GetActorRotation();

	// Calculate target rotation based on direction
	float YawDirection = OpenForwards ? YawRotationAmount : -YawRotationAmount;
	TargetRotation = Cache_Rotation;
	TargetRotation.Yaw += YawDirection;

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
	// Increment alpha based on play rate
	DoorInterpAlpha += 0.016f * PlayRate;

	if (DoorInterpAlpha >= 1.0f)
	{
		// Finished interpolation
		DoorInterpAlpha = 1.0f;
		SetActorRotation(TargetRotation);

		// Stop timer
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().ClearTimer(DoorInterpTimerHandle);
		}

		// Broadcast door opened
		OnDoorOpened.Broadcast();
	}
	else
	{
		// Interpolate rotation
		FRotator NewRotation = UKismetMathLibrary::RLerp(Cache_Rotation, TargetRotation, DoorInterpAlpha, true);
		SetActorRotation(NewRotation);
	}
}
