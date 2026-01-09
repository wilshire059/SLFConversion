// W_GenericError.cpp
// C++ Widget implementation for W_GenericError
//
// 20-PASS VALIDATION: 2026-01-08

#include "Widgets/W_GenericError.h"
#include "Widgets/W_GenericButton.h"
#include "Components/TextBlock.h"

UW_GenericError::UW_GenericError(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_GenericError::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericError::NativeConstruct"));
}

void UW_GenericError::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_GenericError::NativeDestruct"));
}

void UW_GenericError::CacheWidgetReferences()
{
	// Widget references are found via GetWidgetFromName when needed
}

/**
 * EventErrorOkPressed - Handle OK button press
 *
 * Blueprint Logic:
 * 1. Broadcast OnErrorMessageDismissed
 */
void UW_GenericError::EventErrorOkPressed_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericError::EventErrorOkPressed_Implementation"));

	// Broadcast the dismissed event
	OnErrorMessageDismissed.Broadcast();
}

/**
 * EventSetErrorMessage - Set and display error message
 *
 * Blueprint Logic:
 * 1. Set Message variable
 * 2. Set EntryDescriptionText_1 text to the message
 * 3. Select the OK button
 */
void UW_GenericError::EventSetErrorMessage_Implementation(const FText& InMessage)
{
	UE_LOG(LogTemp, Log, TEXT("UW_GenericError::EventSetErrorMessage - %s"), *InMessage.ToString());

	// Set Message variable
	Message = InMessage;

	// Set the error text on EntryDescriptionText_1
	if (UTextBlock* ErrorText = Cast<UTextBlock>(GetWidgetFromName(TEXT("EntryDescriptionText_1"))))
	{
		ErrorText->SetText(InMessage);
	}

	// Select the OK button (W_GB_OK)
	if (UW_GenericButton* OkButton = Cast<UW_GenericButton>(GetWidgetFromName(TEXT("W_GB_OK"))))
	{
		OkButton->SetButtonSelected(true);
	}
}
