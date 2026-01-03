// ANS_FistTrace.cpp
// C++ Animation Notify implementation for ANS_FistTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - enables hand/fist collision tracing for unarmed combat

#include "AnimNotifies/ANS_FistTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CombatManagerComponent.h"
#include "Components/AICombatManagerComponent.h"

UANS_FistTrace::UANS_FistTrace()
	: TraceType(ESLFTraceType::RightHand)
	, TraceThickness(2.0)
	, TraceColor(FLinearColor::Red)
	, TraceDuration(0.5)
	, Radius(10.0)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 100, 100, 255); // Light red for fist trace
#endif
}

void UANS_FistTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Enable hand trace on CombatManager
	// Default to right hand (true) - for left hand, use a separate notify or add variable

	// Try player CombatManager
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->ToggleHandTrace(true, true); // Enable, right hand
		UE_LOG(LogTemp, Log, TEXT("UANS_FistTrace::NotifyBegin - Hand trace ON (CombatManager) on %s"), *Owner->GetName());
	}

	// Try AI CombatManager
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->ToggleHandTrace(true, true); // Enable, right hand
		UE_LOG(LogTemp, Log, TEXT("UANS_FistTrace::NotifyBegin - Hand trace ON (AICombatManager) on %s"), *Owner->GetName());
	}
}

void UANS_FistTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic - hand trace tick handled by component
}

void UANS_FistTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Disable hand trace on CombatManager

	// Try player CombatManager
	UCombatManagerComponent* CombatManager = Owner->FindComponentByClass<UCombatManagerComponent>();
	if (CombatManager)
	{
		CombatManager->ToggleHandTrace(false, true); // Disable, right hand
		UE_LOG(LogTemp, Log, TEXT("UANS_FistTrace::NotifyEnd - Hand trace OFF (CombatManager) on %s"), *Owner->GetName());
	}

	// Try AI CombatManager
	UAICombatManagerComponent* AICombatManager = Owner->FindComponentByClass<UAICombatManagerComponent>();
	if (AICombatManager)
	{
		AICombatManager->ToggleHandTrace(false, true); // Disable, right hand
		UE_LOG(LogTemp, Log, TEXT("UANS_FistTrace::NotifyEnd - Hand trace OFF (AICombatManager) on %s"), *Owner->GetName());
	}
}

FString UANS_FistTrace::GetNotifyName_Implementation() const
{
	return TEXT("Fist Trace");
}
