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

TMap<FName, UInventoryWeaponEntry*> UInventoryWeaponWidget::WeaponEntries;

void UInventoryWeaponWidget::AddEntry(const FItem& item)
{
	
	const FName& WeaponName = item.ItemName;
	UE_LOG(LogTemp, Error, TEXT("Inventory's Owner : %s, Inventory Address : %x"), *GetOuter()->GetName(), this);
	//이미 있으면 Entry를 생성하지 않음
	auto Entry = WeaponEntries.Find(WeaponName);
	if(Entry)
	{
		auto ViewItem = ItemInventory->ItemFromEntryWidget(*(*Entry));
		
		//ListView에 이미 있는 아이템이면 Return
		if (ViewItem != nullptr)
			return;
		ItemInventory->AddItem(WeaponEntries[WeaponName]);
		ItemInventory->RegenerateAllEntries();
		return;
	}

	//auto NewEntry = WeaponEntries.Emplace(WeaponName, NewObject<UInventoryWeaponEntry>(ItemInventory, InventoryEntryClass));
	auto EntryWidget = CreateWidget(ItemInventory, InventoryEntryClass);
	auto NewEntry = Cast<UInventoryWeaponEntry>(EntryWidget);
	check(NewEntry);
	auto FinalEntry = WeaponEntries.Add(WeaponName, NewEntry);
	
	
	

	//데이터테이블에서 테이블을 가져옴.
	static UWPGameInstance* GameInstance = Cast<UWPGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	check(GameInstance);
	UWPDataManager* DataManager = GameInstance->DataManager;
	check(DataManager);

	const FName& TableName = DataManager->GetDataTableName(item.GameObjectType);

	UDataTable* DataTable = DataManager->GetTable(TableName);

	//에러 발생시 알수있는 테이블 이름
	FString ContextString(TEXT("AddEntry_"));
	ContextString += WeaponName.ToString();
	
	
	FWeaponData* dataInfo = static_cast<FWeaponData*>(DataTable->FindRow<FWeaponData>(WeaponName, ContextString));
	check(dataInfo);

	//텍스쳐에 무기를 렌더링함
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(FinalEntry);
	auto CaptureHelper = UCaptureHelper::Instance();
	CaptureHelper->DrawActorToTexture(dataInfo->WeaponClass, RenderTarget, 256, 256);

	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(CaptureHelper->ItemEntryMat, FinalEntry);
	//MaterialInstance->SetTextureParameterValue(TEXT("ItemTexture"), RenderTarget);
	//Entry의 Image에 렌더타깃을 바인딩
	FinalEntry->InitEntry(RenderTarget, MaterialInstance, WeaponName);

	

	ItemInventory->AddItem(WeaponEntries[WeaponName]);
	ItemInventory->RegenerateAllEntries();

	for(int i=0; i<ItemInventory->GetNumItems(); ++i)
	{
		auto InventoryItem = ItemInventory->GetItemAt(i);
		auto EntryItem = Cast<UInventoryWeaponEntry>(InventoryItem);
		auto img = EntryItem->WeaponImage;
		auto txt = EntryItem->WeaponTextBlock;
	}

}

void UInventoryWeaponWidget::RemoveEntry(const FItem& item)
{
	auto entryPtr = WeaponEntries.Find(item.ItemName);
	if (entryPtr == nullptr)
		return;
	auto entry = *entryPtr;

	UE_LOG(LogTemp, Error, TEXT("Pre Remove ItemNums : %d"), ItemInventory->GetNumItems());
	ItemInventory->RemoveItem(entry);
	UE_LOG(LogTemp, Error, TEXT("Post Remove ItemNums : %d"), ItemInventory->GetNumItems());
	
	
}

void UInventoryWeaponWidget::BindInventory(TWeakPtr<FInventory> Inventory)
{
	if(Inventory.IsValid())
	{
		PlayerInventory = Inventory;
		auto Inven = PlayerInventory.Pin();
		Inven->AddWeaponDelegate.AddUObject(this, &UInventoryWeaponWidget::AddEntry);
		Inven->RemoveWeaponDelegate.AddUObject(this, &UInventoryWeaponWidget::RemoveEntry);
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
	ItemInventoryList->AddItem(Item);

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

