// ANS_AI_FistTrace.cpp
// C++ Animation Notify implementation for ANS_AI_FistTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables AI hand/fist collision tracing for unarmed combat

#include "AnimNotifies/ANS_AI_FistTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AICombatManagerComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UANS_AI_FistTrace::UANS_AI_FistTrace()
	: TraceType(ESLFTraceType::RightHand)
	, DebugTraceThickness(2.0)
	, DebugTraceColor(FLinearColor::Red)
	, DebugTraceDuration(0.5)
	, DebugRadius(10.0)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 100, 100, 255); // Light red for fist trace
#endif
}

void UANS_AI_FistTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get AI_CombatManager and enable hand trace
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->ToggleHandTrace(true, true); // Enable, right hand
		UE_LOG(LogTemp, Log, TEXT("UANS_AI_FistTrace::NotifyBegin - Hand trace ON on %s"), *Owner->GetName());
	}
}

void UANS_AI_FistTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

#if WITH_EDITOR
	// From Blueprint: Draw debug spheres at socket locations in editor
	AActor* Owner = MeshComp->GetOwner();
	UWorld* World = Owner->GetWorld();
	if (World && World->IsEditorWorld())
	{
		// Draw debug sphere at hand socket
		FVector RightHandLoc = MeshComp->GetSocketLocation(FName("hand_r"));
		FVector LeftHandLoc = MeshComp->GetSocketLocation(FName("hand_l"));

		UKismetSystemLibrary::DrawDebugSphere(World, RightHandLoc, DebugRadius, 12, DebugTraceColor, DebugTraceDuration, DebugTraceThickness);
		UKismetSystemLibrary::DrawDebugSphere(World, LeftHandLoc, DebugRadius, 12, DebugTraceColor, DebugTraceDuration, DebugTraceThickness);
	}
#endif
}

void UANS_AI_FistTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get AI_CombatManager and disable hand trace
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->ToggleHandTrace(false, true); // Disable, right hand
		UE_LOG(LogTemp, Log, TEXT("UANS_AI_FistTrace::NotifyEnd - Hand trace OFF on %s"), *Owner->GetName());
	}
}

FString UANS_AI_FistTrace::GetNotifyName_Implementation() const
{
	return TEXT("AI Fist Trace");
}
