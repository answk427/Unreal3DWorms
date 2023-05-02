// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerEquipments.h"
#include "../Weapons/Weapon.h"


FPlayerEquipments::FPlayerEquipments()
{

}

void FPlayerEquipments::EquipWeapon(const FWPItem& item, const FWeaponData& WeaponData)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipWeapon"));

	//�ƹ��͵� �����ϰ� ���� ������
	if (CurrentWeapon == nullptr)
		CurrentWeapon = std::make_unique<std::pair<FWPItem, FWeaponData>>(FWPItem(), FWeaponData());

	//�̹� ���� �������� �������̸� ����
	if (CurrentWeapon->first.ItemName == item.ItemName)
		return;
	
	CurrentWeapon->first = item;
	CurrentWeapon->second = WeaponData;
}





