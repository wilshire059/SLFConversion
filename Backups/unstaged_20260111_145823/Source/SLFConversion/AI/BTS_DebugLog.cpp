// BTS_DebugLog.cpp
// Diagnostic BTService that logs every tick to trace BT execution

#include "BTS_DebugLog.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UBTS_DebugLog::UBTS_DebugLog()
{
	NodeName = TEXT("Debug Log");
	ServiceName = TEXT("Unnamed");

	// Tick every 0.5 seconds
	Interval = 0.5f;
	RandomDeviation = 0.0f;
}

void UBTS_DebugLog::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AActor* Owner = OwnerComp.GetOwner();
	FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");

	UE_LOG(LogTemp, Warning, TEXT("[BTS_DebugLog] %s TICK on %s"), *ServiceName, *OwnerName);

	// Log blackboard state
	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		uint8 State = BB->GetValueAsEnum(FName("State"));
		bool bInCombat = BB->GetValueAsBool(FName("InCombat"));
		UObject* Target = BB->GetValueAsObject(FName("Target"));

		UE_LOG(LogTemp, Warning, TEXT("  [BB] State=%d, InCombat=%s, Target=%s"),
			State,
			bInCombat ? TEXT("TRUE") : TEXT("FALSE"),
			Target ? *Target->GetName() : TEXT("null"));
	}
}

FString UBTS_DebugLog::GetStaticDescription() const
{
	return FString::Printf(TEXT("Debug: %s"), *ServiceName);
}
