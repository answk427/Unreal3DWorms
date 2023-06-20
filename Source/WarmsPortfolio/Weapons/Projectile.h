// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "WarmsPortfolio/DataTableStructures.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class AProjectileExplosionEffect;

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

	void SetProjectileInfo(const FProjectileData* ProjectileData);
	virtual UShapeComponent* GetCollider() const override;
protected:
	//�߻�ü Collision
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* mCollisionComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USpringArmComponent* mSpringArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* mCamera;

	//�߻�ü movement component, ƽ���� �ٸ� ������Ʈ�� ��ġ�� ������Ʈ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta = (AllowPrivateAccess))
	UProjectileMovementComponent* mProjectileMovement;

	//�����Ǵ� ��������Ʈ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect, meta = (AllowPrivateAccess))
	TSubclassOf<AProjectileExplosionEffect> mExplosionFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess))
	TSubclassOf<AActor> mTrajectoryActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess))
	TSubclassOf<AActor> mFireRangeActor;
	//�߻�ü ����
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category= Projectile, meta = (AllowPrivateAccess))
	//FProjectileInfo mProjectileInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Projectile, meta = (AllowPrivateAccess))
	FProjectileData mProjectileInfo;

	UPROPERTY(VisibleDefaultsOnly)
	USceneComponent* mGunPos;

	FName FireSocket = TEXT("FireSocket");

	float FireRate = 1.0f;

	//1�ʵ��� �����ϴ� FireRate�� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float FireCoefficient = 1.0f;

	virtual void  Explode(const FHitResult& Impact);

	virtual void BeginPlay() override;
		
	virtual void ApplyDamage(const FHitResult& Impact);

	float GetInitialSpeed();
public:
	virtual void SetObjectType() override;
	virtual void Fire() override;

	virtual void SetWeaponData(const FWeaponData* WeaponData) override;
	virtual void DrawTrajectory() override;
	virtual void Clicking(float DeltaTime) override;
	virtual void PostActorCreated() override;
	virtual void InitMesh() override;
	
};





