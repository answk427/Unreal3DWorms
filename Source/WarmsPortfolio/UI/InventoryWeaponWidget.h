	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Character/FInventory.h"
#include "InventoryWeaponWidget.generated.h"

/**
 * 
 */
class UInventoryWeaponEntry;
class UTileView;
class UListView;

UCLASS()
class WARMSPORTFOLIO_API UInventoryWeaponWidget : public UUserWidget
{
	GENERATED_BODY()
		
	static TMap<FName, UInventoryWeaponEntry*> WeaponEntries;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTileView* ItemInventory;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	UListView* ItemInventoryList;

	//바인딩 된 인벤토리
	TWeakPtr<FInventory> PlayerInventory;

public:
	//Entry 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryWeaponEntry> InventoryEntryClass;

public:
	//인벤토리에 아이템이 추가될 때마다 실행되는 콜백함수
	void AddEntry(const FItem& item);
	void RemoveEntry(const FItem& item);
	void BindInventory(TWeakPtr<FInventory> Inventory);

	void AddToTileView(UObject* Item, TSubclassOf<UUserWidget> mEntryClass);
};
