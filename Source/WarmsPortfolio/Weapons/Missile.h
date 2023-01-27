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
	//발사체 충돌시 콜백되는 함수
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
