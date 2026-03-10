// SLFActionMantle.h
// Traversal action — vault, mantle, hurdle, or climb based on obstacle height

#pragma once

#include "CoreMinimal.h"
#include "Blueprints/SLFActionBase.h"
#include "SLFActionMantle.generated.h"

/** Height tier determines which animation plays */
UENUM(BlueprintType)
enum class ESLFTraversalType : uint8
{
	None     UMETA(DisplayName = "None"),
	Vault    UMETA(DisplayName = "Vault"),     // <100cm — hop over
	Hurdle   UMETA(DisplayName = "Hurdle"),    // 100-150cm — jump over
	Mantle   UMETA(DisplayName = "Mantle"),    // 150-250cm — pull up
	Climb    UMETA(DisplayName = "Climb")      // 250-350cm — full climb
};

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API USLFActionMantle : public USLFActionBase
{
	GENERATED_BODY()

public:
	USLFActionMantle();

	// --- Height thresholds ---

	/** Minimum obstacle height to trigger any traversal (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float MinTraversalHeight = 50.0f;

	/** Max height for vault (hop over) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float VaultMaxHeight = 100.0f;

	/** Max height for hurdle (jump over) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float HurdleMaxHeight = 150.0f;

	/** Max height for mantle (pull up) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float MantleMaxHeight = 250.0f;

	/** Max height for climb */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float ClimbMaxHeight = 350.0f;

	/** Forward trace distance to detect obstacle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float ForwardTraceDistance = 100.0f;

	/** Downward trace distance from above obstacle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal")
	float DownwardTraceDistance = 500.0f;

	// --- Montages per traversal type ---

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal|Montages")
	TSoftObjectPtr<UAnimMontage> VaultMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal|Montages")
	TSoftObjectPtr<UAnimMontage> HurdleMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal|Montages")
	TSoftObjectPtr<UAnimMontage> MantleMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Traversal|Montages")
	TSoftObjectPtr<UAnimMontage> ClimbMontage;

	/** Detect obstacle and determine traversal type */
	UFUNCTION(BlueprintCallable, Category = "Traversal")
	ESLFTraversalType DetectLedge(FVector& OutLedgeLocation) const;

	virtual void ExecuteAction_Implementation() override;
	virtual bool CanExecuteAction_Implementation() override;
};
