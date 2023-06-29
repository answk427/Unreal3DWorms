// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MyHUD.generated.h"

/**
 * 
 */
class UPlayerStateWidget;
UCLASS()
class WARMSPORTFOLIO_API UMyHUD : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* AimImage;

	
	UPROPERTY(meta = (BindWidget))
	UPlayerStateWidget* PlayerStateList;
};
