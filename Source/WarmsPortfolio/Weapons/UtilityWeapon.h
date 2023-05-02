// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "UtilityWeapon.generated.h"

/**
 * 
 */
UCLASS()
class WARMSPORTFOLIO_API AUtilityWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void SetObjectType() override;
		
};
