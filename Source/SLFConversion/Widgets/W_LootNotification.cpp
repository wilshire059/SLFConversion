// W_LootNotification.cpp
// C++ Widget implementation for W_LootNotification
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_LootNotification.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "TimerManager.h"

UW_LootNotification::UW_LootNotification(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_LootNotification::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references and display item info
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_LootNotification::NativeConstruct - Item: %s x%d, Duration: %.1f"),
		*ItemName.ToString(), ItemLootedAmount, Duration);

	// Play fade in animation if it exists
	if (UWidgetAnimation* FadeInAnim = FindAnimationByName(TEXT("FadeIn")))
	{
		PlayAnimation(FadeInAnim);
		UE_LOG(LogTemp, Log, TEXT("UW_LootNotification - Playing FadeIn animation"));
	}

	// Set timer to call EventFinished after Duration seconds (default to 3.0 if not set)
	float TimerDuration = Duration > 0.0 ? Duration : 3.0;
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			RemovalTimerHandle,
			this,
			&UW_LootNotification::EventFinished,
			TimerDuration,
			false
		);
		UE_LOG(LogTemp, Log, TEXT("UW_LootNotification - Timer set for %.1f seconds"), TimerDuration);
	}
}

void UW_LootNotification::NativeDestruct()
{
	// Clear timer if still active
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(RemovalTimerHandle);
	}

	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_LootNotification::NativeDestruct"));
}

void UW_LootNotification::CacheWidgetReferences()
{
	// Set item name text
	if (UTextBlock* ItemNameTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemNameText"))))
	{
		// Truncate name if needed
		FString NameStr = ItemName.ToString();
		if (MaxItemNameLength > 0 && NameStr.Len() > MaxItemNameLength)
		{
			NameStr = NameStr.Left(MaxItemNameLength - 3) + TEXT("...");
		}
		ItemNameTextBlock->SetText(FText::FromString(NameStr));
		UE_LOG(LogTemp, Log, TEXT("UW_LootNotification - Set ItemNameText: %s"), *NameStr);
	}

	// Set item amount text
	if (UTextBlock* ItemAmountTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("ItemAmountText"))))
	{
		FString AmountStr = FString::Printf(TEXT("x%d"), ItemLootedAmount);
		ItemAmountTextBlock->SetText(FText::FromString(AmountStr));
		UE_LOG(LogTemp, Log, TEXT("UW_LootNotification - Set ItemAmountText: %s"), *AmountStr);
	}

	// Set item icon
	if (UImage* ItemIconImage = Cast<UImage>(GetWidgetFromName(TEXT("ItemIcon"))))
	{
		if (!ItemTexture.IsNull())
		{
			UTexture2D* LoadedTexture = ItemTexture.LoadSynchronous();
			if (LoadedTexture)
			{
				ItemIconImage->SetBrushFromTexture(LoadedTexture);
				UE_LOG(LogTemp, Log, TEXT("UW_LootNotification - Set ItemIcon from texture"));
			}
		}
	}
}
void UW_LootNotification::EventFinished_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LootNotification::EventFinished_Implementation"));

	// Play fade out animation if it exists
	if (UWidgetAnimation* FadeOutAnim = FindAnimationByName(TEXT("FadeOut")))
	{
		// Bind to animation finished event
		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UW_LootNotification::OnFadeOutComplete);
		BindToAnimationFinished(FadeOutAnim, AnimFinishedDelegate);

		PlayAnimation(FadeOutAnim);
		UE_LOG(LogTemp, Log, TEXT("UW_LootNotification - Playing FadeOut animation"));
	}
	else
	{
		// No fade out animation, remove immediately
		OnFadeOutComplete();
	}
}

void UW_LootNotification::OnFadeOutComplete()
{
	UE_LOG(LogTemp, Log, TEXT("UW_LootNotification::OnFadeOutComplete - Removing from parent"));
	RemoveFromParent();
}

UWidgetAnimation* UW_LootNotification::FindAnimationByName(const FName& AnimName) const
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
