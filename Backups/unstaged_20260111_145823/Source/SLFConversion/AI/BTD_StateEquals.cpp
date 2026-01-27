// BTD_StateEquals.cpp
// Custom BTDecorator that checks ESLFAIStates and logs every evaluation

#include "BTD_StateEquals.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTD_StateEquals::UBTD_StateEquals()
{
	NodeName = TEXT("State Equals");
	RequiredState = ESLFAIStates::Idle;
	StateKeyName = FName("State");
	bEnableLogging = true;

	// Notify on blackboard changes
	bNotifyBecomeRelevant = true;
	bNotifyTick = false;
}

bool UBTD_StateEquals::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp)
	{
		if (bEnableLogging)
		{
			UE_LOG(LogTemp, Error, TEXT("[BTD_StateEquals] %s - NO BLACKBOARD!"), *NodeName);
		}
		return false;
	}

	// Get the enum value from blackboard
	uint8 CurrentStateValue = BlackboardComp->GetValueAsEnum(StateKeyName);
	ESLFAIStates CurrentState = static_cast<ESLFAIStates>(CurrentStateValue);

	// Compare
	bool bResult = (CurrentState == RequiredState);

	if (bEnableLogging)
	{
		AActor* Owner = OwnerComp.GetOwner();
		FString OwnerName = Owner ? Owner->GetName() : TEXT("Unknown");

		// Get state names for logging
		FString CurrentStateName = UEnum::GetValueAsString(CurrentState);
		FString RequiredStateName = UEnum::GetValueAsString(RequiredState);

		UE_LOG(LogTemp, Warning, TEXT("[BTD_StateEquals] %s on %s: CurrentState=%s (%d), RequiredState=%s (%d) => %s"),
			*NodeName,
			*OwnerName,
			*CurrentStateName,
			static_cast<int32>(CurrentState),
			*RequiredStateName,
			static_cast<int32>(RequiredState),
			bResult ? TEXT("PASS") : TEXT("FAIL"));

		// Also log other relevant blackboard values
		bool bInCombat = BlackboardComp->GetValueAsBool(FName("InCombat"));
		UObject* Target = BlackboardComp->GetValueAsObject(FName("Target"));

		UE_LOG(LogTemp, Warning, TEXT("  [BB Context] InCombat=%s, Target=%s"),
			bInCombat ? TEXT("TRUE") : TEXT("FALSE"),
			Target ? *Target->GetName() : TEXT("null"));
	}

	return bResult;
}

FString UBTD_StateEquals::GetStaticDescription() const
{
	FString StateName = UEnum::GetValueAsString(RequiredState);
	return FString::Printf(TEXT("State == %s"), *StateName);
}
