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
	//�̹� ������ Entry�� �������� ����
	auto Entry = WeaponEntries.Find(WeaponName);
	if(Entry)
	{
		return;
	}
	//Map�� �ִ� Value���� nullptr �� ��
	else if ((*Entry))
	{
		WeaponEntries[WeaponName] = NewObject<UInventoryWeaponEntry>();
	}
	

	//���������̺��� ���̺��� ������.
	static UWPGameInstance* GameInstance = Cast<UWPGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
	check(GameInstance);
	WPDataManager* DataManager = GameInstance->DataManager.Get();
	check(DataManager);

	UDataTable* DataTable = DataManager->GetTable(TableName);

	//���� �߻��� �˼��ִ� ���̺� �̸�
	FString ContextString(TEXT("AddEntry_"));
	ContextString += WeaponName.ToString();
	
	
	FWeaponData* dataInfo = static_cast<FWeaponData*>(DataTable->FindRow<FWeaponData>(WeaponName, ContextString));
	check(dataInfo);

	
	UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>(*Entry);
	auto CaptureHelper = UCaptureHelper::Instance();
	CaptureHelper->DrawActorToTexture(dataInfo->WeaponClass, RenderTarget, 256, 256);
	
}

