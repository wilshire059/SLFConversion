// ANS_InputBuffer.cpp
// C++ Animation Notify implementation for ANS_InputBuffer
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - toggles input buffer open/closed

#include "AnimNotifies/ANS_InputBuffer.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/InputBufferComponent.h"

UANS_InputBuffer::UANS_InputBuffer()
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(0, 200, 255, 255); // Cyan for input buffer
#endif
}

void UANS_InputBuffer::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get InputBuffer component and toggle buffer open
	// Buffer open = true means inputs during animation are queued for later
	UInputBufferComponent* InputBuffer = Owner->FindComponentByClass<UInputBufferComponent>();
	if (InputBuffer)
	{
		InputBuffer->ToggleBuffer(true);
		UE_LOG(LogTemp, Log, TEXT("UANS_InputBuffer::NotifyBegin - Buffer OPEN on %s"), *Owner->GetName());
	}
}

void UANS_InputBuffer::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic in Blueprint - buffer state managed by component
}

void UANS_InputBuffer::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get InputBuffer component and toggle buffer closed
	// Buffer closed = false means queued inputs are now processed
	UInputBufferComponent* InputBuffer = Owner->FindComponentByClass<UInputBufferComponent>();
	if (InputBuffer)
	{
		InputBuffer->ToggleBuffer(false);
		UE_LOG(LogTemp, Log, TEXT("UANS_InputBuffer::NotifyEnd - Buffer CLOSED on %s"), *Owner->GetName());
	}
}

FString UANS_InputBuffer::GetNotifyName_Implementation() const
{
	return TEXT("Input Buffer");
}
