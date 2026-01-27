// W_FirstLootNotification.cpp
// C++ Widget implementation for W_FirstLootNotification
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_FirstLootNotification.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "TimerManager.h"

UW_FirstLootNotification::UW_FirstLootNotification(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_FirstLootNotification::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification::NativeConstruct"));
}

void UW_FirstLootNotification::NativeDestruct()
{
	// Clear timer if still active
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HideTimerHandle);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification::NativeDestruct"));
}

void UW_FirstLootNotification::CacheWidgetReferences()
{
	// Widget reference caching intentionally empty - dev tool widget
}
void UW_FirstLootNotification::EventOnFinish_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification::EventOnFinish_Implementation"));

	// Play fade out animation if it exists
	if (UWidgetAnimation* FadeOutAnim = FindAnimationByName(TEXT("FadeOut")))
	{
		// Bind to animation finished event
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UW_FirstLootNotification::OnFadeOutComplete);
		BindToAnimationFinished(FadeOutAnim, AnimFinishedDelegate);

		PlayAnimation(FadeOutAnim);
		UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification - Playing FadeOut animation"));
	}
	else
	{
		// No fade out animation, hide immediately
		OnFadeOutComplete();
	}
}

void UW_FirstLootNotification::OnFadeOutComplete()
{
	UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification::OnFadeOutComplete - Hiding widget"));
	// Hide the widget (it's part of HUD so don't remove, just hide)
	SetVisibility(ESlateVisibility::Collapsed);
}


void UW_FirstLootNotification::EventShowNotification_Implementation(const FText& ItemName, UTexture2D* ItemIcon, double InDuration)
{
	UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification::EventShowNotification - Item: %s, Duration: %.1f"),
		*ItemName.ToString(), InDuration);

	// Set item name text
	if (UTextBlock* ItemNameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
	{
		ItemNameTextBlock->SetText(ItemName);
		UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification - Set ItemNameText: %s"), *ItemName.ToString());
	}

	// Set item icon
	if (UImage* ItemIconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		if (ItemIcon)
		{
			ItemIconImage->SetBrushFromTexture(ItemIcon);
			UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification - Set ItemIcon from texture"));
		}
	}

	// Make widget visible
	SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	// Play fade in animation if it exists
	if (UWidgetAnimation* FadeInAnim = FindAnimationByName(TEXT("FadeIn")))
	{
		PlayAnimation(FadeInAnim);
		UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification - Playing FadeIn animation"));
	}

	// Clear any existing timer and set new one
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(HideTimerHandle);

		float TimerDuration = InDuration > 0.0 ? InDuration : 3.0;
		World->GetTimerManager().SetTimer(
			HideTimerHandle,
			this,
			&UW_FirstLootNotification::EventOnFinish,
			TimerDuration,
			false
		);
		UE_LOG(LogTemp, Log, TEXT("UW_FirstLootNotification - Timer set for %.1f seconds"), TimerDuration);
	}
}

UWidgetAnimation* UW_FirstLootNotification::FindAnimationByName(const FName& AnimName) const
{
	if (UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass()))
	{
		for (UWidgetAnimation* Anim : WidgetClass->Animations)
		{
			if (Anim && Anim->GetFName() == AnimName)
			{
				return Anim;
			}
		}
	}
	return nullptr;
}
