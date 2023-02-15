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


		
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);
	GetMesh()->OnComponentHit.AddDynamic(this, &APlayerCharacter::OnHit);
	
	
	GetCapsuleComponent()->BodyInstance.bNotifyRigidBodyCollision = true;
	GetMesh()->BodyInstance.bNotifyRigidBodyCollision = true;

	//스탯컴포넌트
	mStatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("CharacterStatComp"));

	//HpBar 위젯
	
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
	
		
	//verify(mStatComponent != nullptr);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if(mProjectileTable == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Character BeginPlay ProjectileTable nullptr"));
		static UWPGameInstance* gameInstance = Cast<UWPGameInstance>(GetGameInstance());
		if (gameInstance)
			mProjectileTable = gameInstance->DataManager->GetTable(FName("Projectile"));
	}
	
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FVector pos;
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

	PlayerInputComponent->BindAction(TEXT("Fire"), EInputEvent::IE_Pressed, this, &APlayerCharacter::OnFire);
	PlayerInputComponent->BindAction(TEXT("RightClick"), EInputEvent::IE_Pressed, this, &APlayerCharacter::OnFireRight);

	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &APlayerCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &APlayerCharacter::LeftRight);

	PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &APlayerCharacter::Yaw);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &APawn::AddControllerPitchInput);

}

void APlayerCharacter::UpDown(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
	mVertical = Value;
}

void APlayerCharacter::LeftRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
	mHorizontal = Value;
}

void APlayerCharacter::Yaw(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::OnFire()
{
	// try and fire a projectile

	//Projectile의 UCLASS를 가지고 있는 데이터테이블을 가져옴.
	/*static UWPGameInstance* gameInstance = Cast<UWPGameInstance>(GetGameInstance());
	check(gameInstance);*/
	//UDataTable* projectileTable = gameInstance->DataManager->GetTable(FName("Projectile"));
	check(mProjectileTable);

	const FProjectileData* currProjectileData = mProjectileTable->FindRow<FProjectileData>(FName(TEXT("GrenadeTest")), TEXT("OnFireProjectileTable"));
	mProjectile = currProjectileData->ProjectileClass;

	
	if (mProjectile != nullptr)
	{
		UWorld* const World = GetWorld();
			if (World != nullptr)
			{
				const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			//임시 하드코딩 offset 벡터
			FVector GunOffset(100.0f, 0.0f, 10.0f);
			FVector playerPos = GetActorLocation();
				
			const FVector SpawnLocation = ((mGunPos != nullptr) ? mGunPos->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

		/*	UE_LOG(LogTemp, Warning, TEXT("Player Pos : %f %f %f  SpawnPos = %f %f %f"), 
				playerPos.X, playerPos.Y, playerPos.Z,
				SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);*/
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			UE_LOG(LogTemp, Error, TEXT("Spawn AProjectile"));
			// spawn the projectile at the muzzle
			//World->SpawnActor<AProjectile>(mProjectile, SpawnLocation, SpawnRotation, ActorSpawnParams);

			//ProjectileMovement 컴포넌트가 초기화 되기 전에 해당 발사체의 정보를 설정하고 스폰
			const FTransform SpawnTr(SpawnRotation, SpawnLocation);
			AProjectile* SpawnedProjectile = World->SpawnActorDeferred<AProjectile>(mProjectile, SpawnTr, this, this);
			SpawnedProjectile->SetProjectileInfo(currProjectileData, 1600.0f);
			UGameplayStatics::FinishSpawningActor(SpawnedProjectile, SpawnTr);
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

}

float APlayerCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	
	UE_LOG(LogTemp, Warning, TEXT("TakeDamage Function Called"));
	//주로 ApplyRadialDamage에서 호출 됨
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
	//GetWorld()->SpawnActor(mProjectile, )
	// try and fire a projectile
	
	if (mProjectileWarms2 != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			//임시 하드코딩 offset 벡터
			FVector GunOffset(100.0f, 0.0f, 10.0f);
			FVector playerPos = GetActorLocation();

			const FVector SpawnLocation = ((mGunPos != nullptr) ? mGunPos->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

	/*		UE_LOG(LogTemp, Warning, TEXT("Player Pos : %f %f %f  SpawnPos = %f %f %f"),
				playerPos.X, playerPos.Y, playerPos.Z,
				SpawnLocation.X, SpawnLocation.Y, SpawnLocation.Z);*/
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AWarmsPortfolioProjectile>(mProjectileWarms2, SpawnLocation, SpawnRotation, ActorSpawnParams);
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
}

void APlayerCharacter::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter OnHit, OtherActotr : %s"), *OtherActor->GetName());
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

//void APlayerCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
//	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
//{
//	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
//	UE_LOG(LogTemp, Warning, TEXT("*****PlayerCharacter NotifyHit, OtherActor : %s"), *Other->GetName());
//}
