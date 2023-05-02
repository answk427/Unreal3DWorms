// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemStruct.h"


void UListItem::Init(const FWPItem& ItemSrc, UMaterialInstanceDynamic* MaterialSrc)
{
	Item = ItemSrc;
	ItemMaterial = MaterialSrc;
}

uint32 GetTypeHash(const FWPItem& KeyItem)
{
	return GetTypeHash(KeyItem.ItemName.GetComparisonIndex());
}


