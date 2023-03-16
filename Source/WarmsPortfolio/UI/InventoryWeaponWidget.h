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

	//���ε� �� �κ��丮
	TWeakPtr<FInventory> PlayerInventory;

public:
	//Entry Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryWeaponEntry> InventoryEntryClass;

public:
	//�κ��丮�� �������� �߰��� ������ ����Ǵ� �ݹ��Լ�
	void AddEntry(const FItem& item);
	void RemoveEntry(const FItem& item);
	void BindInventory(TWeakPtr<FInventory> Inventory);

	void AddToTileView(UObject* Item, TSubclassOf<UUserWidget> mEntryClass);
};
