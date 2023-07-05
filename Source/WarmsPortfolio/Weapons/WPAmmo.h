// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/Projectile.h"
#include "WPAmmo.generated.h"

/**
 * 
 */
UCLASS()
class WARMSPORTFOLIO_API AWPAmmo : public AProjectile
{
	GENERATED_BODY()
public:
	AWPAmmo();

protected:
	virtual void BeginPlay() override;

public:
	//�߻�ü �浹�� �ݹ�Ǵ� �Լ�
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	virtual void PostInitializeComponents() override;
		

	FName AmmoSocketName = TEXT("AmmoSocket");
};
