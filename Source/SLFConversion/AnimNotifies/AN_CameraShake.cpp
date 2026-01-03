// AN_CameraShake.cpp
// C++ Animation Notify implementation for AN_CameraShake
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - calls BPI_GenericCharacter::StartCameraShake

#include "AnimNotifies/AN_CameraShake.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

UAN_CameraShake::UAN_CameraShake()
	: Scale(1.0)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 150, 50, 255);
#endif
}

void UAN_CameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Call StartCameraShake(CameraShake, Scale) on owner via BPI_GenericCharacter interface
	// Will play the desired camera shake on the character
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_StartCameraShake(Owner, CameraShake, Scale);
		UE_LOG(LogTemp, Log, TEXT("UAN_CameraShake::Notify - Started camera shake on %s with scale %f"), *Owner->GetName(), Scale);
	}
}

FString UAN_CameraShake::GetNotifyName_Implementation() const
{
	return TEXT("Camera Shake");
}

