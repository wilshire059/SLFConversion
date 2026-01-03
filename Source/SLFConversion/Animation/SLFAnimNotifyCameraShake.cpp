// SLFAnimNotifyCameraShake.cpp
#include "SLFAnimNotifyCameraShake.h"
#include "GameFramework/PlayerController.h"

FString USLFAnimNotifyCameraShake::GetNotifyName_Implementation() const
{
	return TEXT("Camera Shake");
}

void USLFAnimNotifyCameraShake::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !ShakeClass) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	UE_LOG(LogTemp, Log, TEXT("[AN_CameraShake] Playing shake on %s"), *Owner->GetName());

	// Play camera shake on local player
	if (APlayerController* PC = Owner->GetWorld()->GetFirstPlayerController())
	{
		PC->ClientStartCameraShake(ShakeClass, Scale);
	}
}
