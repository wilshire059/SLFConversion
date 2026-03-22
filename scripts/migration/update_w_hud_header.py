# Script to add all event declarations to W_HUD.h
path = r'C:/scripts/SLFConversion/Source/SLFConversion/Widgets/W_HUD.h'
with open(path, 'r', encoding='utf-8') as f:
    content = f.read()

# New events to add (34 more events with their parameters)
new_events = '''
	// Additional Menu Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowEquipment();
	virtual void EventShowEquipment_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowStatus();
	virtual void EventShowStatus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowCrafting();
	virtual void EventShowCrafting_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowSettings();
	virtual void EventShowSettings_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventShowGameMenu();
	virtual void EventShowGameMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseCrafting();
	virtual void EventCloseCrafting_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseStatus();
	virtual void EventCloseStatus_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseSettings();
	virtual void EventCloseSettings_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventCloseGameMenu();
	virtual void EventCloseGameMenu_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Menu")
	void EventOnExitStorage();
	virtual void EventOnExitStorage_Implementation();

	// Stat/Status Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatUpdated(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);
	virtual void EventOnStatUpdated_Implementation(UB_Stat* UpdatedStat, double Change, bool bUpdateAffectedStats, ESLFValueType ValueType);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatusEffectAdded(UB_StatusEffect* StatusEffect);
	virtual void EventOnStatusEffectAdded_Implementation(UB_StatusEffect* StatusEffect);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnStatsInitialized();
	virtual void EventOnStatsInitialized_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Stats")
	void EventOnBuffDetected(UPDA_Buff* Buff, bool bAdded);
	virtual void EventOnBuffDetected_Implementation(UPDA_Buff* Buff, bool bAdded);

	// Item Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Items")
	void EventOnItemLooted(const FSLFItemInfo& Item, int32 Amount, bool bExists);
	virtual void EventOnItemLooted_Implementation(const FSLFItemInfo& Item, int32 Amount, bool bExists);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Items")
	void EventOnAbilityInfoInvalidated();
	virtual void EventOnAbilityInfoInvalidated_Implementation();

	// Save/Load Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|SaveLoad")
	void EventOnDataLoaded(const FSLFSaveGameInfo& LoadedData);
	virtual void EventOnDataLoaded_Implementation(const FSLFSaveGameInfo& LoadedData);

	// Interaction Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventShowInteractableWidget(AB_Interactable* Interactable);
	virtual void EventShowInteractableWidget_Implementation(AB_Interactable* Interactable);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventHideInteractionWidget();
	virtual void EventHideInteractionWidget_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventSetupRestingPointWidget(AB_RestingPoint* TargetCampfire);
	virtual void EventSetupRestingPointWidget_Implementation(AB_RestingPoint* TargetCampfire);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Interaction")
	void EventShowNpcInteractionWidget();
	virtual void EventShowNpcInteractionWidget_Implementation();

	// Dialog Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Dialog")
	void EventSetupDialog(const FText& InText);
	virtual void EventSetupDialog_Implementation(const FText& InText);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Dialog")
	void EventFinishDialog();
	virtual void EventFinishDialog_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Dialog")
	void EventSetupNpcWindow(const FText& NpcName, UPDA_Vendor* VendorAsset, UAC_AI_InteractionManager* DialogComponent);
	virtual void EventSetupNpcWindow_Implementation(const FText& NpcName, UPDA_Vendor* VendorAsset, UAC_AI_InteractionManager* DialogComponent);

	// Visual Effects Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventFade(ESLFFadeType FadeType, float PlayRate);
	virtual void EventFade_Implementation(ESLFFadeType FadeType, float PlayRate);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventSwitchViewport(int32 Index);
	virtual void EventSwitchViewport_Implementation(int32 Index);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventSendInteractionErrorMessage(const FText& Message, float Time);
	virtual void EventSendInteractionErrorMessage_Implementation(const FText& Message, float Time);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Visual")
	void EventBigScreenMessage(const FText& Message, UMaterialInterface* GradientMaterial, bool bHasBackdrop, float AnimationRateScale);
	virtual void EventBigScreenMessage_Implementation(const FText& Message, UMaterialInterface* GradientMaterial, bool bHasBackdrop, float AnimationRateScale);

	// Save Icon Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|SaveIcon")
	void EventShowAutoSaveIcon(float Duration);
	virtual void EventShowAutoSaveIcon_Implementation(float Duration);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|SaveIcon")
	void EventRemoveAutoSaveIcon();
	virtual void EventRemoveAutoSaveIcon_Implementation();

	// Loading Screen Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Loading")
	void EventCreateLoadingScreen(float AdditionalWaitDuration, bool bAutoRemove);
	virtual void EventCreateLoadingScreen_Implementation(float AdditionalWaitDuration, bool bAutoRemove);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Loading")
	void EventOnLoadingScreenFinished();
	virtual void EventOnLoadingScreenFinished_Implementation();

	// Cinematic Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Cinematic")
	void EventToggleCinematicMode(bool bActive, bool bFadeOutIfNotCinematicMode);
	virtual void EventToggleCinematicMode_Implementation(bool bActive, bool bFadeOutIfNotCinematicMode);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Cinematic")
	void EventShowSkipCinematicNotification();
	virtual void EventShowSkipCinematicNotification_Implementation();

	// Boss Events
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Boss")
	void EventShowBossBar(const FText& BossName, AActor* BossActor);
	virtual void EventShowBossBar_Implementation(const FText& BossName, AActor* BossActor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Boss")
	void EventHideBossBar();
	virtual void EventHideBossBar_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "W_HUD|Boss")
	void EventOnBossDestroyed(AActor* DestroyedActor);
	virtual void EventOnBossDestroyed_Implementation(AActor* DestroyedActor);
'''

# Insert before 'protected:'
insertion_point = content.find('protected:')
if insertion_point != -1:
    content = content[:insertion_point] + new_events + '\n' + content[insertion_point:]

with open(path, 'w', encoding='utf-8') as f:
    f.write(content)

print('Added 34 event declarations to W_HUD.h')
