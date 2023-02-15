// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */


class WARMSPORTFOLIO_API FInventoryItem 
{
	
public:
	FInventoryItem();
	

	FName ItemName;

	UPROPERTY()
	UTextureRenderTarget2D* ItemTexture;

	
};
