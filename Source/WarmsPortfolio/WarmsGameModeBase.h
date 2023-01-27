// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "WarmsGameModeBase.generated.h"



/**
 * 
 */
UCLASS()
class WARMSPORTFOLIO_API AWarmsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	AWarmsGameModeBase();

	virtual void BeginPlay() override;

public:
	TSubclassOf<UUserWidget> HUDWidget;
	UUserWidget* currentWidget;
	
};
