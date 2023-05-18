// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "../Effects/ProjectileExplosionEffect.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

#include "../DataTableStructures.h"

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
	

	//플레이어가 발사체 위를 걸을 수 없음
	mCollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	mCollisionComp->CanCharacterStepUpOn = ECB_No;
	//hit event 발생 여부
	mCollisionComp->BodyInstance.bNotifyRigidBodyCollision = true;
	

	RootComponent = mCollisionComp;

	mProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComp"));
	mProjectileMovement->UpdatedComponent = mCollisionComp;
	mProjectileMovement->bRotationFollowsVelocity = true;
	mProjectileMovement->bShouldBounce = true;

	mMeshComponent->SetupAttachment(RootComponent);

	mCollisionComp->InitSphereRadius(mProjectileInfo.Radius);
	mProjectileMovement->InitialSpeed = mProjectileInfo.InitialSpeed;
	mProjectileMovement->MaxSpeed = mProjectileInfo.MaxSpeed;

	mGunPos = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosSceneComponent"));
	mGunPos->SetupAttachment(mMeshComponent, FireSocket);

	//발사체 궤적 그리기에 사용되는 액터
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
	
	UE_LOG(LogTemp, Error, TEXT("Constructor InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
	UE_LOG(LogTemp, Error, TEXT("Constructor FireCoefficient : %f"), FireCoefficient);
	//InitialLifeSpan = 10.0f;
	
	
	//SetActorEnableCollision(true);
	//mCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
}

void AProjectile::PostInitializeComponents()
{
	UE_LOG(LogTemp, Error, TEXT("Pre PostInitialize FireCoefficient : %f"), FireCoefficient);
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("Post PostInitialize FireCoefficient : %f"), FireCoefficient);
}

void AProjectile::PreInitializeComponents()
{
	UE_LOG(LogTemp, Error, TEXT("Pre PreInitialize Firecoefficient : %f"), FireCoefficient);
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("Post PreInitialize FireCoefficient : %f"), FireCoefficient);
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
	// 타격지점의 노말벡터 방향에서 폭발
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
		UE_LOG(LogTemp, Error, TEXT("SpawnActorDeffered End"));
		if (EffectActor)
		{
			UE_LOG(LogTemp, Error, TEXT("Pre FinishSpawningActor"));
			/*	UE_LOG(LogTemp, Warning, TEXT("SpawnTransform X:%f Y:%f Z:%f"), spawnPos.X,
					spawnPos.Y,
					spawnPos.Z);*/


					//EffectActor->SurfaceHit = Impact;
			UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
			UE_LOG(LogTemp, Error, TEXT("FinishSpawningActor End"));
		}
		ApplyDamage(Impact);
	}
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
	//아직 안한거
	//블루프린트에서 Socket 생성 아직 안함.
	//블루프린트에서 mGunPos를 Socket에 붙여줘야됨.
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

	////발사 위치로 지정된 소켓위치를 찾음
	//if (mMeshComponent->DoesSocketExist(FireSocket))
	//{
	//	
	//	//FirePos = mMeshComponent->GetSocketLocation(FireSocket);
	//}
	//else
	//{
	//	FirePos = GetActorLocation();
	//}
	SetActorEnableCollision(true);
	UGameplayStatics::FinishSpawningActor(this, SpawnTr);
	
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
	//아직 안한거
	//블루프린트에서 Socket 생성 아직 안함.
	//블루프린트에서 mGunPos를 Socket에 붙여줘야됨.
	auto Controller = GetInstigatorController();
	FVector FirePos;
	FRotator FireRot;
	Controller->GetPlayerViewPoint(FirePos, FireRot);
	FirePos = mGunPos->GetComponentLocation();

	FPredictProjectilePathResult Result;
	FPredictProjectilePathParams Params;

	

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

	//몇개의 액터로 궤적을 그릴것인지 개수
	static float TrajectoryNum = 15;
	auto& posArr = Result.PathData;
	float stride = posArr.Num() / TrajectoryNum;

	UWorld* World = GetWorld();
	for(float i = 0; i<=posArr.Num()-1; i+=stride)
	{
		int idx = FMath::RoundToInt(i);
		auto pos = posArr[idx];
		World->SpawnActor(mTrajectoryActor, &pos.Location);
	}


	//타격지점에 폭발범위를 나타내는 액터 생성
	AActor* FireRangeActor = World->SpawnActor(mFireRangeActor, &ImpactPoint);

	FVector Origin, Extents;
	FireRangeActor->GetActorBounds(false, Origin, Extents, true);
	float Scale = mProjectileInfo.ExplodeRange / Extents.X;

	//UE_LOG(LogTemp, Warning, TEXT("BoundingBox Size : %s"), *Extents.ToString());
	FireRangeActor->SetActorScale3D(FVector(Scale, Scale, Scale));
}

void AProjectile::Clicking(float DeltaTime)
{
	//클릭하는 시간만큼 InitialSpeed의 값 증가
	FireRate += FireCoefficient * DeltaTime;
}

void AProjectile::PostActorCreated()
{
	UE_LOG(LogTemp, Error, TEXT("Pre PostActorCreated Firecoefficient : %f"), FireCoefficient);
	Super::PostActorCreated();
	UE_LOG(LogTemp, Error, TEXT("Post PostActorCreated FireCoefficient : %f"), FireCoefficient);
}
