	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Character/FInventory.h"
#include "InventoryWeaponWidget.generated.h"


class UInventoryWeaponEntry;
class UTileView;
class UListView;
struct FPlayerEquipments;
struct FWeaponData;
class AWeapon;
class UWPDataManager;


DECLARE_DELEGATE_TwoParams(FOnClickedInventory, const FWPItem&, const FWeaponData&);

UCLASS()
class WARMSPORTFOLIO_API UInventoryWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

	//Scene Capture를 여러번 하지 않기위해 static 변수로 사용
	static TMap<FName, UMaterialInstanceDynamic*> WeaponListItems;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(BindWidget))
	UTileView* ItemInventory;

	FOnClickedInventory ClickedInventory;
	

	//바인딩 된 인벤토리
	TWeakPtr<FInventory> PlayerInventory;
	
private:
	UPROPERTY()
	UWPDataManager* DataManager = nullptr;
		
	TWeakObjectPtr<UInventoryWeaponEntry> SelectedEntry;
	
public:
	//Entry 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UInventoryWeaponEntry> InventoryEntryClass;

public:
	//인벤토리에 아이템이 추가될 때마다 실행되는 콜백함수
	void AddListItem(const FWPItem& Item);
	//void RemoveEntry(const FItem& item);
	void BindInventory(TWeakPtr<FInventory> Inventory, TWeakPtr<FPlayerEquipments> Equipments);

	void AddToTileView(UObject* Item, TSubclassOf<UUserWidget> mEntryClass);
	void ItemRenderToTexture(const FWPItem& Item, UTextureRenderTarget2D* RenderTarget);
	void ItemDoubleClicked(UObject* Object);
	void ItemClicked(UObject* Object);

	UFUNCTION()
	void VisibleChanged(ESlateVisibility CurrVisibility);

	void ListInit();
	
protected:
	virtual void NativeConstruct() override;

public:
	virtual bool Initialize() override;


};

	


	




