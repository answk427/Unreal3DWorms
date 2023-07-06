// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "WPDataManager.h"
#include "WPGameInstance.generated.h"

/**
 * 
 */


UCLASS()
class WARMSPORTFOLIO_API UWPGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	UWPGameInstance();

	int TeamNum = 2;
	int PawnNumInTeam = 3;

public:
	virtual void Init() override;
	virtual void BeginDestroy() override;
		

	UFUNCTION(BlueprintCallable)
	void SetTeamNum(int num)
	{
		TeamNum = num;
	}

	UFUNCTION(BlueprintCallable)
	void SetPawnNumInTeam(int num)
	{
		PawnNumInTeam = num;
	}

	int GetTeamNum() const { return TeamNum; }
	int GetPawnNumInTeam() const { return PawnNumInTeam; }

	   	
};
