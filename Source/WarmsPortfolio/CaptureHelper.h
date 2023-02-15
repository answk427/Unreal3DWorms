// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CaptureHelper.generated.h"

/**
 * 
 */
UCLASS()
class WARMSPORTFOLIO_API UCaptureHelper : public UObject
{
	GENERATED_BODY()

public:
	static UCaptureHelper* Instance();
		
	void DrawActorToTexture(UClass* ActorClass, UTextureRenderTarget2D* RenderTarget,
		uint32 sizeX, uint32 sizeY);
};


