// W_Settings_KeyMapping.cpp
// C++ Widget implementation for W_Settings_KeyMapping
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/WidgetBlueprint/W_Settings_KeyMapping.json

#include "Widgets/W_Settings_KeyMapping.h"
#include "Blueprints/BFL_Helper.h"
#include "Components/Image.h"
#include "Kismet/KismetSystemLibrary.h"

UW_Settings_KeyMapping::UW_Settings_KeyMapping(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, TargetInputMapping(nullptr)
	, NavigationMapping(nullptr)
	, NavigationIndex(0)
	, SelectInputIcon(nullptr)
	, BackInputIcon(nullptr)
	, ScrollLeftInputIcon(nullptr)
	, ScrollRightInputIcon(nullptr)
	, DefaultInputIcon(nullptr)
	, IA_NavigableMenu_Ok(nullptr)
	, IA_NavigableMenu_Back(nullptr)
	, IA_NavigableMenu_Left(nullptr)
	, IA_NavigableMenu_Right(nullptr)
	, IA_NavigableMenu_ResetToDefaults(nullptr)
{
}

void UW_Settings_KeyMapping::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_KeyMapping::NativeConstruct"));
}

void UW_Settings_KeyMapping::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_Settings_KeyMapping::NativeDestruct"));
}

void UW_Settings_KeyMapping::CacheWidgetReferences()
{
	// Widget bindings are handled by UMG via meta = (BindWidget)
	// Additional caching can be done here if needed
}

void UW_Settings_KeyMapping::OnInputDeviceChanged_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_KeyMapping::OnInputDeviceChanged_Implementation - ActiveDeviceType: %d"),
		static_cast<int32>(ActiveDeviceType));

	// Blueprint JSON Analysis (OnInputDeviceChanged graph):
	// - Entry node connects to Sequence node with 5 outputs (then_0 through then_4)
	// - Each sequence output calls GetKeysForIA for a different input action
	// - Flow: GetKeysForIA -> IsNotEmpty check -> Branch -> SelectInt (pick key index) ->
	//         GetInputIconForKey -> IsValidSoftObjectReference -> SetBrushFromSoftTexture
	//
	// The 5 parallel branches are:
	// then_0: IA_NavigableMenu_Ok -> SelectInputIcon
	// then_1: IA_NavigableMenu_Back -> BackInputIcon
	// then_2: IA_NavigableMenu_Left -> ScrollLeftInputIcon
	// then_3: IA_NavigableMenu_Right -> ScrollRightInputIcon
	// then_4: IA_NavigableMenu_ResetToDefaults -> DefaultInputIcon

	// Branch 0: Update SelectInputIcon with IA_NavigableMenu_Ok key
	UpdateInputIconForAction(IA_NavigableMenu_Ok, SelectInputIcon);

	// Branch 1: Update BackInputIcon with IA_NavigableMenu_Back key
	UpdateInputIconForAction(IA_NavigableMenu_Back, BackInputIcon);

	// Branch 2: Update ScrollLeftInputIcon with IA_NavigableMenu_Left key
	UpdateInputIconForAction(IA_NavigableMenu_Left, ScrollLeftInputIcon);

	// Branch 3: Update ScrollRightInputIcon with IA_NavigableMenu_Right key
	UpdateInputIconForAction(IA_NavigableMenu_Right, ScrollRightInputIcon);

	// Branch 4: Update DefaultInputIcon with IA_NavigableMenu_ResetToDefaults key
	UpdateInputIconForAction(IA_NavigableMenu_ResetToDefaults, DefaultInputIcon);
}

void UW_Settings_KeyMapping::UpdateInputIconForAction(UInputAction* TargetIA, UImage* TargetImage)
{
	// Validate inputs
	if (!TargetImage)
	{
		UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: TargetImage is null, skipping"));
		return;
	}

	if (!TargetIA)
	{
		UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: TargetIA is null, skipping"));
		return;
	}

	if (!NavigationMapping)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateInputIconForAction: NavigationMapping is null"));
		return;
	}

	// Step 1: Get all keys mapped to this input action
	// Blueprint: GetKeysForIA(NavigationMapping, TargetIA) -> MappedKeys array
	TArray<FKey> MappedKeys;
	UBFL_Helper::GetKeysForIA(NavigationMapping, TargetIA, this, MappedKeys);

	UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: Got %d keys for action %s"),
		MappedKeys.Num(), *TargetIA->GetName());

	// Step 2: Check if array is not empty (Branch condition)
	// Blueprint: Array_IsNotEmpty -> Branch
	if (MappedKeys.Num() == 0)
	{
		// FALSE branch: No keys mapped, nothing to update
		UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: No keys mapped for action %s"),
			*TargetIA->GetName());
		return;
	}

	// TRUE branch: Keys exist, select the appropriate one based on device type

	// Step 3: Select key index based on ActiveDeviceType
	// Blueprint: SelectInt(A=1, B=0, bPickA=(ActiveDeviceType == Gamepad))
	// If Gamepad: pick index 1 (typically gamepad key)
	// If not Gamepad (keyboard/mouse): pick index 0 (typically keyboard key)
	const int32 KeyIndex = (ActiveDeviceType == EHardwareDevicePrimaryType::Gamepad) ? 1 : 0;

	// Ensure index is valid (clamp to array bounds)
	const int32 SafeKeyIndex = FMath::Clamp(KeyIndex, 0, MappedKeys.Num() - 1);
	const FKey SelectedKey = MappedKeys[SafeKeyIndex];

	UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: Selected key %s at index %d (DeviceType: %d)"),
		*SelectedKey.ToString(), SafeKeyIndex, static_cast<int32>(ActiveDeviceType));

	// Step 4: Get the icon for this key
	// Blueprint: GetInputIconForKey(SelectedKey) -> Icon (TSoftObjectPtr<UTexture2D>)
	TSoftObjectPtr<UTexture2D> Icon;
	GetInputIconForKey(SelectedKey, Icon);

	// Step 5: Check if icon is valid soft reference
	// Blueprint: IsValidSoftObjectReference(Icon) -> Branch
	if (!Icon.IsValid() && Icon.IsNull())
	{
		// Icon reference is not valid, skip setting brush
		UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: Icon is not valid for key %s"),
			*SelectedKey.ToString());
		return;
	}

	// Step 6: Set the brush from the soft texture reference
	// Blueprint: SetBrushFromSoftTexture(TargetImage, Icon, bMatchSize=true)
	TargetImage->SetBrushFromSoftTexture(Icon, true);

	UE_LOG(LogTemp, Verbose, TEXT("UpdateInputIconForAction: Set brush for action %s with key %s"),
		*TargetIA->GetName(), *SelectedKey.ToString());
}

void UW_Settings_KeyMapping::EventInitializeKeyMapping_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_KeyMapping::EventInitializeKeyMapping_Implementation"));
}


void UW_Settings_KeyMapping::EventReinitializeAllMappings_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_KeyMapping::EventReinitializeAllMappings_Implementation"));
}


void UW_Settings_KeyMapping::EventResetAllEntriesToDefault_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_Settings_KeyMapping::EventResetAllEntriesToDefault_Implementation"));
}
