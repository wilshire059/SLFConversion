// AN_PlayCameraSequence.cpp
// C++ Animation Notify implementation for AN_PlayCameraSequence
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - plays a level sequence for camera effects

#include "AnimNotifies/AN_PlayCameraSequence.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_Player.h"
#include "LevelSequence.h"

UAN_PlayCameraSequence::UAN_PlayCameraSequence()
	: Sequence(nullptr)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(150, 50, 255, 255); // Purple for camera sequence
#endif
}

void UAN_PlayCameraSequence::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	if (!Sequence)
	{
		UE_LOG(LogTemp, Warning, TEXT("UAN_PlayCameraSequence::Notify - No Sequence set on %s"), *Owner->GetName());
		return;
	}

	// From Blueprint: Call PlayCameraSequence via BPI_Player interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_Player::StaticClass()))
	{
		IBPI_Player::Execute_PlayCameraSequence(Owner, Sequence, Settings);
		UE_LOG(LogTemp, Log, TEXT("UAN_PlayCameraSequence::Notify - Playing sequence %s on %s"),
			*Sequence->GetName(), *Owner->GetName());
	}
}

FString UAN_PlayCameraSequence::GetNotifyName_Implementation() const
{
	if (Sequence)
	{
		return FString::Printf(TEXT("Play %s"), *Sequence->GetName());
	}
	return TEXT("Play Camera Sequence");
}
