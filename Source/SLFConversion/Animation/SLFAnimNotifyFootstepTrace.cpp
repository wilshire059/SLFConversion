// SLFAnimNotifyFootstepTrace.cpp
#include "SLFAnimNotifyFootstepTrace.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"

FString USLFAnimNotifyFootstepTrace::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("Footstep: %s"), *FootBoneName.ToString());
}

void USLFAnimNotifyFootstepTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	FVector FootLocation = MeshComp->GetSocketLocation(FootBoneName);

	UE_LOG(LogTemp, Log, TEXT("[AN_FootstepTrace] Footstep at %s"), *FootLocation.ToString());

	// Line trace down to detect surface
	FHitResult HitResult;
	FVector TraceStart = FootLocation;
	FVector TraceEnd = FootLocation - FVector(0.0f, 0.0f, 50.0f);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	bool bHit = UKismetSystemLibrary::LineTraceSingle(
		Owner->GetWorld(),
		TraceStart,
		TraceEnd,
		UEngineTypes::ConvertToTraceType(ECC_Visibility),
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true
	);

	if (bHit)
	{
		// Get physical material for surface type
		if (UPhysicalMaterial* PhysMat = HitResult.PhysMaterial.Get())
		{
			UE_LOG(LogTemp, Verbose, TEXT("[AN_FootstepTrace] Hit surface with PhysMat: %s"), *PhysMat->GetName());

			// Surface type can be used to determine footstep sound/effect
			// EPhysicalSurface SurfaceType = PhysMat->SurfaceType;
		}

		// Spawn footstep effect at hit location if configured
		// Effect spawning would be handled based on surface type
		UE_LOG(LogTemp, Verbose, TEXT("[AN_FootstepTrace] Footstep hit at: %s"), *HitResult.ImpactPoint.ToString());
	}
}
