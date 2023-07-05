// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Missile.generated.h"

/**
 * 
 */

class AWPAmmo;

UCLASS()
class WARMSPORTFOLIO_API AMissile : public AProjectile
{
	GENERATED_BODY()
public:
	AMissile();

protected:
	virtual void BeginPlay() override;

public:
	//�߻�ü �浹�� �ݹ�Ǵ� �Լ�
	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void PostInitializeComponents() override;
	virtual void Fire() override;

		
protected:
	virtual void Explode(const FHitResult& Impact) override;
	virtual void ApplyDamage(const FHitResult& Impact) override;

public:
	virtual void SetWeaponData(const FWeaponData* WeaponData) override;

	virtual void DrawTrajectory() override;
	virtual void Clicking(float DeltaTime) override;

	virtual AWeapon* GetFiredWeapon() override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* AmmoSocket;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AWPAmmo> AmmoClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AWPAmmo* Ammo;

	FName AmmoSocketName = TEXT("AmmoSocket");

	bool bAiming = false;
};


