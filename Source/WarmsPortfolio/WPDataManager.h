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

	FWeaponData* GetWeaponData(EObjectTypeName ItemType, const FName& ItemName, const FString& ContextString);
	TArray<FWPItem> GetAllItem(EObjectTypeName ItemType);
public:
	UPROPERTY()
	FDataTableName GetDataTableName;
	
private:
	//게임이 시작될 때 로딩할 데이터테이블의 집합
	UPROPERTY()
	UDataTable* DataTables;

	//데이터테이블에 접근하기 위한 Map
	TMap<FName, UDataTable*> DataTableMap;
};
