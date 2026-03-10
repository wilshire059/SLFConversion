// SLFPuzzleMarker.h
// Puzzle room marker — designates a room as a puzzle area with a specific type

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SLFEnums.h"
#include "SLFPuzzleMarker.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPuzzleSolved);

UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API ASLFPuzzleMarker : public AActor
{
	GENERATED_BODY()

public:
	ASLFPuzzleMarker();

	/** Type of puzzle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	ESLFPuzzleType PuzzleType = ESLFPuzzleType::Lever;

	/** Whether the puzzle has been solved */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	bool bIsSolved = false;

	/** Number of objectives to complete (e.g., pull 3 levers) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	int32 RequiredObjectives = 1;

	/** Number of objectives completed so far */
	UPROPERTY(BlueprintReadOnly, Category = "Puzzle")
	int32 CompletedObjectives = 0;

	/** Tag linking this puzzle to a boss door (bRequiresPuzzle) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Puzzle")
	FName LinkedBossDoorTag;

	/** Fired when puzzle is solved */
	UPROPERTY(BlueprintAssignable, Category = "Puzzle|Events")
	FOnPuzzleSolved OnPuzzleSolved;

	/** Register an objective completion */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void CompleteObjective();

	/** Check if puzzle is complete */
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	bool IsPuzzleComplete() const { return bIsSolved; }

protected:
	virtual void BeginPlay() override;
};
