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

	mStaticMesh->SetupAttachment(RootComponent);

	mCollisionComp->InitSphereRadius(mProjectileInfo.ProjectileRadius);
	mProjectileMovement->InitialSpeed = mProjectileInfo.InitialSpeed;
	mProjectileMovement->MaxSpeed = mProjectileInfo.MaxSpeed;

	UE_LOG(LogTemp, Error, TEXT("Constructor InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
	//InitialLifeSpan = 10.0f;
	
	
	//SetActorEnableCollision(true);
	//mCollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
}

void AProjectile::PostInitializeComponents()
{
	UE_LOG(LogTemp, Error, TEXT("Pre PostInitialize InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("Post PostInitialize InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
}

void AProjectile::PreInitializeComponents()
{
	UE_LOG(LogTemp, Error, TEXT("Pre PreInitialize InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("Post PreInitialize InitInfo.InitialSpeed : %f"), mProjectileInfo.InitialSpeed);
	mCollisionComp->InitSphereRadius(mProjectileInfo.ProjectileRadius);
	mProjectileMovement->InitialSpeed = mProjectileInfo.InitialSpeed;
	mProjectileMovement->MaxSpeed = mProjectileInfo.MaxSpeed;
}


void AProjectile::SetProjectileInfo(const FProjectileData* ProjectileData, float InitialSpeed)
{
	mProjectileInfo = *ProjectileData;
	mProjectileInfo.InitialSpeed = InitialSpeed;
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
