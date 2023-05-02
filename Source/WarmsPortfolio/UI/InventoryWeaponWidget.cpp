// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWeaponWidget.h"

#include "AimingPointWidget.h"
#include "../WPGameInstance.h"
#include "../WPDataManager.h"
#include "../DataTableStructures.h"
#include "../CaptureHelper.h"
#include "InventoryWeaponEntry.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapons/Weapon.h"
#include "Components/TextBlock.h"
#include "Components/TileView.h"
#include "Components/ListView.h"
#include "../Character/PlayerEquipments.h"
#include "WarmsPortfolio/Weapons/Projectile.h"

TMap<FName, UMaterialInstanceDynamic*> UInventoryWeaponWidget::WeaponListItems;

//void UInventoryWeaponWidget::RemoveEntry(const FItem& item)
//{
//	auto entryPtr = WeaponListItems.Find(item.ItemName);
//	if (entryPtr == nullptr)
//		return;
//	auto entry = *entryPtr;
//
//	UE_LOG(LogTemp, Error, TEXT("Pre Remove ItemNums : %d"), ItemInventory->GetNumItems());
//	ItemInventory->RemoveItem(entry);
//	UE_LOG(LogTemp, Error, TEXT("Post Remove ItemNums : %d"), ItemInventory->GetNumItems());
//}

void UInventoryWeaponWidget::AddListItem(const FWPItem& Item)
{
	UE_LOG(LogTemp, Warning, TEXT("AddListItem"));

	if (PlayerInventory.IsValid())
	{

		auto ItemName = Item.ItemName;
		UMaterialInstanceDynamic* ItemMaterial;

		//해당 아이템을 캡쳐한 텍스쳐 메테리얼이 있는지 확인
		auto pItemMaterial = WeaponListItems.Find(ItemName);
		if (pItemMaterial == nullptr)
		{
			//텍스쳐가 없으면 생성하고 렌더링
			UTextureRenderTarget2D* ItemTexture = NewObject<UTextureRenderTarget2D>(this);
			ItemRenderToTexture(Item, ItemTexture);

			//렌더링한 텍스쳐를 사용하는 메테리얼 생성
			UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(UCaptureHelper::Instance()->ItemEntryMat, this);
			MaterialInstance->SetTextureParameterValue(TEXT("ItemTexture"), ItemTexture);
			ItemMaterial = MaterialInstance;
		}
		else
			ItemMaterial = *pItemMaterial;

		auto ListItem = NewObject<UListItem>(this, ItemName);
		ListItem->Init(Item, ItemMaterial);
		
		ItemInventory->AddItem(ListItem);
		
		

		ItemInventory->RegenerateAllEntries();
	}

}

void UInventoryWeaponWidget::BindInventory(TWeakPtr<FInventory> Inventory, TWeakPtr<FPlayerEquipments> PlayerEquipments)
{
	if (Inventory.IsValid())
	{
		PlayerInventory = Inventory;
		auto Inven = PlayerInventory.Pin();
		Inven->AddWeaponDelegate.AddUObject(this, &UInventoryWeaponWidget::AddListItem);
		//Inven->RemoveWeaponDelegate.AddUObject(this, &UInventoryWeaponWidget::RemoveItem);

		//인벤토리에 있는 모든 아이템을 TileView에 추가
		ListInit();
	}

	if (PlayerEquipments.IsValid())
	{
		//FPlayerEquipments* Equipments = PlayerEquipments.Pin().Get();
		ClickedInventory.BindSP(PlayerEquipments.Pin().ToSharedRef(), &FPlayerEquipments::EquipWeapon);
	}

}

void UInventoryWeaponWidget::AddToTileView(UObject* Item, TSubclassOf<UUserWidget> mEntryClass)
{
	/*UInventoryWeaponEntry* Entry = CreateWidget<UInventoryWeaponEntry>(this, mEntryClass);
	Entry->WeaponTextBlock->SetText(FText::FromString("TwoTwo"));
	ItemInventory->AddItem(Entry);*/
	UInventoryWeaponEntry* CastItem = Cast<UInventoryWeaponEntry>(Item);
	CastItem->Initialize();
	ItemInventory->AddItem(Item);


	UE_LOG(LogTemp, Warning, TEXT("AddToTileView Item Text : %s"), *CastItem->WeaponTextBlock->GetText().ToString());
	// Get all the entries in the UTileView and cast them to UInventoryWeaponEntry
	//TArray<UUserWidget*> DisplayedEntries = ItemInventory->GetDisplayedEntryWidgets();
	auto DisplayedEntries = ItemInventory->GetListItems();
	auto testEntries = ItemInventory->GetDisplayedEntryWidgets();
	for (UObject* EntryObj : DisplayedEntries)
	{
		UInventoryWeaponEntry* DisplayedEntry = Cast<UInventoryWeaponEntry>(EntryObj);
		check(DisplayedEntry);
		UE_LOG(LogTemp, Warning, TEXT("AddToTileView DisplayedItem Text Pre Modify : %s, bDisplayed : %d"), *DisplayedEntry->WeaponTextBlock->GetText().ToString(), ItemInventory->IsItemVisible(DisplayedEntry));

		if (DisplayedEntry)
		{
			DisplayedEntry->WeaponTextBlock->SetText(FText::FromString(FString(TEXT("TestTest"))));
		}
		UE_LOG(LogTemp, Warning, TEXT("AddToTileView DisplayedItem Text Post Modify : %s"), *DisplayedEntry->WeaponTextBlock->GetText().ToString());
	}

	// Regenerate all entries in the UTileView and show the inventory
	ItemInventory->RegenerateAllEntries();
	int nums = ItemInventory->GetNumItems();

	UE_LOG(LogTemp, Warning, TEXT("AddToTileView Final Item nums : %d"), nums);
	UE_LOG(LogTemp, Warning, TEXT("AddToTileView Final DisplayedItem nums : %d"), testEntries.Num());

}

void UInventoryWeaponWidget::ItemRenderToTexture(const FWPItem& Item, UTextureRenderTarget2D* RenderTarget)
{
	if (DataManager == nullptr)
	{
		//데이터테이블에서 테이블을 가져옴.
		static UWPGameInstance* GameInstance = Cast<UWPGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
		check(GameInstance);
		DataManager = GameInstance->DataManager;
	}

	check(DataManager);
	//에러 발생시 알수있는 테이블 이름
	FString ContextString(TEXT("CreatedWeaponWidget_"));
	ContextString += Item.ItemName.ToString();

	FWeaponData* dataInfo = DataManager->GetWeaponData(Item.GameObjectType, Item.ItemName, ContextString);
	check(dataInfo);

	//텍스쳐에 무기를 렌더링함
	if (RenderTarget == nullptr)
		RenderTarget = NewObject<UTextureRenderTarget2D>(this);

	auto CaptureHelper = UCaptureHelper::Instance();
	CaptureHelper->DrawActorToTexture(dataInfo->WeaponClass, RenderTarget, 256, 256);

}

void UInventoryWeaponWidget::ItemClicked(UObject* Object)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemdClicked"));
	auto EntryWidget = ItemInventory->GetEntryWidgetFromItem(Object);
	UInventoryWeaponEntry* Entry = Cast<UInventoryWeaponEntry>(EntryWidget);
	check(Entry);

	//선택된 아이템이 존재하면 그 아이템 선택을 해제
	if(SelectedEntry.IsValid())
	{
		SelectedEntry->UnSelected();
	}

	Entry->Selected();
	//선택된 아이템 변경
	SelectedEntry = Entry;
}

void UInventoryWeaponWidget::ItemDoubleClicked(UObject* Object)
{
	UE_LOG(LogTemp, Warning, TEXT("ItemDoubleClicked"));
	UListItem* ListItem = Cast<UListItem>(Object);
	check(ListItem);
	const FWPItem& Item = ListItem->GetItem();

	check(DataManager);
	FWeaponData* WeaponData = DataManager->GetWeaponData(Item.GameObjectType, Item.ItemName, FString("ItemDoubleClicked"));
	check(WeaponData);
	ClickedInventory.Execute(Item, *WeaponData);
}

void UInventoryWeaponWidget::VisibleChanged(ESlateVisibility CurrVisibility)
{
	auto PlayerController = GetWorld()->GetFirstPlayerController();
	UE_LOG(LogTemp, Warning, TEXT("VisibleChanged"));


	if (PlayerController != nullptr)
	{



		//GameMode.SetHideCursorDuringCapture(false);

		switch (CurrVisibility)
		{
			//인벤토리가 보이지 않으면 마우스를 감춤
		case ESlateVisibility::Collapsed:
			UE_LOG(LogTemp, Warning, TEXT("VisibleChanged ESlateVisibility::Collapsed"));
			PlayerController->bShowMouseCursor = false;
			PlayerController->SetInputMode(FInputModeGameOnly());
			break;
		case ESlateVisibility::SelfHitTestInvisible:
			UE_LOG(LogTemp, Warning, TEXT("VisibleChanged ESlateVisibility::SelfHitTestInvisible"));
			PlayerController->bShowMouseCursor = true;
			FInputModeGameAndUI GameMode;
			GameMode.SetWidgetToFocus(this->TakeWidget());
			PlayerController->SetInputMode(GameMode);
			break;
		}

	}
}

void UInventoryWeaponWidget::ListInit()
{
	TArray<UListItem*> ListItems;
	if (PlayerInventory.IsValid())
	{
		auto& WeaponItems = PlayerInventory.Pin()->Weapons;

		//Inventory에 있는 모든 아이템에 대해 ListItem 생성
		for (auto& Weapon : WeaponItems)
		{
			auto ItemName = Weapon.ItemName;
			UMaterialInstanceDynamic* ItemMaterial;

			//해당 아이템을 캡쳐한 텍스쳐 메테리얼이 있는지 확인
			auto pItemMaterial = WeaponListItems.Find(ItemName);
			if (pItemMaterial == nullptr)
			{
				//텍스쳐가 없으면 생성하고 렌더링
				UTextureRenderTarget2D* ItemTexture = NewObject<UTextureRenderTarget2D>(this);
				ItemRenderToTexture(Weapon, ItemTexture);

				//렌더링한 텍스쳐를 사용하는 메테리얼 생성
				UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(UCaptureHelper::Instance()->ItemEntryMat, this);
				MaterialInstance->SetTextureParameterValue(TEXT("ItemTexture"), ItemTexture);
				ItemMaterial = MaterialInstance;
			}
			else
				ItemMaterial = *pItemMaterial;

			auto ListItem = NewObject<UListItem>(this, ItemName);
			ListItem->Init(Weapon, ItemMaterial);
			ListItems.Add(ListItem);
		}
	}

	ItemInventory->SetListItems(ListItems);
	ItemInventory->RegenerateAllEntries();
}

void UInventoryWeaponWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Warning, TEXT("NativeConstruct"));
	ItemInventory->OnItemDoubleClicked().AddUObject(this, &UInventoryWeaponWidget::ItemDoubleClicked);
	ItemInventory->OnItemClicked().AddUObject(this, &UInventoryWeaponWidget::ItemClicked);
}

bool UInventoryWeaponWidget::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize widget"));
	TScriptDelegate<> InventoryVisibleChanged;
	InventoryVisibleChanged.BindUFunction(this, FName("VisibleChanged"));
	OnVisibilityChanged.Add(InventoryVisibleChanged);
	return Super::Initialize();
}

