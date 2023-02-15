// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWeaponWidget.h"

#include "../WPGameInstance.h"
#include "../WPDataManager.h"
#include "../DataTableStructures.h"
#include "../CaptureHelper.h"
#include "InventoryWeaponEntry.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "../Weapons/Weapon.h"

TMap<FName, UInventoryWeaponEntry*> UInventoryWeaponWidget::WeaponEntries;

void UInventoryWeaponWidget::AddEntry(const FName& WeaponName, const FName& TableName)
{
	//이미 있으면 Entry를 생성하지 않음
	auto Entry = WeaponEntries.Find(WeaponName);
	if(Entry)
	{
		return;
	}
	//Map에 있던 Value값이 nullptr 일 때
	else if ((*Entry))
	{
		WeaponEntries[WeaponName] = NewObject<UInventoryWeaponEntry>();
	}
	

	//데이터테이블에서 테이블을 가져옴.
	static UWPGameInstance* GameInstance = Cast<UWPGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	check(GameInstance);
	WPDataManager* DataManager = GameInstance->DataManager.Get();
	check(DataManager);

	UDataTable* DataTable = DataManager->GetTable(TableName);

	//에러 발생시 알수있는 테이블 이름
	FString ContextString(TEXT("AddEntry_"));
	ContextString += WeaponName.ToString();
	
	
	FWeaponData* dataInfo = static_cast<FWeaponData*>(DataTable->FindRow<FWeaponData>(WeaponName, ContextString));
	check(dataInfo);

	
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(*Entry);
	auto CaptureHelper = UCaptureHelper::Instance();
	CaptureHelper->DrawActorToTexture(dataInfo->WeaponClass, RenderTarget, 256, 256);
	
}

