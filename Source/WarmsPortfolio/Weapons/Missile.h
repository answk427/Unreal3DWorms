// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Missile.generated.h"

/**
 * 
 */
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
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
