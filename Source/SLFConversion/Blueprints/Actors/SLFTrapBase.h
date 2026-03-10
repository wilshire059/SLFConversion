// SLFTrapBase.h
// Base class for dungeon traps (pressure plates, arrow launchers, swinging blades)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "SLFTrapBase.generated.h"

UENUM(BlueprintType)
enum class ESLFTrapType : uint8
{
	PressurePlate  = 0 UMETA(DisplayName = "Pressure Plate"),
	ArrowLauncher  = 1 UMETA(DisplayName = "Arrow Launcher"),
	SwingingBlade  = 2 UMETA(DisplayName = "Swinging Blade"),
	FallingRocks   = 3 UMETA(DisplayName = "Falling Rocks"),
	FireJet        = 4 UMETA(DisplayName = "Fire Jet")
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFTrapBase : public AActor
{
	GENERATED_BODY()

public:
	ASLFTrapBase();

	/** Trap type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	ESLFTrapType TrapType = ESLFTrapType::PressurePlate;

	/** Damage dealt by this trap */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	float Damage = 50.0f;

	/** Whether the trap is currently active */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	bool bIsActive = true;

	/** Whether the trap resets after triggering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	bool bResets = true;

	/** Reset delay in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trap")
	float ResetDelay = 3.0f;

	/** Trigger volume */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* TriggerVolume;

	/** Visual mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* TrapMesh;

	/** Trigger the trap */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap")
	void ActivateTrap(AActor* Victim);
	virtual void ActivateTrap_Implementation(AActor* Victim);

	/** Reset the trap */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Trap")
	void ResetTrap();
	virtual void ResetTrap_Implementation();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnTriggerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FTimerHandle ResetTimerHandle;
};
