// Fill out your copyright notice in the Description page of Project Settings.


#include "WPGameState.h"
#include "Character/PlayerCharacter.h"

AWPGameState::AWPGameState()
{
	TeamColors = { FLinearColor::Red, FLinearColor::Green, FLinearColor::Blue, FLinearColor::Black };
	TeamNames = { TEXT("레드팀"), TEXT("그린팀"), TEXT("블루팀"), TEXT("블랙팀") };
}

void AWPGameState::GetTeam(int TeamIdx, Team& OutTeam) const
{
	if (TeamIdx >= Teams.Num() || TeamIdx < 0)
		return;

	OutTeam = Teams[TeamIdx];
}

bool AWPGameState::RemoveCharacter(APlayerCharacter* PlayerCharacter)
{
	for(auto Team : Teams)
	{
		auto Idx = Team.Find(PlayerCharacter);
		//이번 팀에서 못찾으면 다음 팀을 탐색
		if (Idx == INDEX_NONE)
			continue;
		//찾으면 팀에서 해당 인덱스를 nullptr로 설정한 뒤 true 반환
		else
		{
			Team[Idx] = nullptr;
			return true;
		}
	}

	

	return false;
}

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
			//각 팀의 색으로 메테리얼 설정
			TeamSrc[CharacterNum*i + j]->Init(TeamColors[i], i);
		}
	}
}

void AWPGameState::InitTeam(const int TeamIdx, const Team& TeamSrc)
{
	Teams[TeamIdx] = TeamSrc;
}

int AWPGameState::CheckAliveTeam(int TeamIdx)
{
	int Alive = 0;

	if (TeamIdx < 0 || TeamIdx >= Teams.Num())
		return -1;

	//팀에 속한 캐릭터들이 살아있는지 확인
	for(auto Character : Teams[TeamIdx])
	{
		if (Character && !Character->bIsDead)
			++Alive;
	}

	//살아있는 인원 수 반환
	return Alive;
}

void AWPGameState::GetVictoryTeam(int& OutAliveTeamNum,int& OutVictoryTeam)
{
	//살아있는 팀의 수
	int AliveTeamNum = 0;
	
	int TempVictoryTeam = -1;

	//모든 팀에 대해서 전멸했는지 아닌지 검사
	for(int i=0; i<TeamNum; ++i)
	{
		int AliveNum = CheckAliveTeam(i);

		if (AliveNum == 0 || AliveNum == -1)
			continue;

		++AliveTeamNum;
		TempVictoryTeam = i;
		
	}

	OutAliveTeamNum = AliveTeamNum;
	if (AliveTeamNum == 1)
	{
		VictoryTeam = TempVictoryTeam;
		OutVictoryTeam = TempVictoryTeam;
	}
	else
		OutVictoryTeam = -1;
		
}

int AWPGameState::NextTurn()
{
	//CurrentTeam Update
	int NextTeam = -1;

	int Idx = CurrentTeam;

	//전멸하지 않은 다음팀을 찾음
	for(int i = 0; i<TeamNum-1; ++i)
	{
		Idx = (Idx + 1) % TeamNum;
				
		int AliveNum = CheckAliveTeam(Idx);

		if(AliveNum != 0 && AliveNum != -1)
		{
			NextTeam = Idx;
			break;
		}
	}
	CurrentTeam = NextTeam;
	
	return NextTeam;
}

APlayerCharacter* AWPGameState::GetRandomCharacterInTeam(const int TeamIdx)
{
	if (TeamIdx < 0 || TeamIdx >= TeamNum)
		return nullptr;

	APlayerCharacter* Character;

	do
	{
		Character= Teams[TeamIdx][FMath::RandRange(0, CharacterNum - 1)];
	} while (Character == nullptr || Character->IsPendingKill()); //while (!IsValid(Character));
		
	return Character;
}
