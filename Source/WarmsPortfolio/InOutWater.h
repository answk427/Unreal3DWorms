// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InOutWater.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInOutWater : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class WARMSPORTFOLIO_API IInOutWater
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	bool bInWater = false;

	//물에 들어갈때 실행되는 함수
	virtual void EnterWater() = 0;

	//물에서 나올때 실행되는 함수
	virtual void ExitWater() = 0;
};
