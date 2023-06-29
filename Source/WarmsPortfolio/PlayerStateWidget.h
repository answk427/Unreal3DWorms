// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStateWidget.generated.h"

/**
 * 
 */
class UPlayerStateEntry;
class UListView;
class APlayerCharacter;

UCLASS()
class WARMSPORTFOLIO_API UPlayerStateWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	void DoubleClicked(UObject* Object);
	void InitEntries(TArray<TWeakObjectPtr<APlayerCharacter>> Characters);
	void UpdateEntries();
	void UpdateEntry(UUserWidget& Widget);
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UListView* PlayerStateList;
};
