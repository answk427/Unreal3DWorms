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
	//������ ���۵� �� �ε��� ���������̺��� ����
	UPROPERTY()
	UDataTable* DataTables;

	//���������̺� �����ϱ� ���� Map
	TMap<FName, UDataTable*> DataTableMap;
};
