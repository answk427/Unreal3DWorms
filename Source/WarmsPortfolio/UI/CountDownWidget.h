// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CountDownWidget.generated.h"

/**
 * 
 */


UCLASS()
class WARMSPORTFOLIO_API UCountDownWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	int mCountTime;
	FTimerHandle mTimerHandle;
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountText;

	UFUNCTION()
	void CountStart(float countTime);
};
