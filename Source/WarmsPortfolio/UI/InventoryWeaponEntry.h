// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "InventoryWeaponEntry.generated.h"

/**
 * 
 */
class UImage;
class UTextBlock;
class UBorder;




UCLASS()
class WARMSPORTFOLIO_API UInventoryWeaponEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

private:
	UTexture2D* RenderTargetToTexture(UTextureRenderTarget2D* RenderTarget);
	void SetWeaponImageTexture(UTexture* Texture2D, const FName& TextureName);
public:
		
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* WeaponTextBlock;

	UPROPERTY(meta = (BindWidget))
	UBorder* SelectedBorder;

	void InitEntry(UMaterialInstanceDynamic* RenderTargetMat, const FName& WeaponName);
	void Selected();
	void UnSelected();

protected:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};


