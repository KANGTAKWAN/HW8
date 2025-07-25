#include "MyGameState.h"
#include "MyGameInstance.h"
#include "MyPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnVolume.h"
#include "CoinItem.h"
#include "Components/TextBlock.h"
#include "Blueprint/UserWidget.h"
#include "SpikeTrap.h"

AMyGameState::AMyGameState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Score = 0;
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;
	LevelDuration = 30.0f;
	CurrentLevelIndex = 0;
	MaxLevels = 3;
	CurrentLevelIndex = 0;
	MaxWaves = 3;
	WaveDuration = 10.0f;
	ItemsToSpawnPerWave = {20, 30, 40};
	SpikeTrapClass = nullptr;
}
void AMyGameState::BeginPlay()
{
	Super::BeginPlay();
	//UKismetSystemLibrary::PrintString(this,FString::Printf(TEXT("beginplay  %d %d"),UE_BUILD_DEBUG,UE_BUILD_DEVELOPMENT),true,true,FLinearColor(1,0,0),60.f);
	// UpdateHUD();
	StartLevel();
	GetWorldTimerManager().SetTimer(
			HUDUpdateTimerHandle,
			this,
			&AMyGameState::UpdateHUD,
			0.1f,
			true
		);
}
int32 AMyGameState::GetScore() const
{
	return Score;
}
void AMyGameState::AddScore(int32 Amount)
{
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			MyGameInstance->AddToScore(Amount);
		}
	}
}
void AMyGameState::StartLevel()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->ShowGameHUD();
		}
	}
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{
			CurrentLevelIndex = MyGameInstance->CurrentLevelIndex;
		}
	}

	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;


	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);

	const int32 ItemToSpawn = 20;
	for (int32 i = 0; i < ItemToSpawn; i++)
	{
		if (FoundVolumes.Num() > 0)
		{
			ASpawnVolume* SpawnVolume = Cast<ASpawnVolume>(FoundVolumes[0]);
			if (SpawnVolume)
			{
				AActor* SpawnedActor = SpawnVolume->SpawnRandomItem();
				if (SpawnedActor && SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;  
				}
			}
		}
	}
	

	GetWorldTimerManager().SetTimer(LevelTimerHandle,this,&AMyGameState::OnLevelTimeUp,LevelDuration,false);
	CurrentWaveIndex = 0;
	StartWave();
}

void AMyGameState::StartWave()
{
	OnWaveStarted.Broadcast(CurrentWaveIndex);

	// 코인 카운트 초기화
	SpawnedCoinCount = 0;
	CollectedCoinCount = 0;

	// 이전 Wave 아이템 제거
	for (AActor* Item : CurrentWaveItems)
	{
		if (Item && Item->IsValidLowLevelFast())
		{
			Item->Destroy();
		}
	}
	CurrentWaveItems.Empty();

	// 이번 Wave에 스폰할 아이템 개수 결정
	int32 ItemToSpawn = (ItemsToSpawnPerWave.IsValidIndex(CurrentWaveIndex)) ? ItemsToSpawnPerWave[CurrentWaveIndex] : 20;

	// SpawnVolume을 이용해 아이템 스폰
	if (ASpawnVolume* SpawnVolume = GetSpawnVolume())
	{
		for (int32 i = 0; i < ItemToSpawn; i++)
		{
			if (AActor* SpawnedActor = SpawnVolume->SpawnRandomItem())
			{
				if (SpawnedActor->IsA(ACoinItem::StaticClass()))
				{
					SpawnedCoinCount++;
				}

				CurrentWaveItems.Add(SpawnedActor);
			}
		}
	}

	// Wave별 환경 변화
	if (CurrentWaveIndex == 1)
	{
		EnableWave2();
	}
	else if (CurrentWaveIndex == 2)
	{
		EnableWave3();
	}

	// Wave 타이머 시작 (WaveDuration 후 OnWaveTimeUp 호출)
	GetWorldTimerManager().SetTimer(
		WaveTimerHandle,
		this,
		&ThisClass::OnWaveTimeUp,
		WaveDuration,
		false
	);
}

void AMyGameState::OnWaveTimeUp()
{
	EndWave();
}

void AMyGameState::EndWave()
{
	// 남은 Wave 타이머 제거
	GetWorldTimerManager().ClearTimer(WaveTimerHandle);

	// Wave3에서 활성화한 코인 이동 비활성화
	if (CurrentWaveIndex == 2)
	{
		SetAllCoinsMove(false);
	}

	// 다음 Wave 진행 또는 Level 종료
	++CurrentWaveIndex;
	if (CurrentWaveIndex >= MaxWaves)
	{
		EndLevel();
	}
	else
	{
		StartWave();
	}
}

void AMyGameState::OnLevelTimeUp()
{
		EndLevel();
}

void AMyGameState::OnCoinCollected()
{
		CollectedCoinCount++;	
		// UE_LOG(LogTemp, Warning, TEXT("Coin Collected: %d / %d"), CollectedCoinCount,SpawnedCoinCount)
		if (SpawnedCoinCount > 0 && CollectedCoinCount >= SpawnedCoinCount)
		{
				EndLevel();
		}
}

void AMyGameState::EndLevel()
{
	GetWorldTimerManager().ClearTimer(LevelTimerHandle);
	
	if (UGameInstance* GameInstance = GetGameInstance())
	{
		UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
		if (MyGameInstance)
		{	
			AddScore(Score);
			CurrentLevelIndex++;
			MyGameInstance->CurrentLevelIndex = CurrentLevelIndex;
			
			if (CurrentLevelIndex >= MaxLevels)
			{	 
				OnGameOver();
				return;
			}
			
			if (LevelMapNames.IsValidIndex(CurrentLevelIndex))
			{
				UGameplayStatics::OpenLevel(GetWorld(), LevelMapNames[CurrentLevelIndex]);
			}
			else
			{
				OnGameOver();
			}
		}
	}
}
void AMyGameState::OnGameOver()
{	
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			MyPlayerController->SetPause(true);
			MyPlayerController->ShowMainMenu(true);
		}
	}
}
void AMyGameState::UpdateHUD()
{
	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AMyPlayerController* MyPlayerController = Cast<AMyPlayerController>(PlayerController))
		{
			if (UUserWidget* HUDWidget = MyPlayerController->GetHUDWidget())
			{
				if (UTextBlock* TimeText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Time"))))
				{
					float RemainingTime = GetWorldTimerManager().GetTimerRemaining(LevelTimerHandle);
					TimeText->SetText(FText::FromString(FString::Printf(TEXT("Time: %.1f"), RemainingTime)));
				}
				
				if (UTextBlock* ScoreText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Score"))))
				{
					if (UGameInstance* GameInstance = GetGameInstance())
					{
						UMyGameInstance* MyGameInstance = Cast<UMyGameInstance>(GameInstance);
						if (MyGameInstance)
						{
							ScoreText->SetText(FText::FromString(FString::Printf(TEXT("Score: %d"), MyGameInstance->TotalScore)));
						}
					}
				}
				
				if (UTextBlock* LevelIndexText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Level"))))
				{
					LevelIndexText->SetText(FText::FromString(FString::Printf(TEXT("Level: %d"), CurrentLevelIndex + 1)));
				}
				if (UTextBlock* WaveText = Cast<UTextBlock>(HUDWidget->GetWidgetFromName(TEXT("Wave"))))
				{
					WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave: %d / %d"), CurrentWaveIndex + 1, MaxWaves)));
				}
			}
		}
	}
}
void AMyGameState::EnableWave2()
{
	//Wave 2: 스파이크 트랩
	const FString Msg = TEXT("Wave 2: Spawning and activating 5 Spike Traps!");
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Msg);
	}

	if (ASpawnVolume* SpawnVolume = GetSpawnVolume())
	{
		if (SpikeTrapClass)
		{
			for (int32 i = 0; i < 15; i++)
			{
				if (AActor* SpawnedActor = SpawnVolume->SpawnItem(SpikeTrapClass))
				{
					if (ASpikeTrap* Trap = Cast<ASpikeTrap>(SpawnedActor))
					{
						Trap->ActivateTrap();
					}
				}
			}
		}
	}
}

void AMyGameState::EnableWave3()
{
	//Wave 3: 움직이는 코인
	const FString Msg = TEXT("Wave 3: Spawning coins that orbit around!");
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, Msg);
	}

	SetAllCoinsMove(true);
}

void AMyGameState::SetAllCoinsMove(bool bActive)
{
	for (AActor* CoinActor : CurrentWaveItems)
	{
		if (ACoinItem* Coin = Cast<ACoinItem>(CoinActor))
		{
			Coin->SetWave3MoveActive(bActive);
		}
	}
}

ASpawnVolume* AMyGameState::GetSpawnVolume() const
{
	TArray<AActor*> FoundVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnVolume::StaticClass(), FoundVolumes);
	return (FoundVolumes.Num() > 0) ? Cast<ASpawnVolume>(FoundVolumes[0]) : nullptr;
}

AMyPlayerController* AMyGameState::GetMyPlayerController() const
{
	return Cast<AMyPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
}

UMyGameInstance* AMyGameState::GetMyGameInstance() const
{
	return Cast<UMyGameInstance>(GetGameInstance());
}
