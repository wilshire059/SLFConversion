// SLFActionGrapple.cpp
#include "SLFActionGrapple.h"
#include "Blueprints/Actors/SLFGrapplePoint.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "EngineUtils.h"
#include "Camera/PlayerCameraManager.h"

USLFActionGrapple::USLFActionGrapple()
{
}

ASLFGrapplePoint* USLFActionGrapple::FindBestGrapplePoint() const
{
	if (!OwnerActor) return nullptr;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return nullptr;

	UWorld* World = Character->GetWorld();
	if (!World) return nullptr;

	// Get camera forward direction for aiming
	FVector CameraLocation;
	FRotator CameraRotation;
	if (APlayerController* PC = Cast<APlayerController>(Character->GetController()))
	{
		if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
		{
			CameraLocation = CamMgr->GetCameraLocation();
			CameraRotation = CamMgr->GetCameraRotation();
		}
	}
	FVector AimDirection = CameraRotation.Vector();
	FVector CharLocation = Character->GetActorLocation();

	ASLFGrapplePoint* BestPoint = nullptr;
	float BestScore = -1.0f;

	for (TActorIterator<ASLFGrapplePoint> It(World); It; ++It)
	{
		ASLFGrapplePoint* Point = *It;
		if (!Point->bIsActive) continue;

		float Distance = FVector::Dist(CharLocation, Point->GetActorLocation());
		if (Distance > MaxGrappleRange) continue;

		// Score based on aim alignment and distance
		FVector ToPoint = (Point->GetActorLocation() - CharLocation).GetSafeNormal();
		float Dot = FVector::DotProduct(AimDirection, ToPoint);
		if (Dot < 0.5f) continue; // Must be roughly in front

		// Line-of-sight check
		FHitResult Hit;
		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActor(Character);
		if (World->LineTraceSingleByChannel(Hit, CharLocation, Point->GetActorLocation(),
			ECC_WorldStatic, QueryParams))
		{
			if (Hit.GetActor() != Point) continue; // Blocked
		}

		float Score = Dot * (1.0f - Distance / MaxGrappleRange);
		if (Score > BestScore)
		{
			BestScore = Score;
			BestPoint = Point;
		}
	}

	return BestPoint;
}

bool USLFActionGrapple::CanExecuteAction_Implementation()
{
	if (!OwnerActor || bIsGrappling || bOnCooldown) return false;
	return FindBestGrapplePoint() != nullptr;
}

void USLFActionGrapple::ExecuteAction_Implementation()
{
	if (!OwnerActor) return;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character) return;

	TargetPoint = FindBestGrapplePoint();
	if (!TargetPoint) return;

	bIsGrappling = true;
	UE_LOG(LogTemp, Log, TEXT("[ActionGrapple] Grappling to %s"), *TargetPoint->GetName());

	// Disable gravity during grapple
	Character->GetCharacterMovement()->GravityScale = 0.0f;
	Character->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

	// Play launch montage
	UAnimMontage* Montage = GrappleLaunchMontage.LoadSynchronous();
	if (Montage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0f, 0.0f, NAME_None);
	}

	// Start flight update on timer
	if (UWorld* World = Character->GetWorld())
	{
		World->GetTimerManager().SetTimer(FlightHandle, this,
			&USLFActionGrapple::UpdateGrappleFlight, 0.016f, true);
	}
}

void USLFActionGrapple::UpdateGrappleFlight()
{
	if (!OwnerActor || !TargetPoint)
	{
		OnGrappleArrival();
		return;
	}

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (!Character)
	{
		OnGrappleArrival();
		return;
	}

	FVector CurrentLocation = Character->GetActorLocation();
	FVector TargetLocation = TargetPoint->GetLandingLocation();
	float Distance = FVector::Dist(CurrentLocation, TargetLocation);

	if (Distance < 100.0f)
	{
		// Arrived
		OnGrappleArrival();
		return;
	}

	// Move toward target
	FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
	float DeltaTime = 0.016f;
	FVector NewLocation = CurrentLocation + Direction * GrappleSpeed * DeltaTime;
	Character->SetActorLocation(NewLocation);
}

void USLFActionGrapple::OnGrappleArrival()
{
	if (UWorld* World = OwnerActor ? OwnerActor->GetWorld() : nullptr)
	{
		World->GetTimerManager().ClearTimer(FlightHandle);
	}

	bIsGrappling = false;

	ACharacter* Character = Cast<ACharacter>(OwnerActor);
	if (Character)
	{
		// Restore normal movement
		Character->GetCharacterMovement()->GravityScale = 1.0f;
		Character->GetCharacterMovement()->SetMovementMode(MOVE_Falling);

		// Snap to landing position
		if (TargetPoint)
		{
			Character->SetActorLocation(TargetPoint->GetLandingLocation());
		}

		// Play land montage
		UAnimMontage* Montage = GrappleLandMontage.LoadSynchronous();
		if (Montage && OwnerActor->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
		{
			IBPI_GenericCharacter::Execute_PlayMontageReplicated(OwnerActor, Montage, 1.0f, 0.0f, NAME_None);
		}
	}

	TargetPoint = nullptr;

	// Start cooldown
	bOnCooldown = true;
	UE_LOG(LogTemp, Log, TEXT("[ActionGrapple] Arrived, cooldown %.1fs"), GrappleCooldown);

	if (OwnerActor)
	{
		if (UWorld* World = OwnerActor->GetWorld())
		{
			World->GetTimerManager().SetTimer(CooldownHandle, this,
				&USLFActionGrapple::ClearCooldown, GrappleCooldown, false);
		}
	}
}

void USLFActionGrapple::ClearCooldown()
{
	bOnCooldown = false;
}
