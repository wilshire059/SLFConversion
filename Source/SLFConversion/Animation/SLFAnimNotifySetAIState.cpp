// SLFAnimNotifySetAIState.cpp
#include "SLFAnimNotifySetAIState.h"
#include "Components/AIBehaviorManagerComponent.h"

FString USLFAnimNotifySetAIState::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Set AI State: %s"), *NewState.ToString());
}

void USLFAnimNotifySetAIState::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_SetAIState] Setting state to %s"), *NewState.ToString());

	// Get AIBehaviorManagerComponent and set state
	if (UAIBehaviorManagerComponent* BehaviorManager = Owner->FindComponentByClass<UAIBehaviorManagerComponent>())
	{
		// Convert gameplay tag to ESLFAIStates enum
		FString TagName = NewState.GetTagName().ToString();
		ESLFAIStates State = ESLFAIStates::Idle;

		if (TagName.Contains(TEXT("Idle")))
		{
			State = ESLFAIStates::Idle;
		}
		else if (TagName.Contains(TEXT("RandomRoam")) || TagName.Contains(TEXT("Roam")))
		{
			State = ESLFAIStates::RandomRoam;
		}
		else if (TagName.Contains(TEXT("Patrol")))
		{
			State = ESLFAIStates::Patrolling;
		}
		else if (TagName.Contains(TEXT("Investigating")) || TagName.Contains(TEXT("Investigate")))
		{
			State = ESLFAIStates::Investigating;
		}
		else if (TagName.Contains(TEXT("Combat")))
		{
			State = ESLFAIStates::Combat;
		}
		else if (TagName.Contains(TEXT("PoiseBroken")) || TagName.Contains(TEXT("Poise")))
		{
			State = ESLFAIStates::PoiseBroken;
		}
		else if (TagName.Contains(TEXT("Uninterruptable")))
		{
			State = ESLFAIStates::Uninterruptable;
		}
		else if (TagName.Contains(TEXT("OutOfBounds")))
		{
			State = ESLFAIStates::OutOfBounds;
		}

		BehaviorManager->SetState(State);
	}
}
