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

	UPROPERTY()
	int CurrentTeam = 0;

	TArray<Team> Teams;

public:
	int GetTeamNum() const { return TeamNum; }
	int GetCharacterNum() const { return CharacterNum; }
public: 
	void InitTeams();
	//TeamSrc has All Character in Game. must TeamSrc.Num == TeamNum*CharacterNum
	void InitTeams(const Team& TeamSrc);
	void InitTeam(const int TeamIdx, const Team& TeamSrc);

	int NextTurn();
	APlayerCharacter* GetRandomCharacterInTeam(const int TeamIdx);
	
};
