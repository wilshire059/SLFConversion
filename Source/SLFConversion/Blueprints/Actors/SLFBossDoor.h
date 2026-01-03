// SLFBossDoor.h
// C++ base for B_BossDoor - Boss arena fog gate
#pragma once

#include "CoreMinimal.h"
#include "SLFDoorBase.h"
#include "SLFBossDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBossDoorSealed);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFBossDoor : public ASLFDoorBase
{
	GENERATED_BODY()

public:
	ASLFBossDoor();

	// ============================================================
	// MIGRATION SUMMARY: B_BossDoor
	// Variables: 5 | Functions: 4 | Dispatchers: 1
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UNiagaraComponent* FogWallEffect;

	// Boss Door Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bIsFogWall = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bSealed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	TSubclassOf<AActor> BossClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	FName BossArenaTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss Door")
	bool bDestroyOnBossDefeated = true;

	// Event Dispatcher
	UPROPERTY(BlueprintAssignable, Category = "Boss Door|Events")
	FOnBossDoorSealed OnBossDoorSealed;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void SealDoor();
	virtual void SealDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void UnsealDoor();
	virtual void UnsealDoor_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void OnBossDefeated();
	virtual void OnBossDefeated_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Boss Door")
	void OnPlayerEnterArena(AActor* Player);
	virtual void OnPlayerEnterArena_Implementation(AActor* Player);

protected:
	virtual void BeginPlay() override;
};
