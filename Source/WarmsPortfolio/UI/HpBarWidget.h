// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HpBarWidget.generated.h"

/**
 * 
 */
class UEditableTextBox;
class UEditableText;
class UProgressBar;
class UStatComponent;
class UBorder;

UCLASS()
class WARMSPORTFOLIO_API UHpBarWidget : public UUserWidget
{
	GENERATED_BODY()

	UHpBarWidget(const FObjectInitializer& ObjInitializer);

	
	void UpdateHpBar();
	

private:
	//이 위젯과 연관된 캐릭터의 StatComponent
	TWeakObjectPtr<UStatComponent> currStatComponent;
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HpProgressBar;

	UPROPERTY(meta = (BindWidget))
	UBorder* HpBackGround;

	UPROPERTY(meta = (BindWidget))
	UEditableText* HpEditText;

	
		

public:
	void BindingStatComp(UStatComponent* StatComp);
};
