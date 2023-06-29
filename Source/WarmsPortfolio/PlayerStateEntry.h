// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PlayerStateEntry.generated.h"

/**
 * 
 */
class UProgressBar;
class APlayerCharacter;

UCLASS()
class WARMSPORTFOLIO_API UPlayerStateEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

private:
	void SetHpBar(float Ratio);

	//Hp가 몇초동안 줄어들지 설정	
	float HpUpdateTime = 2.f;
		
	//업데이트 목표 Hp
	float UpdateHpTarget = 0.f;

	bool bHpUpdate = false;
public:
	void BindCharacter(TWeakObjectPtr<APlayerCharacter> Character);
	void UpdateHpBar();

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* CharacterHp;

	UPROPERTY()
	TWeakObjectPtr<APlayerCharacter> BindedCharacter;

	
};


