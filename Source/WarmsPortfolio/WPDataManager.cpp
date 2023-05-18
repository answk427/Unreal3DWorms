// Fill out your copyright notice in the Description page of Project Settings.


#include "WPDataManager.h"

#include "DataTableStructures.h"
#include "Engine/DataTable.h"



UWPDataManager::UWPDataManager()
{
	static ConstructorHelpers::FObjectFinder<UDataTable> tables(TEXT("DataTable'/Game/DataTables/InitDataTables.InitDataTables'"));

	
	if (tables.Succeeded())
	{
		DataTables = tables.Object;
		UE_LOG(LogTemp, Warning, TEXT("Constructor InitDataTables Success"));
	}
}



void UWPDataManager::InitDataTables()
{
	if (DataTables)
	{
		auto& rowMap = DataTables->GetRowMap();
		
		
		for (auto pair : rowMap)
		{
			FInitDataTables* table = reinterpret_cast<FInitDataTables*>(pair.Value);

			if (table)
			{
				DataTableMap.Add(pair.Key, table->DataTable);
				UE_LOG(LogTemp, Warning, TEXT("DataTableMapAdd Success"));
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("InitDataTables Success"));
	}
	
}

UDataTable* UWPDataManager::GetTable(const FName& TableName)
{
	UDataTable** ppDataTable = DataTableMap.Find(TableName);

	if (ppDataTable == nullptr)
		return nullptr;
	else
		return *ppDataTable;
}

UDataTable* UWPDataManager::GetTable(EObjectTypeName ItemType)
{
	return GetTable(GetDataTableName(ItemType));
}

FWeaponData* UWPDataManager::GetWeaponData(EObjectTypeName ItemType, const FName& ItemName,
                                           const FString& ContextString)
{
	const FName& TableName = GetDataTableName(ItemType);
	

	UDataTable* DataTable = GetTable(TableName);
	if (DataTable == nullptr)
		return nullptr;

	FWeaponData* dataInfo = DataTable->FindRow<FWeaponData>(ItemName, ContextString);
	
	FProjectileData* dataInfo2 = DataTable->FindRow<FProjectileData>(ItemName, ContextString);

	dataInfo2->Print();

	return dataInfo;
}

TArray<FWPItem> UWPDataManager::GetAllItem(EObjectTypeName ItemType)
{
	TArray<FWPItem> Items;

	auto TableName = GetDataTableName(ItemType);
	UDataTable* DataTable = GetTable(TableName);
	
	if (DataTable == nullptr)
		return Items;

	TArray<FWeaponData*> Datas;
	DataTable->GetAllRows(TEXT("AllItem"), Datas);

	for(auto Data : Datas)
	{
		Items.Add(FWPItem(Data->WeaponName, ItemType));
	}

	return Items;
}


