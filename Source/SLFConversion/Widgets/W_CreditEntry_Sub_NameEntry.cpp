// W_CreditEntry_Sub_NameEntry.cpp
// C++ Widget implementation for W_CreditEntry_Sub_NameEntry
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session

#include "Widgets/W_CreditEntry_Sub_NameEntry.h"

UW_CreditEntry_Sub_NameEntry::UW_CreditEntry_Sub_NameEntry(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UW_CreditEntry_Sub_NameEntry::NativeConstruct()
{
	Super::NativeConstruct();

	// Cache widget references
	CacheWidgetReferences();

	UE_LOG(LogTemp, Log, TEXT("UW_CreditEntry_Sub_NameEntry::NativeConstruct"));
}

void UW_CreditEntry_Sub_NameEntry::NativeDestruct()
{
	Super::NativeDestruct();

	UE_LOG(LogTemp, Log, TEXT("UW_CreditEntry_Sub_NameEntry::NativeDestruct"));
}

void UW_CreditEntry_Sub_NameEntry::CacheWidgetReferences()
{
	// TODO: Cache any widget references needed for logic
}

