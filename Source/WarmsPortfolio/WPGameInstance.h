// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WPDataManager.h"
#include "WPGameInstance.generated.h"

/**
 * 
 */


UCLASS()
class WARMSPORTFOLIO_API UWPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UWPGameInstance();
	
public:
	virtual void Init() override;
	
public:
	UPROPERTY()
	UWPDataManager* DataManager;
	
};
