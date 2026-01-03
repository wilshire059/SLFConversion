// B_DemoDisplay.h
// C++ class for Blueprint B_DemoDisplay
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA/Blueprint/B_DemoDisplay.json
// Parent: Actor -> AActor
// Variables: 30 | Functions: 9 | Dispatchers: 0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "SLFEnums.h"
#include "SLFGameTypes.h"
#include "SLFPrimaryDataAssets.h"
#include "InputMappingContext.h"
#include "LevelSequence.h"
#include "LevelSequencePlayer.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "SkeletalMergingLibrary.h"
#include "GeometryCollection/GeometryCollectionObject.h"
#include "Field/FieldSystemObjects.h"
#include "B_DemoDisplay.generated.h"

// Forward declarations
class UAnimMontage;
class UDataTable;


// Event Dispatchers


UCLASS(Blueprintable, BlueprintType)
class SLFCONVERSION_API AB_DemoDisplay : public AActor
{
	GENERATED_BODY()

public:
	AB_DemoDisplay();

	// ═══════════════════════════════════════════════════════════════════════
	// VARIABLES (30)
	// ═══════════════════════════════════════════════════════════════════════

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_Main;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_Corner;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_Side;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_Curve;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_CurveEdge;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	double Width;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	double Depth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	double Height;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title")
	FText TitleText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
	TArray<FText> DescriptionText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	TEnumAsByte<ETextJustify::Type> TextAlignment;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
	double DescriptionScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title")
	double TitleScale;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title")
	FColor TitleColour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
	FColor DescriptionColour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Title")
	bool SeperateTitlePanel;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	bool FloorText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	bool Spotlight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
	int32 Numberoflinesbetweenparagraphs;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Description")
	int32 Numberofspacesbetweenlines;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Debug")
	FString DescriptionFinal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Description")
	double TextPadding;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_TitleBarMain;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_TitleBarEnd;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	bool CastShadows;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_ShadowStraight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Default")
	UInstancedStaticMeshComponent* ISM_ShadowRound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	double ShadowCover;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	FDataTableRowHandle Colour;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Panel Settings")
	FLinearColor CustomColour;

	// ═══════════════════════════════════════════════════════════════════════
	// EVENT DISPATCHERS (0)
	// ═══════════════════════════════════════════════════════════════════════



	// ═══════════════════════════════════════════════════════════════════════
	// FUNCTIONS (9)
	// ═══════════════════════════════════════════════════════════════════════

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void CreateInstances();
	virtual void CreateInstances_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void CreateDisplay();
	virtual void CreateDisplay_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void ScalablePanel(const FTransform& BasePosition, double Length);
	virtual void ScalablePanel_Implementation(const FTransform& BasePosition, double Length);
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void Redraw();
	virtual void Redraw_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void FormatText();
	virtual void FormatText_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void AddTitlePanel();
	virtual void AddTitlePanel_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void ShadowBox();
	virtual void ShadowBox_Implementation();
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "B_DemoDisplay")
	void ScaleSafeInstance(UInstancedStaticMeshComponent* Instance, const FTransform& Transform);
	virtual void ScaleSafeInstance_Implementation(UInstancedStaticMeshComponent* Instance, const FTransform& Transform);

	// Pure helper - calculates text alignment offset based on TextAlignment enum
	// Logic: Select based on TextAlignment - Left returns positive offset, Center returns 0, Right returns negative
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "B_DemoDisplay")
	double TextAlignmentOffset(double WidthAdjustment, bool bForceCenter);
	virtual double TextAlignmentOffset_Implementation(double WidthAdjustment, bool bForceCenter);

	// Returns transform for title panel text positioning
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "B_DemoDisplay")
	FTransform TitlePanelTextTransform();
	virtual FTransform TitlePanelTextTransform_Implementation();

	// Returns transform for title text positioning (considers FloorText)
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, BlueprintPure, Category = "B_DemoDisplay")
	FTransform TitleTextTransform();
	virtual FTransform TitleTextTransform_Implementation();

protected:
	// Called when construction script runs
	virtual void OnConstruction(const FTransform& Transform) override;
};
