# System Menu Widget Peer Review - Request for Gemini

## Context
This is a Blueprint-to-C++ migration project for a Soulslike Framework in Unreal Engine 5.7. The system menu widgets (MainMenu, GameMenu, Settings) were migrated but had incomplete implementations that were just fixed.

## Review Request
Please review the following code changes for:
1. Correctness - Does the navigation logic work correctly?
2. Memory safety - Any potential null pointer issues or memory leaks?
3. Edge cases - What happens with empty button arrays, rapid input, etc.?
4. Input handling - Is the keyboard/gamepad input properly routed?
5. Settings persistence - Is the GameUserSettings integration correct?

---

## Change 1: Added NativeOnKeyDown to W_MainMenu

**Issue:** W_MainMenu had NO keyboard/gamepad input handling. Users couldn't navigate the main menu with keyboard.

**File: Source/SLFConversion/Widgets/W_MainMenu.h**
```cpp
// Added to class declaration
virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
```

**File: Source/SLFConversion/Widgets/W_MainMenu.cpp**
```cpp
FReply UW_MainMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
    FKey Key = InKeyEvent.GetKey();

    UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] NativeOnKeyDown - Key: %s"), *Key.ToString());

    // Navigate Up: W, Up Arrow, Gamepad DPad Up, Left Stick Up
    if (Key == EKeys::W || Key == EKeys::Up || Key == EKeys::Gamepad_DPad_Up || Key == EKeys::Gamepad_LeftStick_Up)
    {
        EventNavigateUp();
        return FReply::Handled();
    }

    // Navigate Down: S, Down Arrow, Gamepad DPad Down, Left Stick Down
    if (Key == EKeys::S || Key == EKeys::Down || Key == EKeys::Gamepad_DPad_Down || Key == EKeys::Gamepad_LeftStick_Down)
    {
        EventNavigateDown();
        return FReply::Handled();
    }

    // Navigate Ok/Confirm: Enter, Space, Gamepad A (FaceButton_Bottom)
    if (Key == EKeys::Enter || Key == EKeys::SpaceBar || Key == EKeys::Gamepad_FaceButton_Bottom)
    {
        EventNavigateOk();
        return FReply::Handled();
    }

    return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}
```

Also added `SetIsFocusable(true)` in NativeConstruct to enable keyboard focus.

---

## Change 2: Fixed W_Settings EventNavigateOk

**Issue:** Pressing OK/Enter on a settings entry did nothing - it just logged.

**File: Source/SLFConversion/Widgets/W_Settings_Entry.h**
```cpp
// Added new function declaration
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings_Entry")
void EventActivateEntry();
virtual void EventActivateEntry_Implementation();
```

**File: Source/SLFConversion/Widgets/W_Settings_Entry.cpp**
```cpp
void UW_Settings_Entry::EventActivateEntry_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] EventActivateEntry - Type: %d"), static_cast<int32>(EntryType));

    // Handle activation based on entry type
    switch (EntryType)
    {
    case ESLFSettingEntry::SingleButton:
        // Broadcast the single button pressed event
        OnSingleButtonEntryPressed.Broadcast();
        UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] Broadcast OnSingleButtonEntryPressed"));
        break;

    case ESLFSettingEntry::DoubleButton:
        // For double buttons, toggle the value (On/Off)
        {
            bool bCurrentBool = (CurrentValue == TEXT("1") || CurrentValue.ToLower() == TEXT("true"));
            SetCurrentBoolValue(!bCurrentBool);
        }
        break;

    case ESLFSettingEntry::DropDown:
        // For dropdowns, cycle to next option
        EventScrollDropdownRight();
        break;

    case ESLFSettingEntry::Slider:
        // For sliders, OK typically does nothing (use left/right to adjust)
        break;

    case ESLFSettingEntry::InputKeySelector:
        // For key selectors, this would start listening for key input
        UE_LOG(LogTemp, Log, TEXT("[W_Settings_Entry] InputKeySelector activation - would start key listening"));
        break;

    default:
        break;
    }
}
```

**File: Source/SLFConversion/Widgets/W_Settings.cpp**
```cpp
void UW_Settings::EventNavigateOk_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateOk"));

    // Activate the current entry (for button entries, toggles, etc.)
    if (SettingEntries.IsValidIndex(EntryNavigationIndex))
    {
        UW_Settings_Entry* Entry = SettingEntries[EntryNavigationIndex];
        if (Entry)
        {
            UE_LOG(LogTemp, Log, TEXT("[W_Settings] Activating entry: %s"), *Entry->GetName());
            Entry->EventActivateEntry();
        }
    }
}
```

---

## Change 3: Added Settings Persistence

**Issue:** Settings values were changed in UI but never actually saved to disk.

**File: Source/SLFConversion/Widgets/W_Settings.h**
```cpp
// Added new function
UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_Settings")
void ApplyAndSaveSettings();
virtual void ApplyAndSaveSettings_Implementation();
```

**File: Source/SLFConversion/Widgets/W_Settings.cpp**
```cpp
void UW_Settings::ApplyAndSaveSettings_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[W_Settings] ApplyAndSaveSettings"));

    UGameUserSettings* GameSettings = UGameUserSettings::GetGameUserSettings();
    if (!GameSettings)
    {
        UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Failed to get GameUserSettings!"));
        return;
    }

    // Iterate through all setting entries and apply their values
    for (UW_Settings_Entry* Entry : SettingEntries)
    {
        if (!Entry) continue;

        FGameplayTag Tag = Entry->SettingTag;
        FString Value = Entry->CurrentValue;

        UE_LOG(LogTemp, Log, TEXT("[W_Settings] Applying setting: %s = %s"), *Tag.ToString(), *Value);

        // Handle specific setting tags
        if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.Resolution"))))
        {
            FString Width, Height;
            if (Value.Split(TEXT("x"), &Width, &Height))
            {
                FIntPoint Resolution(FCString::Atoi(*Width), FCString::Atoi(*Height));
                GameSettings->SetScreenResolution(Resolution);
            }
        }
        else if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.WindowMode"))))
        {
            int32 ModeInt = FCString::Atoi(*Value);
            EWindowMode::Type WindowMode = static_cast<EWindowMode::Type>(ModeInt);
            GameSettings->SetFullscreenMode(WindowMode);
        }
        else if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.VSync"))))
        {
            bool bVSync = (Value == TEXT("1") || Value.ToLower() == TEXT("true"));
            GameSettings->SetVSyncEnabled(bVSync);
        }
        else if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Settings.Graphics.FrameRateLimit"))))
        {
            float FrameRate = FCString::Atof(*Value);
            GameSettings->SetFrameRateLimit(FrameRate);
        }
    }

    // Apply and save all changes
    GameSettings->ApplySettings(false);
    GameSettings->SaveSettings();

    UE_LOG(LogTemp, Log, TEXT("[W_Settings] Settings applied and saved"));
}
```

Settings are now saved when closing the Settings menu:
```cpp
void UW_Settings::EventNavigateCancel_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCancel"));

    // Apply and save settings before closing
    ApplyAndSaveSettings();

    // Broadcast settings closed event
    OnSettingsClosed.Broadcast();
}
```

---

## Change 4: Fixed MainMenu Button Events

**Issue:** W_MainMenu::BindButtonEvents() was a stub that said "handled via Blueprint bindings" but buttons weren't working from C++.

**File: Source/SLFConversion/Widgets/W_MainMenu.h**
```cpp
// Added event dispatcher and handlers
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMainMenuButtonClicked, FName, ButtonName);

UPROPERTY(BlueprintAssignable, Category = "Events")
FOnMainMenuButtonClicked OnMenuButtonClicked;

protected:
    void BindButtonEvents();
    void UnbindButtonEvents();

    UFUNCTION()
    void OnContinueClicked();
    UFUNCTION()
    void OnNewGameClicked();
    UFUNCTION()
    void OnLoadGameClicked();
    UFUNCTION()
    void OnSettingsClicked();
    UFUNCTION()
    void OnCreditsClicked();
    UFUNCTION()
    void OnQuitGameClicked();
```

**File: Source/SLFConversion/Widgets/W_MainMenu.cpp**
```cpp
void UW_MainMenu::BindButtonEvents()
{
    UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] Binding button events"));

    if (BtnContinue)
        BtnContinue->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnContinueClicked);
    if (BtnNewGame)
        BtnNewGame->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnNewGameClicked);
    if (BtnLoadGame)
        BtnLoadGame->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnLoadGameClicked);
    if (BtnSettings)
        BtnSettings->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnSettingsClicked);
    if (BtnCredits)
        BtnCredits->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnCreditsClicked);
    if (BtnQuitGame)
        BtnQuitGame->OnButtonClicked.AddDynamic(this, &UW_MainMenu::OnQuitGameClicked);
}

void UW_MainMenu::UnbindButtonEvents()
{
    if (BtnContinue) BtnContinue->OnButtonClicked.RemoveAll(this);
    if (BtnNewGame) BtnNewGame->OnButtonClicked.RemoveAll(this);
    // ... etc for each button
}

void UW_MainMenu::OnNewGameClicked()
{
    UE_LOG(LogTemp, Log, TEXT("[W_MainMenu] New Game button clicked"));
    OnMenuButtonClicked.Broadcast(FName(TEXT("NewGame")));
}
// ... similar for each button
```

---

## Questions for Review

1. **Navigation Wrap Behavior**: W_GameMenu wraps around when navigating (pressing Up at top goes to bottom), but W_MainMenu clamps (pressing Up at top stays at top). Should this be consistent?

2. **Focus Management**: Each menu sets `SetIsFocusable(true)` and `SetUserFocus()` for keyboard input. Is this the correct pattern, or should focus be managed at a higher level?

3. **Settings Tags**: The ApplyAndSaveSettings uses `FGameplayTag::RequestGameplayTag()` which logs warnings if tags don't exist. Should we use `FGameplayTag::TryMakeFromName()` instead for safer tag lookup?

4. **Settings Apply Timing**: Currently settings are applied when closing the menu. Should there be a separate "Apply" button that applies without closing?

5. **InputKeySelector Incomplete**: The `EventActivateEntry` for InputKeySelector just logs. This would need a full key rebinding system. Is this acceptable as a stub for now?

6. **Delegate Unbinding**: Using `RemoveAll(this)` in UnbindButtonEvents. Is this correct, or should we use `RemoveDynamic()` for each specific binding?

---

## Build Status
All changes compile successfully on UE 5.7.

---

## Gemini Peer Review Feedback - APPLIED

The following issues were identified by Gemini and have been fixed:

### Issue 1: Cancel vs Save Logic (CRITICAL)
**Problem:** EventNavigateCancel was calling ApplyAndSaveSettings(), which is bad UX - "Cancel" should discard changes, not save them.

**Fix Applied (W_Settings.cpp):**
```cpp
void UW_Settings::EventNavigateCancel_Implementation()
{
    UE_LOG(LogTemp, Log, TEXT("[W_Settings] EventNavigateCancel - discarding unsaved changes"));

    // Cancel should NOT save - it discards changes
    // If user wants to save, they should use an explicit Apply button or rely on
    // immediate apply behavior in EventNavigateLeft/Right

    // Broadcast settings closed event (without saving)
    OnSettingsClosed.Broadcast();
}
```

### Issue 2: Resolution Parsing Fragility
**Problem:** `Value.Split("x")` would fail if resolution string had spaces like "1920 x 1080".

**Fix Applied (W_Settings.cpp):**
```cpp
if (Value.Split(TEXT("x"), &Width, &Height))
{
    // Trim whitespace from both parts to handle "1920 x 1080" format
    Width.TrimStartAndEndInline();
    Height.TrimStartAndEndInline();

    int32 WidthInt = FCString::Atoi(*Width);
    int32 HeightInt = FCString::Atoi(*Height);

    // Only apply if we got valid values
    if (WidthInt > 0 && HeightInt > 0)
    {
        FIntPoint Resolution(WidthInt, HeightInt);
        GameSettings->SetScreenResolution(Resolution);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("[W_Settings] Invalid resolution format: %s"), *Value);
    }
}
```

### Issue 3: Navigation Inconsistency (Wrap vs Clamp)
**Problem:** W_Settings used wrap navigation but W_MainMenu used clamp navigation.

**Fix Applied (W_MainMenu.cpp):**
```cpp
// NavigateDown - now wraps
NavigationIndex++;
if (NavigationIndex >= Buttons.Num())
{
    NavigationIndex = 0;  // Wrap to first
}

// NavigateUp - now wraps
NavigationIndex--;
if (NavigationIndex < 0)
{
    NavigationIndex = Buttons.Num() - 1;  // Wrap to last
}
```

### Gemini Feedback Accepted (Not Requiring Code Changes)

1. **Hardcoded Keys OK for system menus** - System menus should have static/universal controls, don't need EnhancedInput mapping
2. **RemoveAll(this) is correct** - Efficiently cleans up all bindings for this object instance
3. **Null checks are properly placed** - BtnContinue, GameSettings, Entry checks prevent crashes
4. **InputKeySelector stub is acceptable** - Rebinding is complex, can be implemented separately

---

## Summary of All Changes

| File | Change |
|------|--------|
| `W_MainMenu.h` | Added NativeOnKeyDown, event dispatcher, button handlers |
| `W_MainMenu.cpp` | Implemented keyboard input, button event binding/unbinding, **wrap navigation** |
| `W_Settings.h` | Added ApplyAndSaveSettings function |
| `W_Settings.cpp` | Implemented settings persistence, **Cancel now discards (doesn't save)**, **robust resolution parsing** |
| `W_Settings_Entry.h` | Added EventActivateEntry function |
| `W_Settings_Entry.cpp` | Implemented entry activation for different entry types |
