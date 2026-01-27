// B_DeathCurrency.cpp
// C++ implementation for Blueprint B_DeathCurrency
//
// 20-PASS VALIDATION: 2026-01-18 Session
// Source: BlueprintDNA/Blueprint/B_DeathCurrency.json
//
// OnInteract Logic:
// 1. Get controller from interacting actor
// 2. Get InventoryManager and add currency
// 3. Deactivate Niagara effect
// 4. Destroy the actor

#include "Blueprints/B_DeathCurrency.h"
#include "Blueprints/BFL_StructConversion.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "NiagaraComponent.h"
#include "Components/AC_InventoryManager.h"
#include "Interfaces/BPI_Controller.h"

AB_DeathCurrency::AB_DeathCurrency()
{
	CurrencyAmount = 0;
	CachedDeathCurrencyNiagara = nullptr;
}

void AB_DeathCurrency::BeginPlay()
{
	Super::BeginPlay();

	// Cache Niagara component from Blueprint SCS
	TArray<UNiagaraComponent*> NiagaraComps;
	GetComponents<UNiagaraComponent>(NiagaraComps);
	for (UNiagaraComponent* NC : NiagaraComps)
	{
		if (NC && NC->GetName().Contains(TEXT("DeathCurrency")))
		{
			CachedDeathCurrencyNiagara = NC;
			break;
		}
	}

	// Fallback: just get the first Niagara component
	if (!CachedDeathCurrencyNiagara && NiagaraComps.Num() > 0)
	{
		CachedDeathCurrencyNiagara = NiagaraComps[0];
	}

	UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] BeginPlay - CurrencyAmount: %d, Niagara: %s"),
		CurrencyAmount, CachedDeathCurrencyNiagara ? *CachedDeathCurrencyNiagara->GetName() : TEXT("None"));
}

void AB_DeathCurrency::OnInteract_Implementation(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] OnInteract - InteractingActor: %s, CurrencyAmount: %d"),
		InteractingActor ? *InteractingActor->GetName() : TEXT("None"), CurrencyAmount);

	if (!IsValid(InteractingActor))
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_DeathCurrency] OnInteract - Invalid interacting actor"));
		return;
	}

	// Get controller from interacting actor (pawn)
	AController* Controller = nullptr;
	if (APawn* Pawn = Cast<APawn>(InteractingActor))
	{
		Controller = Pawn->GetController();
	}
	else
	{
		// Try GetInstigatorController
		Controller = InteractingActor->GetInstigatorController();
	}

	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Warning, TEXT("[B_DeathCurrency] OnInteract - No controller found"));
		return;
	}

	// Call AdjustCurrency via interface on controller
	if (Controller->GetClass()->ImplementsInterface(UBPI_Controller::StaticClass()))
	{
		IBPI_Controller::Execute_AdjustCurrency(Controller, CurrencyAmount);
		UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] Called AdjustCurrency(%d) on controller via BPI_Controller"), CurrencyAmount);
	}
	else
	{
		// Fallback: Try to get InventoryManager from pawn
		APawn* Pawn = Controller->GetPawn();
		if (IsValid(Pawn))
		{
			if (UAC_InventoryManager* InventoryManager = Pawn->FindComponentByClass<UAC_InventoryManager>())
			{
				InventoryManager->AdjustCurrency(CurrencyAmount);
				UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] Called InventoryManager->AdjustCurrency(%d)"), CurrencyAmount);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[B_DeathCurrency] No InventoryManager found on pawn"));
			}
		}
	}

	// Deactivate Niagara effect
	if (IsValid(CachedDeathCurrencyNiagara))
	{
		CachedDeathCurrencyNiagara->Deactivate();
		UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] Deactivated Niagara effect"));
	}

	// Store current location for potential animation (future)
	CacheTransform = GetActorTransform();
	PlayerLocation = InteractingActor->GetActorLocation();

	// Destroy this actor
	// Note: In Blueprint this happens after a timeline animation
	// For now, we destroy immediately - animation can be added later
	UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] Destroying actor"));
	Destroy();
}

void AB_DeathCurrency::OnSpawnedFromSave_Implementation(const FGuid& Id, const FInstancedStruct& CustomData)
{
	Super::OnSpawnedFromSave_Implementation(Id, CustomData);

	UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] OnSpawnedFromSave - ID: %s"), *Id.ToString());

	// Extract currency amount from CustomData using the FInt wrapper
	// The Blueprint stores an FSLFInt in the InstancedStruct
	if (CustomData.IsValid())
	{
		int32 SavedAmount = 0;
		if (UBFL_StructConversion::ConvertToFInt(CustomData, SavedAmount))
		{
			CurrencyAmount = SavedAmount;
			UE_LOG(LogTemp, Log, TEXT("[B_DeathCurrency] Restored CurrencyAmount: %d"), CurrencyAmount);
		}
	}
}

