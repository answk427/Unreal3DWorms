// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "WPGameState.generated.h"

/**
 * 
 */

class APlayerCharacter;
using Team = TArray<APlayerCharacter*>;

UCLASS()
class WARMSPORTFOLIO_API AWPGameState : public AGameStateBase
{
	GENERATED_BODY()
				
	//Total Team Num
	UPROPERTY(EditAnywhere)
	int TeamNum = 2;

	//Playerable Character per Team
	UPROPERTY(EditAnywhere)
	int CharacterNum = 3;

	//현재 플레이 중인 팀
	UPROPERTY()
	int CurrentTeam = 0;

	UPROPERTY()
	int VictoryTeam = -1;

	FString GetTeamName(int Team);

	TArray<Team> Teams;
	TArray<FString> TeamNames;

	TArray<FLinearColor> TeamColors;

public:
	AWPGameState();
	int GetTeamNum() const { return TeamNum; }

	int GetCharacterNum() const { return CharacterNum; }

	void GetTeam(int TeamIdx, Team& OutTeam) const;

	bool RemoveCharacter(APlayerCharacter* PlayerCharacter);
public:
	void SetTeamNum(int Num);
	void SetCharacterNum(int Num);

	void InitTeams();
	//TeamSrc has All Character in Game. must TeamSrc.Num == TeamNum*CharacterNum
	void InitTeams(const Team& TeamSrc);
	void InitTeam(const int TeamIdx, const Team& TeamSrc);

	int CheckAliveTeam(int TeamIdx);

	//false면 게임이 끝나지 않음.
	void GetVictoryTeam(int& OutAliveTeamNum, int& OutVictoryTeam);

	FString GetVictoryTeamName();
	

	int NextTurn();
	APlayerCharacter* GetRandomCharacterInTeam(const int TeamIdx);

	
	
};

inline FString AWPGameState::GetVictoryTeamName()
{
	if (VictoryTeam < 0 || VictoryTeam >= TeamNames.Num())
		return FString();
	return TeamNames[VictoryTeam];
}
