// ANS_Trail.cpp
// C++ Animation Notify implementation for ANS_Trail
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - activates/deactivates weapon trail VFX

#include "AnimNotifies/ANS_Trail.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"

UANS_Trail::UANS_Trail()
	: TraceType(ESLFTraceType::RightHand)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(255, 128, 0, 255); // Orange for trail
#endif
}

void UANS_Trail::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors (weapons) and activate their trail components
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		// Try Niagara trail component
		UNiagaraComponent* NiagaraTrail = AttachedActor->FindComponentByClass<UNiagaraComponent>();
		if (NiagaraTrail)
		{
			NiagaraTrail->SetActive(true);
			UE_LOG(LogTemp, Log, TEXT("UANS_Trail::NotifyBegin - Activated Niagara trail on %s"), *AttachedActor->GetName());
		}

		// Try Cascade particle trail component
		UParticleSystemComponent* ParticleTrail = AttachedActor->FindComponentByClass<UParticleSystemComponent>();
		if (ParticleTrail)
		{
			ParticleTrail->SetActive(true);
			UE_LOG(LogTemp, Log, TEXT("UANS_Trail::NotifyBegin - Activated Cascade trail on %s"), *AttachedActor->GetName());
		}
	}
}

void UANS_Trail::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	// No tick logic in Blueprint - trail runs continuously while active
}

void UANS_Trail::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get attached actors and deactivate their trail components
	TArray<AActor*> AttachedActors;
	Owner->GetAttachedActors(AttachedActors);

	for (AActor* AttachedActor : AttachedActors)
	{
		if (!AttachedActor)
		{
			continue;
		}

		// Try Niagara trail component
		UNiagaraComponent* NiagaraTrail = AttachedActor->FindComponentByClass<UNiagaraComponent>();
		if (NiagaraTrail)
		{
			NiagaraTrail->SetActive(false);
			UE_LOG(LogTemp, Log, TEXT("UANS_Trail::NotifyEnd - Deactivated Niagara trail on %s"), *AttachedActor->GetName());
		}

		// Try Cascade particle trail component
		UParticleSystemComponent* ParticleTrail = AttachedActor->FindComponentByClass<UParticleSystemComponent>();
		if (ParticleTrail)
		{
			ParticleTrail->SetActive(false);
			UE_LOG(LogTemp, Log, TEXT("UANS_Trail::NotifyEnd - Deactivated Cascade trail on %s"), *AttachedActor->GetName());
		}
	}
}

FString UANS_Trail::GetNotifyName_Implementation() const
{
	return TEXT("Weapon Trail");
}
