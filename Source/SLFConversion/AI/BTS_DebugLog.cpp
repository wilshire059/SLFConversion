// BTS_DebugLog.cpp
// Debug service that logs blackboard State key every tick

#include "BTS_DebugLog.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTNode.h"
#include "BehaviorTree/BehaviorTree.h"
#include "AIController.h"

UBTS_DebugLog::UBTS_DebugLog()
{
	NodeName = TEXT("Debug Log State");
	Interval = 0.5f; // Run every 0.5 seconds
	RandomDeviation = 0.0f;
}

void UBTS_DebugLog::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		UE_LOG(LogTemp, Error, TEXT("[BTS_DebugLog] NO BLACKBOARD"));
		return;
	}

	// Get owner name
	AAIController* AIC = OwnerComp.GetAIOwner();
	APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
	FString PawnName = Pawn ? Pawn->GetName() : TEXT("Unknown");

	// Get State as Int (should be 0 for Enum keys - diagnostic)
	int32 StateInt = BB->GetValueAsInt(FName("State"));

	// Get State as Enum (correct way for Enum keys)
	uint8 StateEnum = BB->GetValueAsEnum(FName("State"));

	// Get Target (corrected from TargetActor)
	UObject* Target = BB->GetValueAsObject(FName("Target"));

	// Get BT information
	FString TreeName = TEXT("None");
	FString ActiveNodeName = TEXT("None");
	FString ActiveNodeClass = TEXT("None");
	bool bIsRunning = false;
	bool bIsPaused = false;

	if (UBehaviorTree* Tree = OwnerComp.GetCurrentTree())
	{
		TreeName = Tree->GetName();
	}
	bIsRunning = OwnerComp.IsRunning();
	bIsPaused = OwnerComp.IsPaused();

	if (const UBTNode* ActiveNode = OwnerComp.GetActiveNode())
	{
		ActiveNodeName = ActiveNode->GetNodeName();
		ActiveNodeClass = ActiveNode->GetClass()->GetName();
	}

	UE_LOG(LogTemp, Warning, TEXT("[BTS_DebugLog] %s: State(Int)=%d, State(Enum)=%d, Target=%s | BT=%s, Running=%s, ActiveNode=%s (%s)"),
		*PawnName,
		StateInt,
		static_cast<int32>(StateEnum),
		Target ? *Target->GetName() : TEXT("null"),
		*TreeName,
		bIsRunning ? TEXT("YES") : TEXT("NO"),
		*ActiveNodeName,
		*ActiveNodeClass);
}
