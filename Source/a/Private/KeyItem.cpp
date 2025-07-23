#include "KeyItem.h"

AKeyItem::AKeyItem()
{
}
void AKeyItem::OnItemOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Key collected!"));
}
void AKeyItem::ActivateItem(AActor* Activator)
{
	UE_LOG(LogTemp, Warning, TEXT("Key used to unlock the door!"));
}