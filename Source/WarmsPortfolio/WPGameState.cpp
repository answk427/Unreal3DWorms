// Fill out your copyright notice in the Description page of Project Settings.


#include "WPGameState.h"
#include "Character/PlayerCharacter.h"

void AWPGameState::InitTeams()
{
	Teams.SetNum(TeamNum);
}

void AWPGameState::InitTeams(const Team& TeamSrc)
{
	//must TeamSrc.Num == TeamNum * CharacterNum
	check(TeamSrc.Num() == (TeamNum*CharacterNum));

	Teams.SetNum(TeamNum);

	//Add All Charcter.
	for(int i=0; i<TeamNum; ++i)
	{
		for(int j=0; j<CharacterNum; ++j)
		{
			Teams[i].Add(TeamSrc[CharacterNum*i + j]);
		}
	}
}

void AWPGameState::InitTeam(const int TeamIdx, const Team& TeamSrc)
{
	Teams[TeamIdx] = TeamSrc;
}

int AWPGameState::NextTurn()
{
	//CurrentTeam Update
	CurrentTeam = (CurrentTeam + 1) % TeamNum;
	return CurrentTeam;
}

APlayerCharacter* AWPGameState::GetRandomCharacterInTeam(const int TeamIdx)
{
	if (TeamIdx < 0 || TeamIdx >= TeamNum)
		return nullptr;

	APlayerCharacter* Character;

	do
	{
		Character= Teams[TeamIdx][FMath::RandRange(0, CharacterNum - 1)];
	} while (!IsValid(Character));
		
	return Character;
}
