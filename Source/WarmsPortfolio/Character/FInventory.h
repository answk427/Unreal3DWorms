// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GameObjectTypes.h"
#include "FInventory.generated.h"


/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_OneParam(FAddWeaponItemDelegate, const FItem&);
DECLARE_MULTICAST_DELEGATE_OneParam(FRemoveWeaponItemDelegate, const FItem&);

USTRUCT()
struct FItem
{
	GENERATED_BODY()

	FItem() : GameObjectType(EObjectTypeName::UnknownItem){}
	FItem(FName itemName, EObjectTypeName type) : ItemName(itemName), GameObjectType(type){}

	FName ItemName;
	EObjectTypeName GameObjectType;

	bool operator==(const FItem& other) const
	{
		return ItemName == other.ItemName && GameObjectType == other.GameObjectType;
	}
};


struct WARMSPORTFOLIO_API FInventory
{
public:
	FInventory();
	~FInventory();

	TArray<FItem> Weapons;

	FAddWeaponItemDelegate AddWeaponDelegate;
	FRemoveWeaponItemDelegate RemoveWeaponDelegate;
	
public:
	void AddWeaponItem(const FItem& item);
	void RemoveWeaponItem(const FItem& item);
	void RemoveWeaponItem(const int idx);
	
};
