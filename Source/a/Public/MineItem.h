#pragma once

#include "CoreMinimal.h"
#include "BaseItem.h"
#include "MineItem.generated.h"

UCLASS()
class A_API AMineItem : public ABaseItem
{
    GENERATED_BODY()

public:
    AMineItem();

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float ExplosionDelay;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    float ExplosionRadius;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mine")
    int ExplosionDamage;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item|Component")
    USphereComponent* ExplosionCollision;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
    UParticleSystem* ExplosionParticle;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item|Effects")
    USoundBase* ExplosionSound;

    FTimerHandle ExplosionTimerHandle;

    virtual void ActivateItem(AActor* Activator) override;

    void Explode();
    bool bHasExploded;
};
