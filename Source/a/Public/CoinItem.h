#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "CoinItem.generated.h"

UCLASS()
class A_API ACoinItem : public ABaseItem
{
	GENERATED_BODY()
public:
	ACoinItem();
	void SetWave3MoveActive(bool bActive);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item")
	int32 PointValue;
	virtual void ActivateItem(AActor* Activator) override;
	// 코인의 제자리에서 회전하는 속도 (deg/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float SpinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float MoveDistance;

	float Direction;
	FVector StartLocation;
	
	// Wave 3 시 무브 모드 활성화 여부
	bool bMovingMode;

private:
	// 코인의 자전(Spin) 업데이트 함수
	void UpdateSpin(float DeltaTime);

	// 이동 업데이트 함수
	void Move(float DeltaTime);
};