// SLFAnimNotifyStateTelegraph.cpp
#include "SLFAnimNotifyStateTelegraph.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"

FString USLFAnimNotifyStateTelegraph::GetNotifyName_Implementation() const
{
	return TEXT("Telegraph");
}

static FName FindValidBone(USkeletalMeshComponent* MeshComp, FName Preferred)
{
	// Try preferred name first (as socket then as bone)
	if (MeshComp->DoesSocketExist(Preferred))
	{
		return Preferred;
	}
	if (MeshComp->GetBoneIndex(Preferred) != INDEX_NONE)
	{
		return Preferred;
	}

	// Try common bone names
	static const FName Fallbacks[] = {
		FName("weapon_r"), FName("hand_r"), FName("R_Hand"), FName("Hand_R"),
		FName("lowerarm_r"), FName("R_Forearm"), FName("R_Sword"),
	};
	for (const FName& FB : Fallbacks)
	{
		if (MeshComp->GetBoneIndex(FB) != INDEX_NONE)
		{
			return FB;
		}
	}
	return NAME_None;
}

static UNiagaraComponent* SpawnOnBone(
	USkeletalMeshComponent* MeshComp,
	FName BoneName,
	UNiagaraSystem* VFX,
	FLinearColor Color,
	float Scale)
{
	if (!MeshComp || BoneName.IsNone() || !VFX) return nullptr;

	UNiagaraComponent* Comp = UNiagaraFunctionLibrary::SpawnSystemAttached(
		VFX,
		MeshComp,
		BoneName,
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		EAttachLocation::SnapToTarget,
		false
	);

	if (Comp)
	{
		Comp->SetRelativeScale3D(FVector(Scale));
		Comp->SetColorParameter(FName(TEXT("Color")), Color);
		Comp->SetColorParameter(FName(TEXT("BaseColor")), Color);
		Comp->SetFloatParameter(FName(TEXT("EmissiveIntensity")), 15.0f);
		Comp->SetFloatParameter(FName(TEXT("Intensity")), 15.0f);
		Comp->SetFloatParameter(FName(TEXT("SpawnRate")), 50.0f);
		Comp->Activate(true);
	}

	return Comp;
}

void USLFAnimNotifyStateTelegraph::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp) return;
	AActor* Owner = MeshComp->GetOwner();

	// Resolve bones
	FName Bone1 = FindValidBone(MeshComp, AttachSocket);
	FName Bone2 = SecondSocket.IsNone() ? NAME_None : FindValidBone(MeshComp, SecondSocket);

	if (Bone1.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("[Telegraph] FAILED: No valid bone found on %s (tried %s + fallbacks)"),
			Owner ? *Owner->GetName() : TEXT("?"), *AttachSocket.ToString());
		return;
	}

	// Load VFX — NS_RibbonTrail for visible weapon trail telegraph
	UNiagaraSystem* VFXToUse = TelegraphVFX;
	if (!VFXToUse)
	{
		static const TCHAR* VFXPaths[] = {
			TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_RibbonTrail.NS_RibbonTrail"),
			TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_FireParticles.NS_FireParticles"),
			TEXT("/Game/SoulslikeFramework/VFX/Systems/NS_BossBuffHands.NS_BossBuffHands"),
		};
		for (const TCHAR* Path : VFXPaths)
		{
			VFXToUse = LoadObject<UNiagaraSystem>(nullptr, Path);
			if (VFXToUse)
			{
				UE_LOG(LogTemp, Log, TEXT("[Telegraph] Loaded VFX: %s"), Path);
				break;
			}
		}
	}

	if (!VFXToUse)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Telegraph] FAILED: No Niagara system found"));
		return;
	}

	ActiveVFX1 = SpawnOnBone(MeshComp, Bone1, VFXToUse, TelegraphColor, VFXScale);
	ActiveVFX2 = SpawnOnBone(MeshComp, Bone2, VFXToUse, TelegraphColor, VFXScale);

	UE_LOG(LogTemp, Warning, TEXT("[Telegraph] BEGIN on %s - Bone1=%s(%s) Bone2=%s(%s) VFX=%s Dur=%.2fs"),
		Owner ? *Owner->GetName() : TEXT("?"),
		*Bone1.ToString(), ActiveVFX1 ? TEXT("OK") : TEXT("FAIL"),
		Bone2.IsNone() ? TEXT("none") : *Bone2.ToString(),
		ActiveVFX2 ? TEXT("OK") : (Bone2.IsNone() ? TEXT("skip") : TEXT("FAIL")),
		*VFXToUse->GetName(), TotalDuration);
}

void USLFAnimNotifyStateTelegraph::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ActiveVFX1)
	{
		ActiveVFX1->Deactivate();
		ActiveVFX1->DestroyComponent();
		ActiveVFX1 = nullptr;
	}
	if (ActiveVFX2)
	{
		ActiveVFX2->Deactivate();
		ActiveVFX2->DestroyComponent();
		ActiveVFX2 = nullptr;
	}
}
