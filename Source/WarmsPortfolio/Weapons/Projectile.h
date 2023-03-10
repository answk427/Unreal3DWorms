// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WarmsPortfolio/DataTableStructures.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class AProjectileExplosionEffect;
struct FProjectileData;

//발사체 정보
USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	float ProjectileRadius;

	UPROPERTY(EditAnywhere)
	float InitialSpeed;

	UPROPERTY(EditAnywhere)
	float MaxSpeed;

	UPROPERTY(EditAnywhere)
	float Weight; //무게

	UPROPERTY(EditAnywhere)
	float AttackPower; //공격력

	UPROPERTY(EditAnywhere)
	float ExplodeRange; //폭발범위


public:
	FProjectileInfo() :
		ProjectileRadius(5.0f), InitialSpeed(3000.0f), MaxSpeed(3000.0f),
		Weight(10.0f), AttackPower(10.0f),ExplodeRange(100.0f){}

	FProjectileInfo& operator=(const FProjectileData& tableData);
	
};

UCLASS()
class WARMSPORTFOLIO_API AProjectile : public AWeapon
{
	GENERATED_BODY()
public:
	AProjectile();
	virtual void PostInitializeComponents() override;
	virtual void PreInitializeComponents() override;

public:
	//현재 공격력. 플레이어의 효과에 따라 기본공격력에서 달라질 수 있음. (ex 공격력 2배)
	UPROPERTY()
	float mCurrAttackPower;

	void SetProjectileInfo(const FProjectileData* ProjectileData, float InitialSpeed);
protected:
	//발사체 Collision
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* mCollisionComp;

	//발사체 movement component, 틱동안 다른 컴포넌트의 위치를 업데이트함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess))
	UProjectileMovementComponent* mProjectileMovement;

	//스폰되는 폭발이펙트
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess))
	TSubclassOf<AProjectileExplosionEffect> mExplosionFX;

	//발사체 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Projectile, meta = (AllowPrivateAccess))
	FProjectileInfo mProjectileInfo;

	
	

	virtual void Explode(const FHitResult& Impact);

	virtual void BeginPlay() override;
		
	virtual void ApplyDamage(const FHitResult& Impact);
};




inline void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(10.0f);
	
}

