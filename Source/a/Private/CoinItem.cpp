#include "CoinItem.h"
#include "Engine/World.h"
#include "MyGameState.h"

ACoinItem::ACoinItem()
	: SpinSpeed(180.f)
	, MoveSpeed(200.f)
	, MoveDistance(200.f)
	, Direction(1.0f)
	, bMovingMode(false)
	, PointValue(10)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACoinItem::BeginPlay()
{
	Super::BeginPlay();

	StartLocation = GetActorLocation();
}

void ACoinItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateSpin(DeltaTime);

	if (bMovingMode)
	{
		Move(DeltaTime);
	}
}

void ACoinItem::UpdateSpin(float DeltaTime)
{
	// ������ Yaw�� �������� ȸ����Ŵ
	AddActorLocalRotation(FRotator(0.f, SpinSpeed * DeltaTime, 0.f));
}

void ACoinItem::Move(float DeltaTime)
{
	float CurrentDistance = (GetActorLocation() - StartLocation).Length();

	if (MoveDistance <= CurrentDistance)
	{
		StartLocation = GetActorLocation();
		Direction *= -1.0f;
	}

	FVector DeltaLocation = GetActorUpVector() * Direction * MoveSpeed * DeltaTime;
	AddActorLocalOffset(DeltaLocation);
}

void ACoinItem::ActivateItem(AActor* Activator)
{
	Super::ActivateItem(Activator);

	// �÷��̾ ������ ȹ���� ���
	if (Activator && Activator->ActorHasTag("Player"))
	{
		if (UWorld* World = GetWorld())
		{
			if (AMyGameState* GameState = World->GetGameState<AMyGameState>())
			{
				GameState->AddScore(PointValue);
				GameState->OnCoinCollected();
			}
		}
		DestroyItem();
	}
}

void ACoinItem::SetWave3MoveActive(bool bActive)
{
	bMovingMode = bActive;
}
