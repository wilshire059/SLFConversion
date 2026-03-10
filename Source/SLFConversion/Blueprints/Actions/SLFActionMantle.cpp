// SLFActionMantle.cpp
// Traversal action with height-based animation selection
#include "SLFActionMantle.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Components/CapsuleComponent.h"

USLFActionMantle::USLFActionMantle()
{
	VaultMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_Vault.AM_Vault")));
	HurdleMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_Hurdle.AM_Hurdle")));
	MantleMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_Mantle.AM_Mantle")));
	ClimbMontage = TSoftObjectPtr<UAnimMontage>(FSoftObjectPath(TEXT("/Game/GameAnimations/Montages/AM_Climb.AM_Climb")));
}

ESLFTraversalType USLFActionMantle::DetectLedge(FVector& OutLedgeLocation) const
{
	if (!OwnerActor) return ESLFTraversalType::None;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return ESLFTraversalType::None;

	UWorld* World = Character->GetWorld();
	if (!World) return ESLFTraversalType::None;

	FVector CharLocation = Character->GetActorLocation();
	FVector ForwardDir = Character->GetActorForwardVector();

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	// Forward trace at multiple heights to find the obstacle face
	FHitResult ForwardHit;
	bool bHitWall = false;

	// Try from waist height first, then lower
	for (float HeightOffset : {80.0f, 40.0f, 0.0f})
	{
		FVector TraceStart = CharLocation + FVector(0, 0, HeightOffset);
		FVector TraceEnd = TraceStart + ForwardDir * ForwardTraceDistance;

		if (World->LineTraceSingleByChannel(ForwardHit, TraceStart, TraceEnd, ECC_WorldStatic, QueryParams))
		{
			bHitWall = true;
			break;
		}
	}

	if (!bHitWall)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[ActionMantle] No wall found (ForwardTrace=%.0f)"), ForwardTraceDistance);
		return ESLFTraversalType::None;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionMantle] Wall hit at (%.0f,%.0f,%.0f) Actor:%s"),
		ForwardHit.ImpactPoint.X, ForwardHit.ImpactPoint.Y, ForwardHit.ImpactPoint.Z,
		*ForwardHit.GetActor()->GetName());

	// Downward trace from well above the obstacle to find the top
	FVector DownStart = ForwardHit.ImpactPoint + ForwardDir * 30.0f + FVector(0, 0, ClimbMaxHeight);
	FVector DownEnd = DownStart - FVector(0, 0, DownwardTraceDistance);

	FHitResult DownHit;
	if (!World->LineTraceSingleByChannel(DownHit, DownStart, DownEnd, ECC_WorldStatic, QueryParams))
	{
		return ESLFTraversalType::None;
	}

	float LedgeHeight = DownHit.ImpactPoint.Z - CharLocation.Z;

	if (LedgeHeight < MinTraversalHeight || LedgeHeight > ClimbMaxHeight)
	{
		UE_LOG(LogTemp, Log, TEXT("[ActionMantle] Ledge height %.0fcm outside range [%.0f-%.0f]"),
			LedgeHeight, MinTraversalHeight, ClimbMaxHeight);
		return ESLFTraversalType::None;
	}

	UE_LOG(LogTemp, Log, TEXT("[ActionMantle] Ledge detected: height=%.0fcm at (%.0f,%.0f,%.0f)"),
		LedgeHeight, DownHit.ImpactPoint.X, DownHit.ImpactPoint.Y, DownHit.ImpactPoint.Z);

	// Verify room to stand
	float CapsuleHalfHeight = Character->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float CapsuleRadius = Character->GetCapsuleComponent()->GetScaledCapsuleRadius();
	FVector StandingPos = DownHit.ImpactPoint + FVector(0, 0, CapsuleHalfHeight);

	FHitResult StandHit;
	if (World->SweepSingleByChannel(StandHit,
		StandingPos, StandingPos + FVector(0, 0, 1.0f),
		FQuat::Identity, ECC_WorldStatic,
		FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight),
		QueryParams))
	{
		return ESLFTraversalType::None;
	}

	OutLedgeLocation = StandingPos;

	// Determine traversal type by height
	if (LedgeHeight <= VaultMaxHeight)
	{
		return ESLFTraversalType::Vault;
	}
	else if (LedgeHeight <= HurdleMaxHeight)
	{
		return ESLFTraversalType::Hurdle;
	}
	else if (LedgeHeight <= MantleMaxHeight)
	{
		return ESLFTraversalType::Mantle;
	}
	else
	{
		return ESLFTraversalType::Climb;
	}
}

bool USLFActionMantle::CanExecuteAction_Implementation()
{
	if (!OwnerActor) return false;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return false;

	// Can mantle while airborne OR on ground (vaulting while running)
	FVector Ledge;
	return DetectLedge(Ledge) != ESLFTraversalType::None;
}

void USLFActionMantle::ExecuteAction_Implementation()
{
	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	FVector LedgeLocation;
	ESLFTraversalType TraversalType = DetectLedge(LedgeLocation);
	if (TraversalType == ESLFTraversalType::None) return;

	// Select montage based on traversal type
	TSoftObjectPtr<UAnimMontage>* SelectedMontagePtr = nullptr;
	const TCHAR* TypeName = TEXT("Unknown");

	switch (TraversalType)
	{
	case ESLFTraversalType::Vault:
		SelectedMontagePtr = &VaultMontage;
		TypeName = TEXT("Vault");
		break;
	case ESLFTraversalType::Hurdle:
		SelectedMontagePtr = &HurdleMontage;
		TypeName = TEXT("Hurdle");
		break;
	case ESLFTraversalType::Mantle:
		SelectedMontagePtr = &MantleMontage;
		TypeName = TEXT("Mantle");
		break;
	case ESLFTraversalType::Climb:
		SelectedMontagePtr = &ClimbMontage;
		TypeName = TEXT("Climb");
		break;
	default:
		return;
	}

	float Height = LedgeLocation.Z - Character->GetActorLocation().Z;
	UE_LOG(LogTemp, Log, TEXT("[ActionMantle] %s to (%.0f, %.0f, %.0f), height=%.0fcm"),
		TypeName, LedgeLocation.X, LedgeLocation.Y, LedgeLocation.Z, Height);

	// Play selected montage
	if (SelectedMontagePtr)
	{
		UAnimMontage* Montage = SelectedMontagePtr->LoadSynchronous();
		if (Montage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0f, 0.0f, NAME_None);
		}
	}

	// Move to ledge position
	Character->SetActorLocation(LedgeLocation);
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}
