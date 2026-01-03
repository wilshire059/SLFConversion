// DebugCentralComponent.cpp
// C++ implementation for AC_DebugCentral
//
// ═══════════════════════════════════════════════════════════════════════════════
// IMPLEMENTATION SUMMARY - AC_DebugCentral
// ═══════════════════════════════════════════════════════════════════════════════
// Variables:         16/16 (initialized in constructor)
// Functions:         3/3 implemented
// Event Dispatchers: 0/0
// ═══════════════════════════════════════════════════════════════════════════════

#include "DebugCentralComponent.h"
#include "TimerManager.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"

UDebugCentralComponent::UDebugCentralComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Initialize component references (all null initially)
	StatusEffectComponent = nullptr;
	StatComponent = nullptr;
	CombatComponent = nullptr;
	ActionComponent = nullptr;
	InteractionComponent = nullptr;
	BuffComponent = nullptr;
	InputComponent = nullptr;
	EquipmentComponent = nullptr;
	InventoryComponent = nullptr;
	RadarComponent = nullptr;
	ProgressComponent = nullptr;
	SaveLoadComponent = nullptr;

	// Initialize config
	bEnableDebugging = false;
	ToggleKey = EKeys::F1;

	// Initialize runtime
	HUD_Debug = nullptr;
}

void UDebugCentralComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!bEnableDebugging)
	{
		UE_LOG(LogTemp, Log, TEXT("[DebugCentral] Debugging disabled"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("[DebugCentral] BeginPlay - Scheduling LateInitialize"));

	// Schedule late initialization
	if (UWorld* World = GetWorld())
	{
		FTimerHandle InitTimer;
		World->GetTimerManager().SetTimer(InitTimer, this,
			&UDebugCentralComponent::LateInitialize_Implementation, 0.1f, false);
	}

	SetComponentTickEnabled(true);
}

void UDebugCentralComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bEnableDebugging) return;

	// Check for toggle key press
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (PC->WasInputKeyJustPressed(ToggleKey))
		{
			if (HUD_Debug)
			{
				ESlateVisibility CurrentVis = HUD_Debug->GetVisibility();
				HUD_Debug->SetVisibility(CurrentVis == ESlateVisibility::Visible ?
					ESlateVisibility::Collapsed : ESlateVisibility::Visible);

				UE_LOG(LogTemp, Log, TEXT("[DebugCentral] Toggled HUD visibility"));
			}
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// LATE INITIALIZE [1/3]
// ═══════════════════════════════════════════════════════════════════════════════

void UDebugCentralComponent::LateInitialize_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("[DebugCentral] LateInitialize - Caching component references"));

	AActor* Owner = GetOwner();
	if (!Owner) return;

	// Cache all component references
	TArray<UActorComponent*> AllComponents;
	Owner->GetComponents(AllComponents);

	for (UActorComponent* Comp : AllComponents)
	{
		FString ClassName = Comp->GetClass()->GetName();

		if (ClassName.Contains(TEXT("StatusEffect")))
			StatusEffectComponent = Comp;
		else if (ClassName.Contains(TEXT("Stat")) && !ClassName.Contains(TEXT("StatusEffect")))
			StatComponent = Comp;
		else if (ClassName.Contains(TEXT("Combat")))
			CombatComponent = Comp;
		else if (ClassName.Contains(TEXT("Action")))
			ActionComponent = Comp;
		else if (ClassName.Contains(TEXT("Interaction")))
			InteractionComponent = Comp;
		else if (ClassName.Contains(TEXT("Buff")))
			BuffComponent = Comp;
		else if (ClassName.Contains(TEXT("Input")))
			InputComponent = Comp;
		else if (ClassName.Contains(TEXT("Equipment")))
			EquipmentComponent = Comp;
		else if (ClassName.Contains(TEXT("Inventory")))
			InventoryComponent = Comp;
		else if (ClassName.Contains(TEXT("Radar")))
			RadarComponent = Comp;
		else if (ClassName.Contains(TEXT("Progress")))
			ProgressComponent = Comp;
		else if (ClassName.Contains(TEXT("SaveLoad")))
			SaveLoadComponent = Comp;
	}

	UE_LOG(LogTemp, Log, TEXT("[DebugCentral] Cached %d components"), AllComponents.Num());
}
