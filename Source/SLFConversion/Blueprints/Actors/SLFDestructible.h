// SLFDestructible.h
// C++ base for B_Destructible - Breakable objects (barrels, crates, etc.)
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFDestructible.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDestructibleDamaged, float, Damage, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDestructibleDestroyed, AActor*, Destroyer);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFDestructible : public AActor
{
	GENERATED_BODY()

public:
	ASLFDestructible();

	// ============================================================
	// MIGRATION SUMMARY: B_Destructible
	// Variables: 7 | Functions: 4 | Dispatchers: 2
	// ============================================================

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* DestructibleMesh;

	// Health
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible")
	float MaxHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible")
	bool bIsDestroyed = false;

	// Effects
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible|Effects")
	class UNiagaraSystem* DestructionEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible|Effects")
	USoundBase* DestructionSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible|Effects")
	TSubclassOf<AActor> DebrisClass;

	// Loot on destruction
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Destructible|Loot")
	TArray<TSubclassOf<UObject>> LootOnDestroy;

	// Event Dispatchers
	UPROPERTY(BlueprintAssignable, Category = "Destructible|Events")
	FOnDestructibleDamaged OnDestructibleDamaged;

	UPROPERTY(BlueprintAssignable, Category = "Destructible|Events")
	FOnDestructibleDestroyed OnDestructibleDestroyed;

	// Functions
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destructible")
	void ApplyDamage(float Damage, AActor* DamageCauser);
	virtual void ApplyDamage_Implementation(float Damage, AActor* DamageCauser);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destructible")
	void DestroyObject(AActor* Destroyer);
	virtual void DestroyObject_Implementation(AActor* Destroyer);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destructible")
	void SpawnDebris();
	virtual void SpawnDebris_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Destructible")
	void SpawnLoot();
	virtual void SpawnLoot_Implementation();

protected:
	virtual void BeginPlay() override;
};
