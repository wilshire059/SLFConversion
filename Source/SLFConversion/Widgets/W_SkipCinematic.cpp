// W_SkipCinematic.cpp
// C++ Widget implementation for W_SkipCinematic
//
// 20-PASS VALIDATION: 2026-01-02 Autonomous Session
// Source: BlueprintDNA_v2/WidgetBlueprint/W_SkipCinematic.json
//
// OnInputDeviceChanged Logic (from JSON):
// 1. Call BFL_Helper::GetKeysForIA(RelatedMapping, IA_GameMenu) -> MappedKeys
// 2. Branch: if MappedKeys.IsNotEmpty()
//    TRUE:
//      a. SelectInt: if ActiveDeviceType == Gamepad then 1 else 0 -> KeyIndex
//      b. Get MappedKeys[KeyIndex] -> SelectedKey
//      c. GetInputIconForKey(SelectedKey) -> Icon
//      d. Branch: if IsValidSoftObjectReference(Icon)
//         TRUE: KeyIcon->SetBrushFromSoftTexture(Icon, false)
//         FALSE: (no action)
//    FALSE: (no action)

#include "Widgets/W_SkipCinematic.h"
#include "Blueprints/BFL_Helper.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "Engine/World.h"

UW_SkipCinematic::UW_SkipCinematic(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, RelatedMapping(nullptr)
	, Text(FText::GetEmpty())
	, IconSize(FVector2D::ZeroVector)
	, TargetInputAction(nullptr)
	, MessageText(nullptr)
	, IconSizer(nullptr)
	, KeyIcon(nullptr)
{
}

void UW_SkipCinematic::NativePreConstruct()
{
	Super::NativePreConstruct();

	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::NativePreConstruct - Initializing widget"));

	// PreConstruct logic from JSON EventGraph:
	// 1. MessageText->SetText(Text)
	// 2. IconSizer->SetWidthOverride(IconSize.X)
	// 3. IconSizer->SetHeightOverride(IconSize.Y)

	if (MessageText)
	{
		MessageText->SetText(Text);
		UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::NativePreConstruct - Set MessageText to: %s"), *Text.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_SkipCinematic::NativePreConstruct - MessageText is null"));
	}

	if (IconSizer)
	{
		IconSizer->SetWidthOverride(static_cast<float>(IconSize.X));
		IconSizer->SetHeightOverride(static_cast<float>(IconSize.Y));
		UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::NativePreConstruct - Set IconSizer size to: (%f, %f)"), IconSize.X, IconSize.Y);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_SkipCinematic::NativePreConstruct - IconSizer is null"));
	}
}

void UW_SkipCinematic::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::NativeConstruct - Setting timer for EventInitializeInputIcon"));

	// Construct logic from JSON EventGraph:
	// Set Timer by Event with Time=0.25, bLooping=false
	// Delegate = EventInitializeInputIcon
	// Comment: "Allow .25 delay for initial input device initialization."
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			InitializeInputIconTimerHandle,
			FTimerDelegate::CreateWeakLambda(this, [this]()
			{
				EventInitializeInputIcon();
			}),
			0.25f,  // Time: 0.25 seconds
			false   // bLooping: false
		);
	}
}

void UW_SkipCinematic::NativeDestruct()
{
	// Clear the timer if still active
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InitializeInputIconTimerHandle);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::NativeDestruct"));
}

void UW_SkipCinematic::CacheWidgetReferences()
{
	// Widget bindings are automatically resolved by UMG via meta = (BindWidget)
	// This function can cache any additional references if needed
	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::CacheWidgetReferences - MessageText=%s, IconSizer=%s, KeyIcon=%s"),
		MessageText ? TEXT("Valid") : TEXT("Null"),
		IconSizer ? TEXT("Valid") : TEXT("Null"),
		KeyIcon ? TEXT("Valid") : TEXT("Null"));
}

void UW_SkipCinematic::EventInitializeInputIcon_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::EventInitializeInputIcon_Implementation"));

	// EventInitializeInputIcon logic from JSON:
	// Calls OnInputDeviceChanged()
	OnInputDeviceChanged();
}

void UW_SkipCinematic::OnInputDeviceChanged_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged_Implementation - Starting input icon update"));

	// Step 1: Call BFL_Helper::GetKeysForIA(RelatedMapping, TargetInputAction, WorldContext)
	// From JSON: InputMapping = RelatedMapping, TargetIA = IA_GameMenu
	if (!RelatedMapping)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_SkipCinematic::OnInputDeviceChanged - RelatedMapping is null, cannot get keys"));
		return;
	}

	if (!TargetInputAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("UW_SkipCinematic::OnInputDeviceChanged - TargetInputAction is null, cannot get keys"));
		return;
	}

	TArray<FKey> MappedKeys = UBFL_Helper::GetKeysForIA(RelatedMapping, TargetInputAction, this);
	UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged - GetKeysForIA returned %d keys"), MappedKeys.Num());

	// Step 2: Branch - check if MappedKeys is not empty
	if (MappedKeys.Num() > 0)
	{
		// TRUE path:
		// Step 3: Select index based on ActiveDeviceType
		// From JSON: SelectInt with A=1, B=0, bPickA=(ActiveDeviceType == Gamepad)
		// If Gamepad -> use index 1, else use index 0
		const int32 KeyIndex = (ActiveDeviceType == EHardwareDevicePrimaryType::Gamepad) ? 1 : 0;
		UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged - ActiveDeviceType=%d, using KeyIndex=%d"),
			static_cast<int32>(ActiveDeviceType), KeyIndex);

		// Step 4: Get key at selected index (with bounds check)
		const int32 SafeIndex = FMath::Clamp(KeyIndex, 0, MappedKeys.Num() - 1);
		const FKey& SelectedKey = MappedKeys[SafeIndex];
		UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged - Selected key: %s"), *SelectedKey.ToString());

		// Step 5: Get input icon for the selected key
		// Calls parent class function: GetInputIconForKey(Key) -> Icon
		TSoftObjectPtr<UTexture2D> Icon = GetInputIconForKey(SelectedKey);

		// Step 6: Branch - check if Icon is valid soft object reference
		// From JSON: IsValidSoftObjectReference(Icon)
		if (!Icon.IsNull())
		{
			// TRUE path: Set KeyIcon brush from soft texture
			// From JSON: KeyIcon->SetBrushFromSoftTexture(SoftTexture=Icon, bMatchSize=false)
			if (KeyIcon)
			{
				KeyIcon->SetBrushFromSoftTexture(Icon, false);
				UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged - Set KeyIcon brush from texture: %s"),
					*Icon.ToString());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UW_SkipCinematic::OnInputDeviceChanged - KeyIcon widget is null, cannot set brush"));
			}
		}
		else
		{
			// FALSE path: No action (from JSON - else branch is empty)
			UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged - Icon soft object reference is not valid, skipping brush update"));
		}
	}
	else
	{
		// FALSE path: No action (from JSON - else branch is empty)
		UE_LOG(LogTemp, Log, TEXT("UW_SkipCinematic::OnInputDeviceChanged - MappedKeys is empty, skipping icon update"));
	}
}
