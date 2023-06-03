// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWeaponEntry.h"
#include <Components/Image.h>
#include <Components/TextBlock.h>
#include <Components/Border.h>

#include "Engine/TextureRenderTarget2D.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ItemStruct.h"



void UInventoryWeaponEntry::InitEntry(UMaterialInstanceDynamic* RenderTargetMat, const FName& WeaponName)
{
	if(RenderTargetMat != nullptr)
	{
		WeaponImage->SetBrushFromMaterial(RenderTargetMat);
	}

	WeaponTextBlock->SetText(FText::FromName(WeaponName));
	
}

void UInventoryWeaponEntry::Selected()
{
	SelectedBorder->SetBrushColor(FLinearColor::Red);
}

void UInventoryWeaponEntry::UnSelected()
{
	SelectedBorder->SetBrushColor(FLinearColor::White);
}

void UInventoryWeaponEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	UListItem* Item = Cast<UListItem>(ListItemObject);
	InitEntry(Item->GetMaterial(), Item->GetItem().ItemName);
}


UTexture2D* UInventoryWeaponEntry::RenderTargetToTexture(UTextureRenderTarget2D* RenderTarget)
{
	UTexture2D* Texture2D = UTexture2D::CreateTransient(
		RenderTarget->SizeX, RenderTarget->SizeY);

	//UI Texture Setting
	//Texture2D->MipGenSettings = TMGS_NoMipmaps;
	
	Texture2D->AddToRoot();
	Texture2D->UpdateResource();

	

	//UTextureRenderTarget�� �ִ� �ȼ� �������� ImageData�� ����
	FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
	
	TArray<FColor> ImageData;
	
	RenderTargetResource->ReadPixels(ImageData, FReadSurfaceDataFlags(), 
		FIntRect(0,0,RenderTarget->SizeX, RenderTarget->SizeY));
	
	//ImageData�� Byte Size
	int32 Size = ImageData.Num() * sizeof(FColor);

	//Texture2D�� Lock�ϰ� �ʿ��� ������ŭ �Ҵ��Ͽ� ImageData�� ����
	Texture2D->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	void* Data = Texture2D->PlatformData->Mips[0].BulkData.Realloc(Size);
	FMemory::Memcpy(Data, ImageData.GetData(), Size);
	Texture2D->PlatformData->Mips[0].BulkData.Unlock();

	return Texture2D;
}

void UInventoryWeaponEntry::SetWeaponImageTexture(UTexture* Texture2D, const FName& TextureName)
{
	UMaterialInstanceDynamic* DynamicMat = UMaterialInstanceDynamic::Create(WeaponImage->GetDynamicMaterial(), this);
	DynamicMat->SetTextureParameterValue(TextureName, Texture2D);
	WeaponImage->SetBrushFromMaterial(DynamicMat);
	
}


//void UpdateRenderTarget()
//{
//	// Check if the Render Target and camera are valid
//	if (RenderTarget && Camera)
//	{
//		// Get the world and player controller
//		UWorld* World = GetWorld();
//		APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;
//
//		// Check if the player controller is valid
//		if (PlayerController)
//		{
//			// Get the view target of the camera
//			FViewTargetTransitionParams TransitionParams;
//			TransitionParams.BlendTime = 0.0f;
//			TransitionParams.BlendFunction = VTBlend_Linear;
//			PlayerController->SetViewTarget(Camera, TransitionParams);
//
//			// Render the view of the camera to the Render Target
//			FTextureRenderTargetResource* RenderTargetResource = RenderTarget->GameThread_GetRenderTargetResource();
//			UGameplayStatics::DrawMaterialToRenderTarget(World, RenderTargetResource, Material, FLinearColor::White, true);
//		}
//	}
//}
//};