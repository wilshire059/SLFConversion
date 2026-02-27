// SLFEnemySpawnPoint.cpp

#include "Blueprints/Actors/SLFEnemySpawnPoint.h"
#include "Components/SphereComponent.h"
#include "Components/BillboardComponent.h"
#include "Blueprints/SLFSoulslikeEnemy.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

ASLFEnemySpawnPoint::ASLFEnemySpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	// Root scene component
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

#if WITH_EDITORONLY_DATA
	// Editor sprite for visibility in editor
	EditorSprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorSprite"));
	if (EditorSprite)
	{
		EditorSprite->SetupAttachment(Root);
		EditorSprite->SetHiddenInGame(true);
	}
#endif

	// Spawn radius visualization (editor only, hidden in game)
	SpawnRadiusVisual = CreateDefaultSubobject<USphereComponent>(TEXT("SpawnRadiusVisual"));
	if (SpawnRadiusVisual)
	{
		SpawnRadiusVisual->SetupAttachment(Root);
		SpawnRadiusVisual->SetSphereRadius(SpawnRadius);
		SpawnRadiusVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SpawnRadiusVisual->SetHiddenInGame(true);
		SpawnRadiusVisual->ShapeColor = FColor::Red;
	}
}

void ASLFEnemySpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay && EnemyClass)
	{
		SpawnEnemy();
	}
}

ACharacter* ASLFEnemySpawnPoint::SpawnEnemy()
{
	if (!EnemyClass || !GetWorld())
	{
		return nullptr;
	}

	// Don't spawn if one already exists and is alive
	if (SpawnedEnemy && !bEnemyKilled)
	{
		return SpawnedEnemy;
	}

	FVector SpawnLoc = GetRandomSpawnLocation();
	FRotator SpawnRot = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	SpawnParams.Owner = this;

	ACharacter* NewEnemy = GetWorld()->SpawnActor<ACharacter>(EnemyClass, SpawnLoc, SpawnRot, SpawnParams);
	if (NewEnemy)
	{
		SpawnedEnemy = NewEnemy;
		bEnemyKilled = false;

		// Set patrol path if this is a soulslike enemy
		ASLFSoulslikeEnemy* SoulsEnemy = Cast<ASLFSoulslikeEnemy>(NewEnemy);
		if (SoulsEnemy && PatrolPath)
		{
			SoulsEnemy->PatrolPath = PatrolPath;
		}

		// Listen for death
		NewEnemy->OnDestroyed.AddDynamic(this, &ASLFEnemySpawnPoint::OnSpawnedEnemyDeath);

		UE_LOG(LogTemp, Log, TEXT("SpawnPoint [%s]: Spawned %s at %s"),
			*GetName(), *EnemyClass->GetName(), *SpawnLoc.ToString());
	}

	return NewEnemy;
}

void ASLFEnemySpawnPoint::DespawnEnemy()
{
	if (SpawnedEnemy)
	{
		SpawnedEnemy->OnDestroyed.RemoveDynamic(this, &ASLFEnemySpawnPoint::OnSpawnedEnemyDeath);
		SpawnedEnemy->Destroy();
		SpawnedEnemy = nullptr;
	}
}

void ASLFEnemySpawnPoint::OnPlayerRested()
{
	if (bRespawnOnRest && bEnemyKilled)
	{
		bEnemyKilled = false;
		SpawnedEnemy = nullptr;
		SpawnEnemy();
	}
}

bool ASLFEnemySpawnPoint::IsEnemyAlive() const
{
	return SpawnedEnemy != nullptr && !bEnemyKilled;
}

void ASLFEnemySpawnPoint::OnSpawnedEnemyDeath(AActor* DeadActor)
{
	if (DeadActor == SpawnedEnemy)
	{
		bEnemyKilled = true;
		SpawnedEnemy = nullptr;
		UE_LOG(LogTemp, Log, TEXT("SpawnPoint [%s]: Enemy killed"), *GetName());
	}
}

FVector ASLFEnemySpawnPoint::GetRandomSpawnLocation() const
{
	if (SpawnRadius <= 0.0f)
	{
		return GetActorLocation();
	}

	FVector2D RandCircle = FMath::RandPointInCircle(SpawnRadius);
	return GetActorLocation() + FVector(RandCircle.X, RandCircle.Y, 0.0f);
}
