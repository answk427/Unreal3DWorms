// Fill out your copyright notice in the Description page of Project Settings.


#include "FInventory.h"

FInventory::FInventory()
{
}

FInventory::~FInventory()
{
}

void FInventory::AddWeaponItem(const FWPItem& item)
{
	bool FindItem = Weapons.Contains(item);
	//이미 있는 아이템이면 return
	if (FindItem)
		return;

	Weapons.Add(item);
	AddWeaponDelegate.Broadcast(item);
}

void FInventory::RemoveWeaponItem(const FWPItem& item)
{
	int idx;
	bool FindItem = Weapons.Find(item, idx);
	if (!FindItem)
		return;
	Weapons.RemoveAt(idx);
	RemoveWeaponDelegate.Broadcast(item);
}

void FInventory::RemoveWeaponItem(const int idx)
{
	bool IsItem =Weapons.IsValidIndex(idx);
	if (!IsItem)
		return;
		
	RemoveWeaponDelegate.Broadcast(Weapons[idx]);
	Weapons.RemoveAt(idx);
}
