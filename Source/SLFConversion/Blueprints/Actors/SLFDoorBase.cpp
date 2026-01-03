// SLFDoorBase.cpp
#include "SLFDoorBase.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "TimerManager.h"

ASLFDoorBase::ASLFDoorBase()
{
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;
	
	PrimaryActorTick.bCanEverTick = true;




	UE_LOG(LogTemp, Log, TEXT("[DoorBase] Created"));
}

void ASLFDoorBase::BeginPlay()
{
	Super::BeginPlay();
	if (DoorMesh)
	{
		InitialRotation = DoorMesh->GetRelativeRotation();
	}
	TargetRotation = InitialRotation;
	UE_LOG(LogTemp, Log, TEXT("[DoorBase] BeginPlay - Initial rotation: %s"), *InitialRotation.ToString());
}

void ASLFDoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if ((DoorState == ESLFDoorState::Opening || DoorState == ESLFDoorState::Closing) && DoorMesh)
	{
		FRotator CurrentRotation = DoorMesh->GetRelativeRotation();
		FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, OpenSpeed);
		DoorMesh->SetRelativeRotation(NewRotation);

		if (NewRotation.Equals(TargetRotation, 1.0f))
		{
			if (DoorState == ESLFDoorState::Opening)
			{
				DoorState = ESLFDoorState::Open;
				OnDoorStateChanged.Broadcast(true);
				UE_LOG(LogTemp, Log, TEXT("[DoorBase] Door fully open"));

				if (bAutoClose)
				{
					GetWorldTimerManager().SetTimer(AutoCloseTimerHandle, this, &ASLFDoorBase::HandleAutoClose, AutoCloseDelay, false);
				}
			}
			else
			{
				DoorState = ESLFDoorState::Closed;
				OnDoorStateChanged.Broadcast(false);
				UE_LOG(LogTemp, Log, TEXT("[DoorBase] Door fully closed"));
			}
		}
	}
}

void ASLFDoorBase::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Log, TEXT("[DoorBase] Interact by: %s"), Interactor ? *Interactor->GetName() : TEXT("None"));

	if (bIsLocked)
	{
		if (!TryUnlock(Interactor))
		{
			UE_LOG(LogTemp, Log, TEXT("[DoorBase] Door is locked"));
			return;
		}
	}

	ToggleDoor();
}

void ASLFDoorBase::OpenDoor_Implementation()
{
	if (DoorState == ESLFDoorState::Closed || DoorState == ESLFDoorState::Closing)
	{
		DoorState = ESLFDoorState::Opening;
		TargetRotation = InitialRotation + FRotator(0.0f, OpenAngle, 0.0f);
		GetWorldTimerManager().ClearTimer(AutoCloseTimerHandle);
		UE_LOG(LogTemp, Log, TEXT("[DoorBase] Opening door"));
	}
}

void ASLFDoorBase::CloseDoor_Implementation()
{
	if (DoorState == ESLFDoorState::Open || DoorState == ESLFDoorState::Opening)
	{
		DoorState = ESLFDoorState::Closing;
		TargetRotation = InitialRotation;
		UE_LOG(LogTemp, Log, TEXT("[DoorBase] Closing door"));
	}
}

void ASLFDoorBase::ToggleDoor_Implementation()
{
	if (DoorState == ESLFDoorState::Closed || DoorState == ESLFDoorState::Closing)
	{
		OpenDoor();
	}
	else
	{
		CloseDoor();
	}
}

bool ASLFDoorBase::TryUnlock_Implementation(AActor* Interactor)
{
	// Override in Blueprint to check for key item
	UE_LOG(LogTemp, Log, TEXT("[DoorBase] TryUnlock - RequiredKey: %s"), *RequiredKeyTag.ToString());
	return false;
}

void ASLFDoorBase::HandleAutoClose()
{
	CloseDoor();
}
