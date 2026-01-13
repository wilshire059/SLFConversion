// W_BigScreenMessage.cpp
// C++ Widget implementation for W_BigScreenMessage
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_BigScreenMessage.h"
#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetTree.h"
#include "Blueprint/WidgetBlueprintGeneratedClass.h"

UW_BigScreenMessage::UW_BigScreenMessage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, MessageTextBlock(nullptr)
	, BgMsgTextBlock(nullptr)
	, FadeSequentialAnim(nullptr)
{
}

void UW_BigScreenMessage::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_BigScreenMessage::NativeConstruct"));
}

void UW_BigScreenMessage::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_BigScreenMessage::NativeDestruct"));
}

void UW_BigScreenMessage::CacheWidgetReferences()
{
	// Cache Message TextBlock
	MessageTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("Message")));
	if (!MessageTextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BigScreenMessage] Message TextBlock not found!"));
	}

	// Cache BgMsg TextBlock (background message)
	BgMsgTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("BgMsg")));
	if (!BgMsgTextBlock)
	{
		UE_LOG(LogTemp, Warning, TEXT("[BigScreenMessage] BgMsg TextBlock not found!"));
	}

	// Find animation by name - UMG animations are accessible via GetAnimation
	// Note: Animation must be created in Blueprint UMG designer
	UE_LOG(LogTemp, Log, TEXT("[BigScreenMessage] Widget references cached - Message: %s, BgMsg: %s"),
		MessageTextBlock ? TEXT("Found") : TEXT("Missing"),
		BgMsgTextBlock ? TEXT("Found") : TEXT("Missing"));
}

void UW_BigScreenMessage::EventShowMessage_Implementation(const FText& InMessage, UMaterialInterface* GradientMaterial, bool HasBackdrop, double AnimationRateScale)
{
	UE_LOG(LogTemp, Log, TEXT("UW_BigScreenMessage::EventShowMessage - Message: %s, HasBackdrop: %s, Rate: %.2f"),
		*InMessage.ToString(),
		HasBackdrop ? TEXT("true") : TEXT("false"),
		AnimationRateScale);

	// Make this widget visible
	SetVisibility(ESlateVisibility::Visible);

	// Set the message text
	if (MessageTextBlock)
	{
		MessageTextBlock->SetText(InMessage);
		UE_LOG(LogTemp, Log, TEXT("[BigScreenMessage] Set Message text to: %s"), *InMessage.ToString());

		// Apply gradient material if provided
		if (GradientMaterial)
		{
			FSlateFontInfo FontInfo = MessageTextBlock->GetFont();
			FontInfo.FontMaterial = GradientMaterial;
			MessageTextBlock->SetFont(FontInfo);
		}
	}

	// Set backdrop visibility
	if (BgMsgTextBlock)
	{
		BgMsgTextBlock->SetText(InMessage);
		BgMsgTextBlock->SetVisibility(HasBackdrop ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
		UE_LOG(LogTemp, Log, TEXT("[BigScreenMessage] BgMsg visibility: %s"),
			HasBackdrop ? TEXT("Visible") : TEXT("Collapsed"));
	}

	// Play the fade animation if available
	// Try to find and play the FadeSequential animation
	UWidgetAnimation* FadeAnim = nullptr;

	// Get all animations and find FadeSequential
	TArray<UWidgetAnimation*> Animations;
	UWidgetBlueprintGeneratedClass* WidgetClass = Cast<UWidgetBlueprintGeneratedClass>(GetClass());
	if (WidgetClass)
	{
		Animations = WidgetClass->Animations;
		for (UWidgetAnimation* Anim : Animations)
		{
			if (Anim && Anim->GetFName().ToString().Contains(TEXT("FadeSequential")))
			{
				FadeAnim = Anim;
				break;
			}
		}
	}

	if (FadeAnim)
	{
		// Check if animation is already playing
		if (!IsAnimationPlaying(FadeAnim))
		{
			float PlaybackSpeed = FMath::Max(0.1f, (float)AnimationRateScale);
			PlayAnimation(FadeAnim, 0.0f, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
			UE_LOG(LogTemp, Log, TEXT("[BigScreenMessage] Playing FadeSequential animation at speed %.2f"), PlaybackSpeed);
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("[BigScreenMessage] FadeSequential animation already playing"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[BigScreenMessage] FadeSequential animation not found! Message will show without animation."));
	}
}
