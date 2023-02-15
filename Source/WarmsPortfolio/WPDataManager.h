// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class UDataTable;
struct FInitDataTables;

class WARMSPORTFOLIO_API WPDataManager
{
public:
	WPDataManager();
	~WPDataManager();

public:
	void InitDataTables();
	UDataTable* GetTable(const FName& TableName);
	
	
private:
	//게임이 시작될 때 로딩할 데이터테이블의 집합
	UPROPERTY()
	UDataTable* DataTables;

	//데이터테이블에 접근하기 위한 Map
	TMap<FName, UDataTable*> DataTableMap;
};
