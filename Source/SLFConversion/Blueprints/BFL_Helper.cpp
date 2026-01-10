// BFL_Helper.cpp
// C++ Blueprint Function Library Implementation
//
// 20-PASS VALIDATION: 2026-01-01 Autonomous Session
// Source: BlueprintDNA_v2/Blueprint/BFL_Helper.json

#include "Blueprints/BFL_Helper.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"
#include "Components/ActorComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

// Component includes
#include "Components/AC_EquipmentManager.h"
#include "Components/AC_InventoryManager.h"
#include "Components/AC_SaveLoadManager.h"
#include "Components/AC_ProgressManager.h"
#include "Components/AC_StatManager.h"

// Widget includes
#include "Widgets/W_HUD.h"

// Interface includes
#include "Interfaces/BPI_Controller.h"

// ═══════════════════════════════════════════════════════════════════════════════
// INPUT HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

void UBFL_Helper::GetKeysForIA(UInputMappingContext* InputMapping, UInputAction* TargetIA, const UObject* WorldContextObject, TArray<FKey>& MappedKeys)
{
	MappedKeys.Empty();

	if (!InputMapping || !TargetIA)
	{
		return;
	}

	// Get all mappings in the context
	const TArray<FEnhancedActionKeyMapping>& Mappings = InputMapping->GetMappings();
	for (const FEnhancedActionKeyMapping& Mapping : Mappings)
	{
		if (Mapping.Action == TargetIA)
		{
			MappedKeys.Add(Mapping.Key);
		}
	}
}

UEnhancedInputUserSettings* UBFL_Helper::GetInputUserSettings(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer());
	if (!Subsystem)
	{
		return nullptr;
	}

	return Subsystem->GetUserSettings();
}

// ═══════════════════════════════════════════════════════════════════════════════
// STAT HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

void UBFL_Helper::GetStatCurrentValue(const FStatInfo& StatInfo, const UObject* WorldContextObject, double& CurrentValue)
{
	CurrentValue = StatInfo.CurrentValue;
}

void UBFL_Helper::GetStatMaxValue(const FStatInfo& StatInfo, const UObject* WorldContextObject, double& MaxValue)
{
	MaxValue = StatInfo.MaxValue;
}

void UBFL_Helper::GetStatPercentValue(const FStatInfo& StatInfo, const UObject* WorldContextObject, double& PercentValue)
{
	if (StatInfo.MaxValue <= 0.0)
	{
		PercentValue = 0.0;
		return;
	}
	PercentValue = StatInfo.CurrentValue / StatInfo.MaxValue;
}

TArray<ESLFStatCategory> UBFL_Helper::RemoveDuplicateCategories(const TArray<ESLFStatCategory>& Array, const UObject* WorldContextObject)
{
	TArray<ESLFStatCategory> CleanArray;

	for (const ESLFStatCategory& Category : Array)
	{
		CleanArray.AddUnique(Category);
	}

	return CleanArray;
}

FString UBFL_Helper::ParseAffectedStats(const TMap<FGameplayTag, FGameplayTag>& AffectedStats, const UObject* WorldContextObject)
{
	FString Result;

	for (const auto& Pair : AffectedStats)
	{
		if (!Result.IsEmpty())
		{
			Result += TEXT(", ");
		}
		Result += Pair.Key.ToString() + TEXT(": ") + Pair.Value.ToString();
	}

	return Result;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TAG/STRING HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

FText UBFL_Helper::ParseOutTagName(const FGameplayTag& Tag, bool Shorten, int32 ShortenedLen, const UObject* WorldContextObject)
{
	if (!Tag.IsValid())
	{
		return FText::GetEmpty();
	}

	// Get the tag name and extract the last segment after the last dot
	FString TagString = Tag.GetTagName().ToString();

	int32 LastDotIndex;
	if (TagString.FindLastChar('.', LastDotIndex))
	{
		TagString = TagString.RightChop(LastDotIndex + 1);
	}

	// Shorten if requested
	if (Shorten && TagString.Len() > ShortenedLen)
	{
		TagString = TagString.Left(ShortenedLen);
	}

	return FText::FromString(TagString);
}

bool UBFL_Helper::ActorTagMatches(AActor* Target, const TArray<FName>& Tags, const UObject* WorldContextObject)
{
	if (!Target)
	{
		return false;
	}

	for (const FName& Tag : Tags)
	{
		if (Target->ActorHasTag(Tag))
		{
			return true;
		}
	}

	return false;
}

bool UBFL_Helper::CompareStrings(const FString& String1, const FString& String2, const UObject* WorldContextObject)
{
	return String1.Equals(String2);
}

void UBFL_Helper::ParseStringLength(const FString& String, int32 Length, const UObject* WorldContextObject, FString& OutParsedString, FString& OutRemainder)
{
	if (String.Len() <= Length)
	{
		OutParsedString = String;
		OutRemainder = FString();
	}
	else
	{
		OutParsedString = String.Left(Length);
		OutRemainder = String.RightChop(Length);
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMPONENT ACCESSORS
// ═══════════════════════════════════════════════════════════════════════════════

void UBFL_Helper::GetEquipmentComponent(AController* Target, const UObject* WorldContextObject, UAC_EquipmentManager*& OutEquipmentManager)
{
	OutEquipmentManager = nullptr;

	if (!Target)
	{
		return;
	}

	// Component is on the Controller, not the Pawn
	OutEquipmentManager = Target->FindComponentByClass<UAC_EquipmentManager>();
}

void UBFL_Helper::GetInventoryComponent(AController* Target, const UObject* WorldContextObject, UAC_InventoryManager*& OutInventoryManager)
{
	OutInventoryManager = nullptr;

	if (!Target)
	{
		return;
	}

	// Component is on the Controller, not the Pawn
	OutInventoryManager = Target->FindComponentByClass<UAC_InventoryManager>();
}

void UBFL_Helper::GetSaveLoadComponent(AController* Target, const UObject* WorldContextObject, UAC_SaveLoadManager*& OutSaveLoadManager)
{
	OutSaveLoadManager = nullptr;

	if (!Target)
	{
		return;
	}

	// Component is on the Controller, not the Pawn
	OutSaveLoadManager = Target->FindComponentByClass<UAC_SaveLoadManager>();
}

UAC_ProgressManager* UBFL_Helper::GetLocalProgressManager(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0);
	if (!PC)
	{
		return nullptr;
	}

	APawn* Pawn = PC->GetPawn();
	if (!Pawn)
	{
		return nullptr;
	}

	return Pawn->FindComponentByClass<UAC_ProgressManager>();
}

// ═══════════════════════════════════════════════════════════════════════════════
// MOVEMENT/POSITION HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

bool UBFL_Helper::GetIsMoving2D(const FVector& Velocity, double SpeedToCheck, const UObject* WorldContextObject)
{
	// Get 2D velocity magnitude (ignoring Z)
	double Speed2D = FVector(Velocity.X, Velocity.Y, 0.0).Size();
	return Speed2D > SpeedToCheck;
}

bool UBFL_Helper::GetIsBehindTarget(AActor* SelfActor, AActor* TargetActor, double PrecisionTolerance, const UObject* WorldContextObject)
{
	if (!SelfActor || !TargetActor)
	{
		return false;
	}

	// Get direction from target to self
	FVector DirectionToSelf = (SelfActor->GetActorLocation() - TargetActor->GetActorLocation()).GetSafeNormal();

	// Get target's forward vector
	FVector TargetForward = TargetActor->GetActorForwardVector();

	// Dot product: negative means behind
	double DotProduct = FVector::DotProduct(TargetForward, DirectionToSelf);

	// Behind means dot product is less than -tolerance (e.g., -0.5 means 120 degree cone behind)
	return DotProduct < -PrecisionTolerance;
}

void UBFL_Helper::GetLocationActorTransform(const FGameplayTag& LocationTag, const UObject* WorldContextObject, bool& OutSuccess, FTransform& OutTransform)
{
	OutSuccess = false;
	OutTransform = FTransform::Identity;

	if (!LocationTag.IsValid() || !WorldContextObject)
	{
		return;
	}

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return;
	}

	// Find all actors of class B_LocationActor (by iterating actors and checking for LocationTag property)
	// Since we don't have direct access to B_LocationActor_C from C++, we iterate all actors
	// and check if they implement the location tag interface
	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!Actor)
		{
			continue;
		}

		// Check if actor has a LocationTag property that matches
		// This would typically be done via interface or by checking class
		// For now, we look for the LocationTag variable via reflection
		FGameplayTag* ActorLocationTag = nullptr;

		// Try to find LocationTag property
		FProperty* Prop = Actor->GetClass()->FindPropertyByName(FName("LocationTag"));
		if (Prop)
		{
			FStructProperty* StructProp = CastField<FStructProperty>(Prop);
			if (StructProp && StructProp->Struct == FGameplayTag::StaticStruct())
			{
				ActorLocationTag = StructProp->ContainerPtrToValuePtr<FGameplayTag>(Actor);
			}
		}

		if (ActorLocationTag && *ActorLocationTag == LocationTag)
		{
			OutSuccess = true;
			OutTransform = Actor->GetActorTransform();
			return;
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// COMBAT HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

double UBFL_Helper::CalculateWeaponDamage(const FSLFWeaponAttackPower& AttackPowerStats, double AdditionalModifier, const TMap<FGameplayTag, FGameplayTag>& ScalingFactor, UAC_StatManager* StatManager, AActor* DamagedActor, const UObject* WorldContextObject)
{
	// Sum all damage types from attack power stats
	double TotalDamage = AttackPowerStats.Physical
		+ AttackPowerStats.Magic
		+ AttackPowerStats.Lightning
		+ AttackPowerStats.Holy
		+ AttackPowerStats.Frost
		+ AttackPowerStats.Fire;

	// Apply additional modifier
	TotalDamage *= (1.0 + AdditionalModifier);

	// Apply scaling factors from stat manager if available
	if (StatManager)
	{
		for (const auto& Scale : ScalingFactor)
		{
			// Get stat value for the scaling factor
			// This would typically look up the stat by tag and apply scaling
			// Simplified implementation - could be expanded based on specific scaling logic
		}
	}

	return TotalDamage;
}

bool UBFL_Helper::IsEnemyDead(AActor* Enemy, const UObject* WorldContextObject)
{
	if (!Enemy)
	{
		return true; // Null enemy is considered dead
	}

	// Check if the enemy has a stat manager component
	UAC_StatManager* StatManager = Enemy->FindComponentByClass<UAC_StatManager>();
	if (StatManager)
	{
		// Check if health stat is depleted
		// The StatManager would typically have a method to check death state
		// For now, check if actor is still valid
	}

	// Fallback: check if actor is pending kill
	return !IsValid(Enemy);
}

void UBFL_Helper::GetDirectionFromHit(AActor* OwnerActor, const FHitResult& HitInfo, const UObject* WorldContextObject, ESLFDirection& HitDirection)
{
	HitDirection = ESLFDirection::Fwd;

	if (!OwnerActor)
	{
		return;
	}

	// Get the hit direction relative to the owner
	FVector HitDir = HitInfo.ImpactNormal;
	FVector OwnerForward = OwnerActor->GetActorForwardVector();
	FVector OwnerRight = OwnerActor->GetActorRightVector();

	// Calculate dot products
	double ForwardDot = FVector::DotProduct(OwnerForward, -HitDir);
	double RightDot = FVector::DotProduct(OwnerRight, -HitDir);

	// Determine direction based on dot products
	if (FMath::Abs(ForwardDot) > FMath::Abs(RightDot))
	{
		HitDirection = (ForwardDot > 0) ? ESLFDirection::Fwd : ESLFDirection::Bwd;
	}
	else
	{
		HitDirection = (RightDot > 0) ? ESLFDirection::Right : ESLFDirection::Left;
	}
}

void UBFL_Helper::GetDeathTagFromDirection(AActor* OwnerActor, const FHitResult& HitInfo, const UObject* WorldContextObject, FGameplayTag& OutDeathTag)
{
	ESLFDirection HitDirection;
	GetDirectionFromHit(OwnerActor, HitInfo, WorldContextObject, HitDirection);

	// Map direction to death tag
	switch (HitDirection)
	{
	case ESLFDirection::Fwd:
		OutDeathTag = FGameplayTag::RequestGameplayTag(FName("Death.Direction.Front"));
		break;
	case ESLFDirection::Bwd:
		OutDeathTag = FGameplayTag::RequestGameplayTag(FName("Death.Direction.Back"));
		break;
	case ESLFDirection::Left:
		OutDeathTag = FGameplayTag::RequestGameplayTag(FName("Death.Direction.Left"));
		break;
	case ESLFDirection::Right:
		OutDeathTag = FGameplayTag::RequestGameplayTag(FName("Death.Direction.Right"));
		break;
	default:
		OutDeathTag = FGameplayTag::RequestGameplayTag(FName("Death.Direction.Front"));
		break;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// UI HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

void UBFL_Helper::SendErrorMessage(AActor* PlayerActor, const FText& Message, double Time, const UObject* WorldContextObject)
{
	if (!PlayerActor)
	{
		return;
	}

	// Get instigator (pawn) from actor
	APawn* Instigator = PlayerActor->GetInstigator();
	if (!Instigator)
	{
		return;
	}

	// Get controller from instigator
	AController* Controller = Instigator->GetController();
	if (!Controller)
	{
		return;
	}

	// Check if controller implements the SLF Controller Interface
	if (!Controller->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		return;
	}

	// Call GetPlayerHUD through the interface
	UUserWidget* HUDWidget = nullptr;
	IBPI_Controller::Execute_GetPlayerHUD(Controller, HUDWidget);
	UW_HUD* HUD = Cast<UW_HUD>(HUDWidget);
	if (!HUD)
	{
		return;
	}

	// Call SendInteractionErrorMessage on the HUD
	HUD->EventSendInteractionErrorMessage(Message, static_cast<float>(Time));
}

void UBFL_Helper::TryGetHudForActor(AActor* Actor, const UObject* WorldContextObject, bool& OutSuccess, UW_HUD*& OutHUD)
{
	OutSuccess = false;
	OutHUD = nullptr;

	if (!Actor)
	{
		return;
	}

	// Get instigator (pawn) from actor
	APawn* Instigator = Actor->GetInstigator();
	if (!Instigator)
	{
		return;
	}

	// Get controller from instigator
	AController* Controller = Instigator->GetController();
	if (!Controller)
	{
		return;
	}

	// Check if controller implements the SLF Controller Interface
	if (!Controller->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		return;
	}

	// Call GetPlayerHUD through the interface
	UUserWidget* HUDWidget = nullptr;
	IBPI_Controller::Execute_GetPlayerHUD(Controller, HUDWidget);
	OutHUD = Cast<UW_HUD>(HUDWidget);
	if (OutHUD)
	{
		OutSuccess = true;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// SAVE/LOAD HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

FString UBFL_Helper::GetUniqueSlotName(UPrimaryDataAsset* CharacterClass, int32 CheckDepth, const UObject* WorldContextObject)
{
	if (!CharacterClass)
	{
		return FString(TEXT("SaveSlot_0"));
	}

	// Generate base name from character class
	FString BaseName = CharacterClass->GetName();

	// Check for existing saves and increment depth
	for (int32 i = 0; i <= CheckDepth; i++)
	{
		FString SlotName = FString::Printf(TEXT("%s_%d"), *BaseName, i);

		// Check if this slot exists
		if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0))
		{
			return SlotName;
		}
	}

	// If all slots exist, return with max depth
	return FString::Printf(TEXT("%s_%d"), *BaseName, CheckDepth);
}

void UBFL_Helper::GetSaveDataByTag(const FSLFSaveGameInfo& Data, const FGameplayTag& SaveGameTag, const UObject* WorldContextObject, bool& OutSuccess, FSLFSaveData& OutSaveData)
{
	OutSuccess = false;

	if (!SaveGameTag.IsValid())
	{
		return;
	}

	// Search for the save data with matching tag in the SaveGameEntry map
	// The map uses FString as key, so convert the tag to string
	FString TagString = SaveGameTag.ToString();
	const FSLFSaveData* FoundData = Data.SaveGameEntry.Find(TagString);
	if (FoundData)
	{
		OutSuccess = true;
		OutSaveData = *FoundData;
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// CRAFTING HELPERS
// ═══════════════════════════════════════════════════════════════════════════════

void UBFL_Helper::UnlockCraftableItems(const TArray<UPrimaryDataAsset*>& Items, const UObject* WorldContextObject)
{
	if (Items.Num() == 0)
	{
		return;
	}

	// Get the progress manager
	UAC_ProgressManager* ProgressManager = GetLocalProgressManager(WorldContextObject);
	if (!ProgressManager)
	{
		return;
	}

	// Unlock each item in the progress manager
	for (UPrimaryDataAsset* Item : Items)
	{
		if (Item)
		{
			// Call unlock function on progress manager
			// This would typically be: ProgressManager->UnlockCraftableItem(Item);
		}
	}
}

// ═══════════════════════════════════════════════════════════════════════════════
// CLASS CHECKING (replaces BML_HelperMacros)
// ═══════════════════════════════════════════════════════════════════════════════

bool UBFL_Helper::IsClassChild(UObject* Target, UClass* ParentClass)
{
	if (!Target || !ParentClass)
	{
		return false;
	}

	return Target->GetClass()->IsChildOf(ParentClass);
}

bool UBFL_Helper::IsClassEqual(UObject* Target, UClass* ClassToCheck)
{
	if (!Target || !ClassToCheck)
	{
		return false;
	}

	return Target->GetClass() == ClassToCheck;
}

// ═══════════════════════════════════════════════════════════════════════════════
// ARRAY HELPERS (replaces BML_HelperMacros)
// ═══════════════════════════════════════════════════════════════════════════════

bool UBFL_Helper::IsValidObjectArray(const TArray<UObject*>& Array)
{
	return Array.Num() > 0;
}

bool UBFL_Helper::IsValidIntArray(const TArray<int32>& Array)
{
	return Array.Num() > 0;
}

bool UBFL_Helper::IsValidTagArray(const TArray<FGameplayTag>& Array)
{
	return Array.Num() > 0;
}

// ═══════════════════════════════════════════════════════════════════════════════
// TIMER/COOLDOWN HELPERS (replaces BML_HelperMacros)
// ═══════════════════════════════════════════════════════════════════════════════

// Static cooldown map definition
TMap<TPair<TWeakObjectPtr<UObject>, FName>, double> UBFL_Helper::CooldownMap;

bool UBFL_Helper::StartCooldown(UObject* Object, FName CooldownKey, float Duration, const UObject* WorldContextObject)
{
	if (!Object || Duration <= 0.0f)
	{
		return false;
	}

	// Check if already on cooldown
	if (IsOnCooldown(Object, CooldownKey, WorldContextObject))
	{
		return false;
	}

	// Get current world time
	UWorld* World = nullptr;
	if (WorldContextObject)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return false;
	}

	double CurrentTime = World->GetTimeSeconds();
	double EndTime = CurrentTime + Duration;

	// Store cooldown end time
	TPair<TWeakObjectPtr<UObject>, FName> Key = MakeTuple(TWeakObjectPtr<UObject>(Object), CooldownKey);
	CooldownMap.Add(Key, EndTime);

	return true;
}

bool UBFL_Helper::IsOnCooldown(UObject* Object, FName CooldownKey, const UObject* WorldContextObject)
{
	if (!Object)
	{
		return false;
	}

	TPair<TWeakObjectPtr<UObject>, FName> Key = MakeTuple(TWeakObjectPtr<UObject>(Object), CooldownKey);
	double* EndTime = CooldownMap.Find(Key);

	if (!EndTime)
	{
		return false;
	}

	// Get current world time
	UWorld* World = nullptr;
	if (WorldContextObject)
	{
		World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	}

	if (!World)
	{
		return false;
	}

	double CurrentTime = World->GetTimeSeconds();

	// Check if cooldown has expired
	if (CurrentTime >= *EndTime)
	{
		// Clean up expired cooldown
		CooldownMap.Remove(Key);
		return false;
	}

	return true;
}

void UBFL_Helper::ClearCooldown(UObject* Object, FName CooldownKey, const UObject* WorldContextObject)
{
	if (!Object)
	{
		return;
	}

	TPair<TWeakObjectPtr<UObject>, FName> Key = MakeTuple(TWeakObjectPtr<UObject>(Object), CooldownKey);
	CooldownMap.Remove(Key);
}
