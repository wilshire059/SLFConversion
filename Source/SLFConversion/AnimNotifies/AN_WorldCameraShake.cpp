// AN_WorldCameraShake.cpp
// C++ Animation Notify implementation for AN_WorldCameraShake
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - plays world camera shake via BPI_GenericCharacter

#include "AnimNotifies/AN_WorldCameraShake.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Camera/CameraShakeBase.h"

UAN_WorldCameraShake::UAN_WorldCameraShake()
	: InnerRadius(0.0)
	, OuterRadius(1000.0)
	, Falloff(1.0)
	, OrientTowardsEpicenter(false)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(200, 100, 255, 255); // Purple for camera shake
#endif
}

void UAN_WorldCameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// Get epicenter location (actor location)
	FVector Epicenter = Owner->GetActorLocation();

	// From Blueprint: Call StartWorldCameraShake via BPI_GenericCharacter interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		TSubclassOf<UCameraShakeBase> ShakeClass = nullptr;
		if (CameraShake)
		{
			ShakeClass = TSubclassOf<UCameraShakeBase>(CameraShake);
		}

		IBPI_GenericCharacter::Execute_StartWorldCameraShake(
			Owner,
			ShakeClass,
			Epicenter,
			InnerRadius,
			OuterRadius,
			Falloff,
			OrientTowardsEpicenter
		);

		UE_LOG(LogTemp, Log, TEXT("UAN_WorldCameraShake::Notify - Started world camera shake at %s on %s"),
			*Epicenter.ToString(), *Owner->GetName());
	}
}

FString UAN_WorldCameraShake::GetNotifyName_Implementation() const
{
	return TEXT("World Camera Shake");
}
