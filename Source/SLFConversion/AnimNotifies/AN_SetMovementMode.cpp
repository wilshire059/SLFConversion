// AN_SetMovementMode.cpp
// C++ Animation Notify implementation for AN_SetMovementMode
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - sets character movement mode

#include "AnimNotifies/AN_SetMovementMode.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"

UAN_SetMovementMode::UAN_SetMovementMode()
	: MovementMode(ESLFMovementType::Run) // Default to Run (400), not Walk (200)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(0, 150, 255, 255); // Blue for movement
#endif
}

void UAN_SetMovementMode::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// Call SetMovementMode via BPI_GenericCharacter interface
	if (Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass()))
	{
		IBPI_GenericCharacter::Execute_SetMovementMode(Owner, MovementMode);
		UE_LOG(LogTemp, Log, TEXT("UAN_SetMovementMode::Notify - Set movement mode %d on %s"),
			(int32)MovementMode, *Owner->GetName());
	}
}

FString UAN_SetMovementMode::GetNotifyName_Implementation() const
{
	return TEXT("Set Movement Mode");
}
