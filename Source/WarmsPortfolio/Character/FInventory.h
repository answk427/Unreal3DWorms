// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/ItemStruct.h"
#include "FInventory.generated.h"



/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FAddWeaponItemDelegate, const FWPItem&);
DECLARE_MULTICAST_DELEGATE_OneParam(FRemoveWeaponItemDelegate, const FWPItem&);


USTRUCT()
struct WARMSPORTFOLIO_API FInventory
{
	GENERATED_BODY()
public:
	FInventory();
	~FInventory();

	TArray<FWPItem> Weapons;

	FAddWeaponItemDelegate AddWeaponDelegate;
	FRemoveWeaponItemDelegate RemoveWeaponDelegate;
	
public:
	void AddWeaponItem(const FWPItem& item);
	void RemoveWeaponItem(const FWPItem& item);
	void RemoveWeaponItem(const int idx);
		
};
