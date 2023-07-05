// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Effects/ProjectileExplosionEffect.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "DataTableStructures.h"
#include "SoundManager.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "WarmsGameModeBase.h"
#include "Kismet/KismetMathLibrary.h"

FProjectileInfo& FProjectileInfo::operator=(const FProjectileData& tableData)
{
	ProjectileRadius = tableData.Radius;
	MaxSpeed = tableData.MaxSpeed;
	Weight = tableData.Weight;
	AttackPower = tableData.AttackPower;
	ExplodeRange = tableData.ExplodeRange;

	return *this;
}

AProjectile::AProjectile()
{
	mCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	mCollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	

	//�÷��̾ �߻�ü ���� ���� �� ����
	mCollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	mCollisionComp->CanCharacterStepUpOn = ECB_No;
	//hit event �߻� ����
	mCollisionComp->BodyInstance.bNotifyRigidBodyCollision = true;
	

	RootComponent = mCollisionComp;

	mProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	mProjectileMovement->UpdatedComponent = mCollisionComp;
	mProjectileMovement->bRotationFollowsVelocity = true;
	mProjectileMovement->bShouldBounce = true;

	//mMeshComponent->SetupAttachment(RootComponent);

	mCollisionComp->InitSphereRadius(mProjectileInfo.Radius);
	mProjectileMovement->InitialSpeed = mProjectileInfo.InitialSpeed;
	mProjectileMovement->MaxSpeed = mProjectileInfo.MaxSpeed;

	mGunPos = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosSceneComponent"));
	mGunPos->SetRelativeLocation(FVector::ZeroVector);

	//�߻�ü ���� �׸��⿡ ���Ǵ� ����
	static ConstructorHelpers::FClassFinder<AActor> Trajectory(TEXT("Blueprint'/Game/BluePrints/TrajectoryActor.TrajectoryActor_C'"));
	if(Trajectory.Succeeded())
	{
		mTrajectoryActor = Trajectory.Class;
	}

	static ConstructorHelpers::FClassFinder<AActor> FireRange(TEXT("Blueprint'/Game/BluePrints/Effects/FireRangeActor.FireRangeActor_C'"));
	

	if (FireRange.Succeeded())
	{
		mFireRangeActor = FireRange.Class;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Constructor InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
	UE_LOG(LogTemp, Warning, TEXT("Constructor FireCoefficient : %f"), FireCoefficient);

	mSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	mCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));

	mSpringArm->SetupAttachment(GetRootComponent());
	mSpringArm->SetRelativeLocation(FVector::ZeroVector);
	mCamera->SetupAttachment(mSpringArm);
	//InitialLifeSpan = 10.0f;
	
	
	//SetActorEnableCollision(true);
	//mCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
} 

void AProjectile::PostInitializeComponents()
{
	UE_LOG(LogTemp, Warning, TEXT("Pre PostInitialize FireCoefficient : %f"), FireCoefficient);
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("Post PostInitialize FireCoefficient : %f"), FireCoefficient);
}

void AProjectile::PreInitializeComponents()
{
	UE_LOG(LogTemp, Warning, TEXT("Pre PreInitialize Firecoefficient : %f"), FireCoefficient);
	
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Warning, TEXT("Post PreInitialize FireCoefficient : %f"), FireCoefficient);
	mCollisionComp->InitSphereRadius(mProjectileInfo.Radius);
	mProjectileMovement->InitialSpeed = GetInitialSpeed();
	mProjectileMovement->MaxSpeed = mProjectileInfo.MaxSpeed;
}


void AProjectile::SetProjectileInfo(const FProjectileData* ProjectileData)
{
	mProjectileInfo = *ProjectileData;

	if (ProjectileData)
	{
		mProjectileInfo = *ProjectileData;
		UE_LOG(LogTemp, Warning, TEXT("SetProjectileInfo Success, InitialSpeed : %f"), ProjectileData->InitialSpeed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetProjectileInfo Failed"));
	}
	//mProjectileInfo.InitialSpeed = InitialSpeed;
}

UShapeComponent* AProjectile::GetCollider() const
{
	return mCollisionComp;
}

inline void AProjectile::Explode(const FHitResult& Impact)
{
	// Ÿ�������� �븻���� ���⿡�� ����
	const FVector NudgedImpactLocation = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;

	if (mExplosionFX)
	{
		FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), NudgedImpactLocation);
		//FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);

		//UE_LOG(LogTemp, Warning, TEXT("ImpactPoint X:%f Y:%f Z:%f"), Impact.ImpactPoint.X, Impact.ImpactPoint.Y, Impact.ImpactPoint.Z);

		const FVector spawnPos = SpawnTransform.GetLocation();

		
		AProjectileExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AProjectileExplosionEffect>(mExplosionFX, SpawnTransform);
		//AProjectileExplosionEffect* const EffectActor = GetWorld()->SpawnActorDeferred<AProjectileExplosionEffect>(AProjectileExplosionEffect::StaticClass(), SpawnTransform);
		//auto EffectActor = NewObject<AProjectileExplosionEffect>(GetWorld(), mExplosionFX);
		UE_LOG(LogTemp, Warning, TEXT("SpawnActorDeffered End"));
		if (EffectActor)
		{
			UE_LOG(LogTemp, Warning, TEXT("Pre FinishSpawningActor"));
			/*	UE_LOG(LogTemp, Warning, TEXT("SpawnTransform X:%f Y:%f Z:%f"), spawnPos.X,
					spawnPos.Y,
					spawnPos.Z);*/


					//EffectActor->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
			UE_LOG(LogTemp, Warning, TEXT("FinishSpawningActor End"));
		}
	}

	//전역카메라로 폭발지점을 보여줍니다.
	float CameraDistance = 800.f;

	FVector CameraLoc = Impact.ImpactPoint + Impact.ImpactNormal*CameraDistance;
	FRotator CameraRot = UKismetMathLibrary::FindLookAtRotation(CameraLoc, Impact.ImpactPoint);

	((AWarmsGameModeBase*)GetWorld()->GetAuthGameMode())->UseWorldCamera(CameraLoc,
		CameraRot, nullptr, 0.75f);

	ApplyDamage(Impact);

	//폭발범위만큼 지형파괴
	RemoveVoxelSphere(Impact.ImpactPoint, mProjectileInfo.ExplodeRange);

	
	UE_LOG(LogTemp, Warning, TEXT("Projectile Explode"));
}

void AProjectile::ApplyDamage(const FHitResult& Impact)
{
	TArray<AActor*> Ignores;


	DrawDebugSphere(GetWorld(), Impact.ImpactPoint, mProjectileInfo.ExplodeRange, 16,
		FColor::Red, false, 2.0f);

	
	UGameplayStatics::ApplyRadialDamage(GetWorld(), mProjectileInfo.AttackPower, Impact.ImpactPoint,
		mProjectileInfo.ExplodeRange, nullptr, Ignores, this);

	
}

void AProjectile::PlayFireSound()
{
	auto GM = Cast<AWarmsGameModeBase>(GetWorld()->GetAuthGameMode());
	auto Sound = GM->SoundManager->GetSound(TEXT("PowerfulShot"));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(), 0.3f);
}

float AProjectile::GetInitialSpeed()
{
	return FMath::Min(mProjectileInfo.InitialSpeed * FireRate, mProjectileInfo.MaxSpeed);
}

void AProjectile::SetObjectType()
{
	GameObjectType = EObjectTypeName::Projectile;
}

void AProjectile::Fire()
{
	//���� ���Ѱ�
	//�������Ʈ���� Socket ���� ���� ����.
	//�������Ʈ���� mGunPos�� Socket�� �ٿ���ߵ�.
	UE_LOG(LogTemp, Warning, TEXT("--------AProjectile Fire-----------"));
	UE_LOG(LogTemp, Warning, TEXT("Projectile InitialSpeed : %f, FireRate : %f"), GetInitialSpeed(), FireRate);
	UE_LOG(LogTemp, Warning, TEXT("Projectile FireCoefficient : %f"), FireCoefficient);
	GetCollider()->IgnoreActorWhenMoving(GetInstigator(), true);

	auto Controller = GetInstigatorController();
	FVector FirePos;
	FRotator FireRot;
	Controller->GetPlayerViewPoint(FirePos, FireRot);
	FirePos = mGunPos->GetComponentLocation();
	
	
	const FTransform SpawnTr(FireRot, FirePos);
	DrawDebugSphere(GetWorld(), SpawnTr.GetLocation(), 30.f, 64, FColor::Black, false, 3.f);
	////�߻� ��ġ�� ������ ������ġ�� ã��
	//if (mMeshComponent->DoesSocketExist(FireSocket))
	//{
	//	
	//	//FirePos = mMeshComponent->GetSocketLocation(FireSocket);
	//}
	//else
	//{
	//	FirePos = GetActorLocation();
	//}
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	//DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
	SetActorEnableCollision(true);
	
	//FTransform const* const OriginalSpawnTransform = GetDeferredTransformCache(this);
	//UE_LOG(LogTemp, Warning, TEXT("GetDefferedTransform %s"), *OriginalSpawnTransform->ToString());

	GetRootComponent()->SetWorldTransform(SpawnTr);
	//AActor* thisActor = UGameplayStatics::FinishSpawningActor(this, SpawnTr);
	FinishSpawning(SpawnTr, true);


	PlayFireSound();
	//SetActorTransform(SpawnTr);
	//FinishSpawningTransform(SpawnTr);
	UE_LOG(LogTemp, Warning, TEXT("AProjectile Fire End"));
}

void AProjectile::SetWeaponData(const FWeaponData* WeaponData)
{
	Super::SetWeaponData(WeaponData);

	const FProjectileData* ProjectileData = static_cast<const FProjectileData*>(WeaponData);

	
	if(ProjectileData)
	{
		mProjectileInfo = *ProjectileData;
		UE_LOG(LogTemp, Warning, TEXT("SetWeaponData Success, InitialSpeed : %f"), ProjectileData->InitialSpeed);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetWeaponData Failed"));
	}
}

void AProjectile::DrawTrajectory()
{
	//���� ���Ѱ�
	//�������Ʈ���� Socket ���� ���� ����.
	//�������Ʈ���� mGunPos�� Socket�� �ٿ���ߵ�.
	auto Controller = GetInstigatorController();
	FVector FirePos;
	FRotator FireRot;
	Controller->GetPlayerViewPoint(FirePos, FireRot);
	FirePos = mGunPos->GetComponentLocation();

	FPredictProjectilePathResult Result;
	FPredictProjectilePathParams Params;
	//UE_LOG(LogTemp, Warning, TEXT("DrawTrajectory FirePos : %s"), *FirePos.ToString());

	

	Params.StartLocation = FirePos;
	Params.DrawDebugTime = 1.f;
	Params.LaunchVelocity = FireRot.Vector().GetSafeNormal() * GetInitialSpeed();
	Params.MaxSimTime = 3.f;
	//Params.DrawDebugType = EDrawDebugTrace::ForDuration;
	Params.ActorsToIgnore.Add(GetInstigator());
	Params.ActorsToIgnore.Add(this);
	Params.bTraceWithCollision = true;
	

	UGameplayStatics::PredictProjectilePath(GetWorld(), Params, Result);

	FVector ImpactPoint = Result.HitResult.ImpactPoint;
	//DrawDebugSphere(GetWorld(), ImpactPoint, 64.f, 32, FColor::Blue, false, 1.f);
	//UE_LOG(LogTemp, Warning, TEXT("ImpactPoint %s"), *Result.HitResult.ImpactPoint.ToString());

	//��� ���ͷ� ������ �׸������� ����
	static float TrajectoryNum = 15;
	auto& posArr = Result.PathData;
	float stride = posArr.Num() / TrajectoryNum;

	UWorld* World = GetWorld();
	if(posArr.Num()>0)
	{
		for (float i = 1; i <= posArr.Num() - 1; i += stride)
		{
			int idx = FMath::RoundToInt(i);
			auto pos = posArr[idx];
			World->SpawnActor(mTrajectoryActor, &pos.Location);
		}
	}
	


	//Ÿ�������� ���߹����� ��Ÿ���� ���� ����
	if(Result.HitResult.bBlockingHit)
	{
		AActor* FireRangeActor = World->SpawnActor(mFireRangeActor, &ImpactPoint);
		FVector Origin, Extents;
		FireRangeActor->GetActorBounds(false, Origin, Extents, true);
		float Scale = mProjectileInfo.ExplodeRange / Extents.X;

		//UE_LOG(LogTemp, Warning, TEXT("BoundingBox Size : %s"), *Extents.ToString());
		FireRangeActor->SetActorScale3D(FVector(Scale, Scale, Scale));
	}
		
}

void AProjectile::Clicking(float DeltaTime)
{
	//Ŭ���ϴ� �ð���ŭ InitialSpeed�� �� ����
	FireRate += FireCoefficient * DeltaTime;
}

void AProjectile::PostActorCreated()
{
	UE_LOG(LogTemp, Warning, TEXT("Pre PostActorCreated Firecoefficient : %f"), FireCoefficient);
	Super::PostActorCreated();
	UE_LOG(LogTemp, Warning, TEXT("Post PostActorCreated FireCoefficient : %f"), FireCoefficient);
}

void AProjectile::InitMesh()
{
	Super::InitMesh();
	if (mMeshComponent == nullptr)
		return;

	if(mMeshComponent->DoesSocketExist(FireSocket))
	{
		UE_LOG(LogTemp, Warning, TEXT("InitMesh Socket Exists"));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("InitMesh Socket Not Exists"));
	}
	//mGunPos->SetupAttachment(mMeshComponent, FireSocket);
	mGunPos->AttachToComponent(mMeshComponent, FAttachmentTransformRules::KeepRelativeTransform
		, FireSocket);



}


void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(10.0f);
	mSpringArm->SetRelativeLocation(FVector::ZeroVector);
	DrawDebugSphere(GetWorld(), GetActorLocation(), 30.f, 64, FColor::White, false, 3.f);
	UE_LOG(LogTemp, Warning, TEXT("AProjectile BeginPlay"));
}