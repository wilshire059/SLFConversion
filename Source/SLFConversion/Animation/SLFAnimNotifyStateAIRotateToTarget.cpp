// SLFAnimNotifyStateAIRotateToTarget.cpp
#include "SLFAnimNotifyStateAIRotateToTarget.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

FString USLFAnimNotifyStateAIRotateToTarget::GetNotifyName_Implementation() const
{
	return TEXT("AI Rotate To Target");
}

void USLFAnimNotifyStateAIRotateToTarget::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp) return;

	ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner());
	if (!Character) return;

	AAIController* AIController = Cast<AAIController>(Character->GetController());
	if (!AIController) return;

	// Get focus actor from blackboard
	UBlackboardComponent* Blackboard = AIController->GetBlackboardComponent();
	if (!Blackboard) return;

	AActor* FocusActor = Cast<AActor>(Blackboard->GetValueAsObject(TEXT("TargetActor")));
	if (!FocusActor) return;

	// Calculate target rotation toward focus actor
	FVector CharLocation = Character->GetActorLocation();
	FVector TargetLocation = FocusActor->GetActorLocation();
	FRotator CurrentRotation = Character->GetActorRotation();
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(CharLocation, TargetLocation);

	// Only rotate yaw
	TargetRotation.Pitch = CurrentRotation.Pitch;
	TargetRotation.Roll = CurrentRotation.Roll;

	// Lerp rotation toward target
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, FrameDeltaTime, RotationSpeed);
	Character->SetActorRotation(NewRotation);
}
