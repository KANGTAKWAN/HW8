#include "MyPawn.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MyPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AMyPawn::AMyPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	// ĸ�� ������Ʈ
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);
	CapsuleComp->InitCapsuleSize(42.f, 96.f); // (������, ����)
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComp->SetCollisionObjectType(ECC_Pawn); // ������ ����
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore); // �⺻ ��� ����
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); // ���� Static�� ����
	CapsuleComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	CapsuleComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // �ٸ� Pawn�� ��ħ
	CapsuleComp->SetSimulatePhysics(false);

	// ���̷�Ż �޽� ������Ʈ
	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);
	SkeletalMeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f)); // ĸ�� ���� ����
	SkeletalMeshComp->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f)); // ���� ȸ�� ����

	// �������� ������Ʈ
	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = false;

	// ī�޶� ������Ʈ
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshAsset(TEXT("/Game/Resources/Characters/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	if (MeshAsset.Succeeded())
	{
		SkeletalMeshComp->SetSkeletalMesh(MeshAsset.Object);
	}
	AutoPossessPlayer = EAutoReceiveInput::Player0;

}
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
	
}
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (AMyPlayerController* MyPC = Cast<AMyPlayerController>(GetController()))
		{
			if (MyPC->MoveAction)
			{
				EnhancedInput->BindAction(MyPC->MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
			}

			if (MyPC->LookAction)
			{
				EnhancedInput->BindAction(MyPC->LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
			}
		}
	}
}
void AMyPawn::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (!MovementVector.IsNearlyZero())
	{
		// �̵� �ӵ� (cm/s)
		const float MoveSpeed = 300.f;

		// DeltaTime ���
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		// �̵� ���͸� ���� ��ǥ��� ��ȯ (X: �յ�, Y: �¿�)
		FVector MoveDirection = FVector(MovementVector.X, MovementVector.Y, 0.f);

		// ���� �̵���
		FVector DeltaLocation = MoveDirection * MoveSpeed *DeltaTime;

		// �浹 ����ϸ� ��ġ ����
		AddActorLocalOffset(DeltaLocation, false);

	}
}
void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookVector = Value.Get<FVector2D>();
	if (!LookVector.IsNearlyZero())
	{
		const float RotationSpeed = 90.f;
		float DeltaTime = GetWorld()->GetDeltaSeconds();

		// �¿� ȸ�� (Yaw)
		FRotator YawRotation(0.f, LookVector.X * RotationSpeed * DeltaTime, 0.f);
		AddActorLocalRotation(YawRotation);

		// ���� ȸ�� (Pitch)
		if (SpringArmComp)
		{
			FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
			SpringArmRotation.Pitch = FMath::Clamp(SpringArmRotation.Pitch - LookVector.Y * RotationSpeed * DeltaTime, -80.f, 80.f);
			SpringArmComp->SetRelativeRotation(SpringArmRotation);
		}
	}
}