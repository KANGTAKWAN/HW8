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
	// ������ ���ڸ����� ȸ���ϴ� �ӵ� (deg/sec)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float SpinSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float MoveSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float MoveDistance;

	float Direction;
	FVector StartLocation;
	
	// Wave 3 �� ���� ��� Ȱ��ȭ ����
	bool bMovingMode;

private:
	// ������ ����(Spin) ������Ʈ �Լ�
	void UpdateSpin(float DeltaTime);

	// �̵� ������Ʈ �Լ�
	void Move(float DeltaTime);
};