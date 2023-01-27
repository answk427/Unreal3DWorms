// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WPGameInstance.generated.h"

/**
 * 
 */

struct FProjectileData;
class UDataTable;

UCLASS()
class WARMSPORTFOLIO_API UWPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UWPGameInstance();
public:
	virtual void Init() override;

private:
	UDataTable* ProjectileTable;

	
};
