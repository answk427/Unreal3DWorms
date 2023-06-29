// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"

#include <Kismet/GameplayStatics.h>
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/WidgetComponent.h>

#include <Components/SceneComponent.h>
#include "Weapons/Projectile.h"
#include "WarmsPortfolioProjectile.h"

#include "StatComponent.h"
#include "UI/HpBarWidget.h"
#include "WPGameInstance.h"

#include "Engine/DataTable.h"
#include "DataTableStructures.h"

#include "FInventory.h"
#include "PlayerEquipments.h"
#include "UI/InventoryWeaponWidget.h"
#include "CaptureHelper.h"
#include "Components/TextBlock.h"
#include "Weapons/ItemActor.h"
#include "Weapons/MyRope.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/PostProcessVolume.h"
#include "Kismet/KismetMathLibrary.h"

#include "WarmsGameModeBase.h"

#include "DrawDebugHelpers.h"
#include "PlayerAnimation.h"


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

	mSpringArmSky = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARMSKY"));
	mCameraSky = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERASKY"));

	mSpringArmSky->SetupAttachment(RootComponent);
	mCameraSky->SetupAttachment(mSpringArmSky);


	GetMesh()->SetRelativeLocationAndRotation(
		FVector(0.f, 0.f, -88.f), FRotator(0.f, -90.0f, 0.f));

	/*static ConstructorHelpers::FObjectFinder<USkeletalMesh> SkeletalMesh(TEXT("SkeletalMesh'/Game/MyCharacter/Mesh/character.character'"));

	if (SkeletalMesh.Succeeded())
		GetMesh()->SetSkeletalMesh(SkeletalMesh.Object);*/

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

	

	AWarmsGameModeBase* MyGameMode = Cast<AWarmsGameModeBase>(GetWorld()->GetAuthGameMode());
	StartDamagedDelegate.AddUObject(MyGameMode, &AWarmsGameModeBase::AddDamagedPlayer);
	EndDamagedDelegate.AddUObject(MyGameMode, &AWarmsGameModeBase::RemoveDamagedPlayer);
	OnDieDelegate.AddUObject(MyGameMode, &AWarmsGameModeBase::DieCharacter);

}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("bFireHoldDown : %s"), bFireHoldDown? TEXT("true") : TEXT("false"));

	if(bFireHoldDown)
		Aiming();

	if (bTakingDamage)
	{
		TakingDamageTime += DeltaTime;
		
		//데미지를 받은 이후로 캐릭터의 속도가 0이되면 턴을 종료할 준비를 함
		if (TakingDamageTime > 1.f && bTakingDamage)
		{
			if (GetVelocity().IsNearlyZero(5.f))
			{
				UE_LOG(LogTemp, Warning, TEXT("GetVelocity : %f"), GetVelocity().Size());
				GetCapsuleComponent()->SetSimulatePhysics(false);
				SetActorRotation(FRotator::ZeroRotator);
				UE_LOG(LogTemp, Error, TEXT("Tick bTakingDamage false"));
				bTakingDamage = false;
				TakingDamageTime = 0.f;
				EndDamagedDelegate.Broadcast(this);
			}
		}
	}

	if(IsInWaterFull())
	{
		DieInWater();
		//UE_LOG(LogTemp, Warning, TEXT("diffZ = %f, CharacterHeight : %f"), diffZ, CharacterHeight);
	}

	

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
	
	UE_LOG(LogTemp, Warning, TEXT("SetupPlayerInputComponent Actor Name : %s , PlayerInputComponentName : %s"), *GetName(), *PlayerInputComponent->GetName());

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
	PlayerInputComponent->BindAction(TEXT("ToggleCursor"), EInputEvent::IE_Released, this, &APlayerCharacter::ToggleShowCursor);

	PlayerInputComponent->BindAxis(TEXT("Pull"), this, &APlayerCharacter::Pull);
	PlayerInputComponent->BindAxis(TEXT("Push"), this, &APlayerCharacter::Push);

}

void APlayerCharacter::ToggleShowCursor()
{
	auto MyController = GetWorld()->GetFirstPlayerController();

	//Toggle이므로 현재 bool값에서 뒤집음.
	bool ShowCursor = !MyController->bShowMouseCursor;

	/*if (ShowCursor)
		MyController->SetInputMode(FInputModeUIOnly());
	else
		MyController->SetInputMode(FInputModeGameAndUI());*/


	MyController->SetShowMouseCursor(ShowCursor);

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

	PlayerAnim->PlayAimingMotion();
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
	{
		return;
	}
		

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
	float X, Y;
	X = FMath::FRandRange(-6000.f, 1500.f);
	Y = FMath::FRandRange(3000.f, 9000.f);

	FVector SpawnLocation;
	
	UE_LOG(LogTemp, Warning, TEXT("ReleasedFire"));
	bFireHoldDown = false;

	if (mCurrentWeapon == nullptr || mCurrentWeapon->IsPendingKill())
		return;

	mCurrentWeapon->Fire();

	((AWarmsGameModeBase*)GetWorld()->GetAuthGameMode())->SwitchCamera(mCurrentWeapon);

}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                   AActor* DamageCauser)
{
	StartDamagedDelegate.Broadcast(this);
	GetCapsuleComponent()->SetSimulatePhysics(true);

	UE_LOG(LogTemp, Warning, TEXT("TakeDamage Function Called"));
	//�ַ� ApplyRadialDamage���� ȣ�� ��
	if (DamageEvent.IsOfType(FRadialDamageEvent::ClassID))
	{
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage Function FRadialDamageEvent"));
		mStatComponent->SetHp((float)mStatComponent->GetHp() - Damage);
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage CurrentHp : %f"), mStatComponent->GetHp());

		GetCapsuleComponent()->AddRadialImpulse(DamageCauser->GetActorLocation(), 500.f, Damage * ImpulseRate, ERadialImpulseFalloff::RIF_Linear, true);
		
		DrawDebugSphere(GetWorld(), DamageCauser->GetActorLocation(), 500.f, 16,
			FColor::Blue, false, 2.0f);
		
		UE_LOG(LogTemp, Warning, TEXT("TakeDamage DamageCauser %s"), *DamageCauser->GetActorLocation().ToString());
		
		//GetMovementComponent()->AddRadialImpulse(GetActorLocation(), 100.f, 100.f, ERadialImpulseFalloff::RIF_Linear, true);
		
	}

	//오브젝트에 힘을 가한 뒤에 true로 변경해야 함
	bTakingDamage = true;
	

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

void APlayerCharacter::ActiveOneCamera(int i)
{
	switch(i)
	{
	case 0 :
		mCamera->SetActive(true);
		mCameraSky->SetActive(false);
		break;
	case 1:
		mCamera->SetActive(false);
		mCameraSky->SetActive(true);
		break;
	}

}

void APlayerCharacter::DieChecking()
{
	bIsDead = true;
	AWarmsGameModeBase* MyGameMode = Cast<AWarmsGameModeBase>(GetWorld()->GetAuthGameMode());
	MyGameMode->AddDeadPlayer(this);
}

void APlayerCharacter::SetColor(FLinearColor& Color)
{
	TeamColor = Color;

	FVector ColorVec(Color.R, Color.G, Color.B);
	GetMesh()->SetVectorParameterValueOnMaterials(FName(TEXT("CharacterColor")),ColorVec);
	

	UHpBarWidget* hpBarInstance = Cast<UHpBarWidget>(mHpBarWidget->GetUserWidgetObject());
	hpBarInstance->SetHpBarColor(Color);
	/*	UMaterialInstanceDynamic* Material =
		UMaterialInstanceDynamic::Create(GraveMaterial, this);
	Material->SetVectorParameterValue(ColorParamName, Color);
	GraveMesh->SetMaterial(0, Material);*/
}

void APlayerCharacter::SetTeam(int Number)
{
	TeamNumber = Number;
}

void APlayerCharacter::Init(FLinearColor& Color, int TeamNum)
{
	SetColor(Color);
	SetTeam(TeamNum);
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
	UE_LOG(LogTemp, Error, TEXT("*************%s PostInitializeComponents*********************"), *GetName());
	mHpBarWidget->InitWidget();

	verify(mStatComponent != nullptr);

	//체력이 0이 됐을 때 사망플래그 표시
	mStatComponent->HpOnZero.AddUObject(this, &APlayerCharacter::DieChecking);
	//mStatComponent->HpOnZero.AddLambda([this]() {bIsDead = true; });

	UHpBarWidget* hpBarInstance = Cast<UHpBarWidget>(mHpBarWidget->GetUserWidgetObject());
	if (hpBarInstance)
	{
		hpBarInstance->BindingStatComp(mStatComponent);
	}

	PlayerAnim = Cast<UPlayerAnimation>(GetMesh()->GetAnimInstance());
}

void APlayerCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("*************%s PreInitializeComponents*********************"), *GetName());
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

void APlayerCharacter::PostLoad()
{
	Super::PostLoad();
	UE_LOG(LogTemp, Error, TEXT("*************%s PostLoad*********************"), *GetName());
}

void APlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (bTakingDamage)
		EndDamagedDelegate.Broadcast(this);

	UE_LOG(LogTemp, Warning, TEXT("%s EndPlay"), *GetName());
}

//void APlayerCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
//	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
//{
//	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
//	UE_LOG(LogTemp, Warning, TEXT("*****PlayerCharacter NotifyHit, OtherActor : %s"), *Other->GetName());
//}


inline void APlayerCharacter::EnterWater()
{
	bInWater = true;
	WaterZ = GetActorLocation().Z;

}

inline void APlayerCharacter::ExitWater()
{
	bInWater = false;
}

void APlayerCharacter::DieInWater()
{
	if (bIsDead)
		return;

	//물에 완전 빠지면 사망처리
	if(!bTakingDamage)
	{
		bTakingDamage = true;
		StartDamagedDelegate.Broadcast(this);
	}
	UE_LOG(LogTemp, Warning, TEXT("DieInWater"));

	mStatComponent->SetHp(0);
				   		
}

bool APlayerCharacter::IsInWaterFull()
{
	if (!bInWater)
		return false;

	float diffZ = WaterZ - GetActorLocation().Z;
	float CharacterHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

	//입수할 때 물의 Z값과 현재 Z값의 차이가 캐릭터의 높이/2 보다 크면 완전잠수상태
	if (diffZ > CharacterHeight * 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("InWaterFull"));
		return true;
	}

	return false;
}

