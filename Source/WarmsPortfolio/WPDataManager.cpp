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


