// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include <Kismet/GameplayStatics.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/WidgetComponent.h>

#include <Components/SceneComponent.h>
#include "../Weapons/Projectile.h"
#include "../WarmsPortfolioProjectile.h"

#include "StatComponent.h"
#include "../UI/HpBarWidget.h"
#include "../WPGameInstance.h"

#include "Engine/DataTable.h"
#include "../DataTableStructures.h"

#include "FInventory.h"
#include "PlayerEquipments.h"
#include "../UI/InventoryWeaponWidget.h"
#include "../CaptureHelper.h"
#include "Components/TextBlock.h"
#include "../Weapons/ItemActor.h"
#include "../Weapons/MyRope.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/KismetMathLibrary.h"

UDataTable* APlayerCharacter::mProjectileTable = nullptr;

// Sets default values
APlayerCharacter::APlayerCharacter()
{

	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = GetCapsuleComponent();

	mSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	mSpringArm->SetupAttachment(RootComponent);
	mCamera->SetupAttachment(mSpringArm);


	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.0f, 0.f));

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(TEXT("SkeletalMesh'/Game/MyCharacter/Mesh/character.character'"));

	if (SkeletalMesh.Succeeded())
		GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);

	AWarmsPortfolioProjectile* projectile = CreateDefaultSubobject<AWarmsPortfolioProjectile>(TEXT("projectile"));
	UClass* uprojectile = projectile->GetClass();
	mProjectileWarms2 = uprojectile;



	//GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::OnSphereComponentBeginOverlap);
	//GetMesh()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);


	GetCapsuleComponent()->BodyInstance.bNotifyRigidBodyCollision = true;
	GetMesh()->BodyInstance.bNotifyRigidBodyCollision = true;

	//����������Ʈ
	mStatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("CharacterStatComp"));

	//HpBar ����

	mHpBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HpWidget"));
	mHpBarWidget->SetupAttachment(GetMesh());
	mHpBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	mHpBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UHpBarWidget> UW(
		TEXT("WidgetBlueprint'/Game/UI/HP.HP_C'"));


	if (UW.Succeeded())
	{
		mHpBarWidget->SetWidgetClass(UW.Class);
		mHpBarWidget->SetDrawSize(FVector2D(200.0f, 100.0f));
	}

	//�κ��丮
	mInventory = MakeShared<FInventory>();
	//���â
	mEquipments = MakeShared<FPlayerEquipments>();

	JumpMaxHoldTime = 3.0f;
	JumpMaxCount = 2;

	FName WeaponSocket(TEXT("RightHandSocket"));
	mWeaponPos = CreateDefaultSubobject<USceneComponent>(TEXT("GunPos"));
	mWeaponPos->SetupAttachment(GetMesh(), WeaponSocket);


}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitInventoryWidget();
	if (mProjectileTable == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Character BeginPlay ProjectileTable nullptr"));
		static UWPGameInstance* gameInstance = Cast<UWPGameInstance>(GetGameInstance());
		if (gameInstance)
			mProjectileTable = gameInstance->DataManager->GetTable(FName("Projectile"));
	}

	mRope = GetWorld()->SpawnActor<AMyRope>();
	//mRope->AttachToComponent(mWeaponPos, FAttachmentTransformRules::KeepRelativeTransform);
	mRope->Init(this);

	TArray<AActor*> Result;

	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), APostProcessVolume::StaticClass(),
		FName(TEXT("ActionLine")), Result);


	UE_LOG(LogTemp, Warning, TEXT("Result Num : %d"), Result.Num());
	if (Result.Num() != 0)
	{
		for (auto elem : Result)
		{
			APostProcessVolume* PostProcessVolume = Cast<APostProcessVolume>(elem);
			if (elem != nullptr)
			{
				ActionLine = PostProcessVolume;
				ActionLine->bEnabled = false;
				UE_LOG(LogTemp, Warning, TEXT("ProcessVolume Search Success"));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ProcessVolume Cast Failed"));
			}
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ProcessVolume Search Fail"));
	}

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("bFireHoldDown : %s"), bFireHoldDown? TEXT("true") : TEXT("false"));

	if(bFireHoldDown)
		Aiming();

	//GetCharacterMovement()->AddInputVector(FVector(1.0f, 0.f, 0.f));

	//if(mHpBarWidgetComp)
	//{
	//	pos = mHpBarWidgetComp->GetComponentLocation();
	//	UE_LOG(LogTemp, Warning, TEXT("HpBar is null in CharacterTick"));
	//}
	//else
	//	UE_LOG(LogTemp, Warning, TEXT("HpBar Pos %f %f %f"), pos.X, pos.Y, pos.Z);
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &APlayerCharacter::ClickedFire);
	//PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Repeat, this, &APlayerCharacter::Aiming);
	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Released, this, &APlayerCharacter::ReleasedFire);

	PlayerInputComponent->BindAction(TEXT("RightClick"), EInputEvent::IE_Pressed, this, &APlayerCharacter::OnFireRight);
	PlayerInputComponent->BindAction(TEXT("RightClick"), EInputEvent::IE_Repeat, this, &APlayerCharacter::Aiming);
	
	
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &APlayerCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &APlayerCharacter::LeftRight);

	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &APlayerCharacter::Yaw);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction(TEXT("OpenInventory"), EInputEvent::IE_Pressed, this, &APlayerCharacter::OpenInventory);
	PlayerInputComponent->BindAction(TEXT("AcquireItem"), EInputEvent::IE_Pressed, this, &APlayerCharacter::AcquireItem);

	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Released, this, &APlayerCharacter::StopJumping);
	PlayerInputComponent->BindAction(TEXT("ActionPull"), EInputEvent::IE_Released, this, &APlayerCharacter::PullEnd);

	PlayerInputComponent->BindAxis(TEXT("Pull"), this, &APlayerCharacter::Pull);
	PlayerInputComponent->BindAxis(TEXT("Push"), this, &APlayerCharacter::Push);

	
}

void APlayerCharacter::UpDown(float Value)
{
	if (Value == 0)
		return;
	if (Hanging)
	{
		mRope->AddForceCharacter(0.f, Value);
	}
	else
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
	AddMovementInput(GetActorForwardVector(), Value);
	mVertical = Value;
}

void APlayerCharacter::LeftRight(float Value)
{
	if (Value == 0)
		return;

	if (Hanging)
	{
		mRope->AddForceCharacter(Value, 0.f);
	}
	else
	{
		AddMovementInput(GetActorRightVector(), Value);
	}
	AddMovementInput(GetActorRightVector(), Value);


	mHorizontal = Value;
}

void APlayerCharacter::Yaw(float Value)
{
	/*if(!Hanging)
	{
		AddControllerYawInput(Value);
	}*/

	if (Hanging)
	{
		//ĳ���Ͱ� ������ �ٶ󺸵��� ��
		FVector X = mRope->mCableStarts.Last() - GetActorLocation();
		FVector Z = FVector::CrossProduct(X, GetActorRightVector());
		FVector Y = FVector::CrossProduct(Z, X);

		FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), mRope->GetActorLocation());
		//FRotator MyRot = UKismetMathLibrary::MakeRotationFromAxes(X, Y, Z);

		/*UE_LOG(LogTemp, Warning, TEXT("FindLookAtRotation Rot : %s"), *Rot.ToString());
		UE_LOG(LogTemp, Warning, TEXT("My Cross Rot : %s"), *MyRot.ToString());*/
		SetActorRotation(Rot);
		//mCamera->SetRelativeRotation(FRotator::ZeroRotator);
	}
	else
	{
		AddControllerYawInput(Value);
	}

}

void APlayerCharacter::Pull(float Value)
{
	if (Value == 0)
		return;

	if (Hanging)
	{
		ActionLine->bEnabled = true;
		mRope->PullRope();
	}

}

void APlayerCharacter::Push(float Value)
{
	if (Value == 0)
		return;

	mRope->PushRope();

}

void APlayerCharacter::Aiming()
{
	if(mCurrentWeapon == nullptr)
		return;
	
	//mCurrentWeapon->SetActorLocation(mWeaponPos->GetComponentLocation(), true);
	mCurrentWeapon->Clicking(GetWorld()->GetDeltaSeconds());
	mCurrentWeapon->DrawTrajectory();
}

void APlayerCharacter::ClickedFire()
{
	UE_LOG(LogTemp, Warning, TEXT("ClickedFire"));
	check(mProjectileTable);

	if (Hanging)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter OnFire in Hanging"));

		FVector CameraPosition = mCamera->GetComponentLocation();
		FRotator CameraRotator = mCamera->GetComponentRotation();

		mRope->Fire(mWeaponPos->GetComponentLocation(), CameraPosition, CameraRotator);

		return;
	}

	//���� �������� ���⸦ ������.
	const auto WeaponData = mEquipments->GetWeapon();

	//�������� ���Ⱑ ���� �� ����
	if (WeaponData == nullptr)
		return;

	if (mCurrentWeapon != nullptr)
	{
		mCurrentWeapon->Destroy();
		mCurrentWeapon = nullptr;
	}
		

	TSubclassOf<AWeapon> WeaponUClass = WeaponData->second->WeaponClass;

	if (WeaponUClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			//�ӽ� �ϵ��ڵ� offset ����
			FVector GunOffset(100.0f, 0.0f, 10.0f);
			FVector playerPos = GetActorLocation();

			const FVector SpawnLocation = ((mWeaponPos != nullptr) ? mWeaponPos->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
			

			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			UE_LOG(LogTemp, Warning, TEXT("Spawn AProjectile"));
			
			//ProjectileMovement ������Ʈ�� �ʱ�ȭ �Ǳ� ���� �ش� �߻�ü�� ������ �����ϰ� ����
			const FTransform SpawnTr(SpawnRotation, SpawnLocation);
			
			//AWeapon* SpawnedWeapon = World->SpawnActorDeferred<AWeapon>(WeaponUClass, SpawnTr, this, this);
			//mCurrentWeapon = World->SpawnActorDeferred<AWeapon>(WeaponUClass, SpawnTr, this, this);
			mCurrentWeapon = World->SpawnActorDeferred<AWeapon>(WeaponUClass, FTransform(), this, this);

			//SpawnedWeapon->SetWeaponData(WeaponData->second.get());
			mCurrentWeapon->SetWeaponData(WeaponData->second.get());
			mCurrentWeapon->SetActorEnableCollision(false);
			UE_LOG(LogTemp, Warning, TEXT("Weapon Spawn Location : %s"), *mCurrentWeapon->GetActorLocation().ToString());
			mCurrentWeapon->AttachToComponent(mWeaponPos, FAttachmentTransformRules::SnapToTargetIncludingScale);

			//mCurrentWeapon->SetActorHiddenInGame(true);
			//SpawnedWeapon->AttachToComponent(mWeaponPos, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			
			//SpawnedWeapon->Fire();
		}
	}


	// try and play the sound if specified
	//if (FireSound != nullptr)
	//{
	//	UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	//}

	//// try and play a firing animation if specified
	//if (FireAnimation != nullptr)
	//{
	//	// Get the animation object for the arms mesh
	//	UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
	//	if (AnimInstance != nullptr)
	//	{
	//		AnimInstance->Montage_Play(FireAnimation, 1.f);
	//	}
	//}

	bFireHoldDown = true;
}

void APlayerCharacter::ReleasedFire()
{
	UE_LOG(LogTemp, Warning, TEXT("ReleasedFire"));
	bFireHoldDown = false;

	if (mCurrentWeapon == nullptr || mCurrentWeapon->IsPendingKill())
		return;

	mCurrentWeapon->Fire();
}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                   AActor* DamageCauser)
{

	UE_LOG(LogTemp, Warning, TEXT("TakeDamage Function Called"));
	//�ַ� ApplyRadialDamage���� ȣ�� ��
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage Function FRadialDamageEvent"));
		mStatComponent->SetHp((float)mStatComponent->GetHp() - Damage);
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage CurrentHp : %f"), mStatComponent->GetHp());
	}

	return Damage;
}


void APlayerCharacter::OnFireRight()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter OnFire in Hanging"));

	FVector CameraPosition = mCamera->GetComponentLocation();
	FRotator CameraRotator = mCamera->GetComponentRotation();

	//mRope->Fire(mWeaponPos->GetComponentLocation(), CameraPosition, CameraRotator);
	mRope->Fire(mCamera->GetComponentLocation(), CameraPosition, CameraRotator);

}

void APlayerCharacter::InitInventoryWidget()
{
	mInventoryWidget = CreateWidget<UInventoryWeaponWidget>(GetWorld(), mInventoryWidgetClass);
	check(mInventoryWidget);
	mInventoryWidget->AddToViewport();
	mInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);

	UInventoryWeaponWidget* InventoryInstance = Cast<UInventoryWeaponWidget>
		(mInventoryWidget);

	if (InventoryInstance)
	{
		InventoryInstance->BindInventory(mInventory, mEquipments);
		//InventoryInstance->ItemInventory->OnItemClicked().Add()
	}


}

void APlayerCharacter::OpenInventory()
{

	if (mInventoryWidget)
	{
		//���� �׽�Ʈ�� �ڵ�
		//mInventory->AddWeaponItem(FItem(FName(TEXT("GrenadeTest")), EObjectTypeName::Projectile));
		/*mEntry = Cast<UInventoryWeaponEntry>(CreateWidget(GetWorld(), mEntryClass));
		UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(mEntry);
		UCaptureHelper::Instance()->DrawActorToTexture(mProjectile, RenderTarget, 256, 256);
		UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(UCaptureHelper::Instance()->ItemEntryMat, mEntry);
		MaterialInstance->SetTextureParameterValue(TEXT("ItemTexture"), RenderTarget);

		mEntry->InitEntry(RenderTarget, MaterialInstance, FName(TEXT("hihi")));*/

		UInventoryWeaponWidget* Inventory = Cast<UInventoryWeaponWidget>(mInventoryWidget);
		check(Inventory != nullptr);
		mInventory->AddWeaponItem(FWPItem(FName(TEXT("GrenadeTest")), EObjectTypeName::Projectile));
		mInventory->AddWeaponItem(FWPItem(FName(TEXT("MissileTest")), EObjectTypeName::Projectile));
		mInventory->AddWeaponItem(FWPItem(FName(TEXT("RocketLauncher")), EObjectTypeName::Projectile));

		auto Visibility = mInventoryWidget->GetVisibility();

		UE_LOG(LogTemp, Warning, TEXT("OpenInventory mInventoryWidget not Null"));


		//ESlateVisibility

		switch (Visibility)
		{
		case ESlateVisibility::Collapsed:
			mInventoryWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UE_LOG(LogTemp, Warning, TEXT("Visibility is Collapsed"));
			break;

		case ESlateVisibility::SelfHitTestInvisible:
			mInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
			UE_LOG(LogTemp, Warning, TEXT("Visibility is SelfHitTestInvisible"));
			break;

		default:
			UE_LOG(LogTemp, Warning, TEXT("Visibility is Default"));
			break;
		}
	}
	else
		UE_LOG(LogTemp, Warning, TEXT("OpenInventory mInventoryWidget is Null"));

}

void APlayerCharacter::AcquireItem()
{
	TArray<AActor*> OverlapItems;
	GetOverlappingActors(OverlapItems, AItemActor::StaticClass());
	if (OverlapItems.Num() == 0)
		return;

	AItemActor* FirstItem = Cast<AItemActor>(OverlapItems[0]);
	const FWPItem Item = FirstItem->GetItemInfo();
	mInventory->AddWeaponItem(Item);

	FirstItem->Destroy();
}



void APlayerCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter OnHit, OtherActotr : %s"), *OtherActor->GetName());
}

void APlayerCharacter::OnSphereComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter BeginOverlap, OtherActotr : %s"), *OtherActor->GetName());
}

void APlayerCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	mHpBarWidget->InitWidget();

	verify(mStatComponent != nullptr);
	UHpBarWidget* hpBarInstance = Cast<UHpBarWidget>(mHpBarWidget->GetUserWidgetObject());
	if (hpBarInstance)
	{
		hpBarInstance->BindingStatComp(mStatComponent);
	}

}

void APlayerCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	//verify(mStatComponent != nullptr);
}

void APlayerCharacter::Jump()
{
	if (Hanging)
	{
		mRope->Deactivate();
	}
	else
	{
		Super::Jump();
	}

}

//void APlayerCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
//	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
//{
//	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
//	UE_LOG(LogTemp, Warning, TEXT("*****PlayerCharacter NotifyHit, OtherActor : %s"), *Other->GetName());
//}
