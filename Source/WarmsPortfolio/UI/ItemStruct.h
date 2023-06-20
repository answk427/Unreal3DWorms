// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameObjectTypes.h"
#include "ItemStruct.generated.h"


USTRUCT()
struct FWPItem
{
	GENERATED_BODY()

	FWPItem() : GameObjectType(EObjectTypeName::UnknownItem) {}
	FWPItem(FName itemName, EObjectTypeName type) : ItemName(itemName), GameObjectType(type) {}

	FName ItemName;
	EObjectTypeName GameObjectType;

	bool operator==(const FWPItem& other) const
	{
		return ItemName == other.ItemName && GameObjectType == other.GameObjectType;
	}
};

/**
 * 
 */
UCLASS()
class WARMSPORTFOLIO_API UListItem : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	FWPItem Item;

	UPROPERTY()
	UMaterialInstanceDynamic* ItemMaterial;
public:
	void Init(const FWPItem& ItemSrc, UMaterialInstanceDynamic* TextureSrc);
	const FWPItem& GetItem() const { return Item; }
	UMaterialInstanceDynamic* GetMaterial() { return ItemMaterial; }
};


uint32 GetTypeHash(const FWPItem& KeyItem);