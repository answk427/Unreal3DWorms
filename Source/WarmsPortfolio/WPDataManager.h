// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameObjectTypes.h"
#include "UI/ItemStruct.h"
#include "WPDataManager.generated.h"


/**
 * 
 */
class UDataTable;
class AWeapon;
struct FWeaponData;
struct FInitDataTables;



USTRUCT()
struct FDataTableName 
{
	GENERATED_BODY()
	
	FDataTableName()
	{
		DataTableNames.Init(FName(""), 10);
		DataTableNames[EObjectTypeName::Projectile] = "Projectile";
	}

	TArray<FName> DataTableNames;
public:
	FName operator()(EObjectTypeName i) { return DataTableNames[i]; }
};

UCLASS()
class WARMSPORTFOLIO_API UWPDataManager : public UObject
{
	GENERATED_BODY()
public:
	UWPDataManager();
	

public:
	void InitDataTables();
	UDataTable* GetTable(const FName& TableName);
	UDataTable* GetTable(EObjectTypeName ItemType);

	FWeaponData* GetWeaponData(EObjectTypeName ItemType, const FName& ItemName, const FString& ContextString);
	TArray<FWPItem> GetAllItem(EObjectTypeName ItemType);
public:
	UPROPERTY()
	FDataTableName GetDataTableName;
	
private:
	//������ ���۵� �� �ε��� ���������̺��� ����
	UPROPERTY()
	UDataTable* DataTables;

	//���������̺�� �����ϱ� ���� Map
	TMap<FName, UDataTable*> DataTableMap;
};
