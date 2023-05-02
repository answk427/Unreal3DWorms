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

//�߻�ü ����
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
	float Weight; //����

	UPROPERTY(EditAnywhere)
	float AttackPower; //���ݷ�

	UPROPERTY(EditAnywhere)
	float ExplodeRange; //���߹���


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
	//���� ���ݷ�. �÷��̾��� ȿ���� ���� �⺻���ݷ¿��� �޶��� �� ����. (ex ���ݷ� 2��)
	UPROPERTY()
	float mCurrAttackPower;

	void SetProjectileInfo(const FProjectileData* ProjectileData, float InitialSpeed);
	virtual UShapeComponent* GetCollider() const override;
protected:
	//�߻�ü Collision
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* mCollisionComp;

	//�߻�ü movement component, ƽ���� �ٸ� ������Ʈ�� ��ġ�� ������Ʈ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess))
	UProjectileMovementComponent* mProjectileMovement;

	//�����Ǵ� ��������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess))
	TSubclassOf<AProjectileExplosionEffect> mExplosionFX;

	//�߻�ü ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Projectile, meta = (AllowPrivateAccess))
	FProjectileInfo mProjectileInfo;

	
	

	virtual void Explode(const FHitResult& Impact);

	virtual void BeginPlay() override;
		
	virtual void ApplyDamage(const FHitResult& Impact);

public:
	virtual void SetObjectType() override;
};




inline void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(10.0f);
	
}

