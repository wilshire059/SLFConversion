// AN_FootstepTrace.cpp
// C++ Animation Notify implementation for AN_FootstepTrace
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Logic migrated from JSON export - traces for surface, plays sound/VFX

#include "AnimNotifies/AN_FootstepTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interfaces/BPI_GenericCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

UAN_FootstepTrace::UAN_FootstepTrace()
	: TraceLength(50.0f)
	, DebugTrace(EDrawDebugTrace::None)
{
#if WITH_EDITORONLY_DATA
	NotifyColor = FColor(150, 100, 50, 255);
#endif
}

void UAN_FootstepTrace::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();

	// From Blueprint: Get socket location and trace downward
	FVector SocketLocation = MeshComp->GetSocketLocation(FootSocket);
	FVector TraceStart = SocketLocation;
	FVector TraceEnd = SocketLocation - FVector(0.0f, 0.0f, TraceLength);

	// Line trace for objects
	FHitResult HitResult;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Owner);

	bool bHit = UKismetSystemLibrary::LineTraceSingleForObjects(
		Owner,
		TraceStart,
		TraceEnd,
		TraceType,
		false, // bTraceComplex
		ActorsToIgnore,
		DebugTrace,
		HitResult,
		true // bIgnoreSelf
	);

	if (!bHit)
	{
		return;
	}

	// Get physical surface type from hit
	EPhysicalSurface SurfaceType = EPhysicalSurface::SurfaceType_Default;
	if (HitResult.PhysMaterial.IsValid())
	{
		SurfaceType = HitResult.PhysMaterial->SurfaceType;
	}

	// Get hit location for sound/VFX
	FVector HitLocation = HitResult.Location;

	// Check if owner implements BPI_GenericCharacter
	bool bImplementsInterface = Owner->GetClass()->ImplementsInterface(UBPI_GenericCharacter::StaticClass());

	// Look up and play sound for this surface type
	if (const TSoftObjectPtr<USoundBase>* FoundSound = Sounds.Find(SurfaceType))
	{
		if (bImplementsInterface && !FoundSound->IsNull())
		{
			// Call PlaySoftSoundAtLocation via interface
			IBPI_GenericCharacter::Execute_PlaySoftSoundAtLocation(
				Owner,
				*FoundSound,
				HitLocation,
				FRotator::ZeroRotator,
				1.0, // Volume
				1.0, // Pitch
				0.0, // StartTime
				nullptr, // Attenuation
				nullptr, // Concurrency
				Owner, // Owner
				nullptr  // InitialParams
			);
			UE_LOG(LogTemp, Verbose, TEXT("UAN_FootstepTrace: Playing footstep sound at %s for surface %d"), *HitLocation.ToString(), (int32)SurfaceType);
		}
	}

	// Look up and play VFX for this surface type
	if (const TSoftObjectPtr<UNiagaraSystem>* FoundEffect = Effects.Find(SurfaceType))
	{
		if (bImplementsInterface && !FoundEffect->IsNull())
		{
			// Call PlaySoftNiagaraOneshotAtLocationReplicated via interface
			IBPI_GenericCharacter::Execute_PlaySoftNiagaraOneshotAtLocationReplicated(
				Owner,
				*FoundEffect,
				HitLocation,
				FRotator::ZeroRotator,
				true, // bAutoDestroy
				true, // bAutoActivate
				false // bPreCullCheck
			);
			UE_LOG(LogTemp, Verbose, TEXT("UAN_FootstepTrace: Playing footstep VFX at %s for surface %d"), *HitLocation.ToString(), (int32)SurfaceType);
		}
	}
}

FString UAN_FootstepTrace::GetNotifyName_Implementation() const
{
	return TEXT("Footstep Trace");
}

