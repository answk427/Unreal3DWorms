// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Templates/SubclassOf.h"

#include "WarmsGameModeBase.generated.h"


/**
 * 
 */

class UItemActorManager;
class USoundManager;
class UWPDataManager;
class APlayerCharacter;
class AWorldData;
class ACineCameraActor;
class AGraveActor;
class UMyHUD;



UCLASS()
class WARMSPORTFOLIO_API AWarmsGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
	AWarmsGameModeBase();

	virtual void BeginPlay() override;

private:
	TSubclassOf<APlayerCharacter> CharacterClass;
	

	//죽으면 스폰할 묘지 액터
	TSubclassOf<AGraveActor> GraveClass;

	AWorldData* WorldData;
	ACineCameraActor* WorldCamera;

	//Number of Player Damaged by Weapon
	int DamagedPlayerNum = 0;
	//이번 턴에서 공격받은 플레이어들의 배열
	TArray<TWeakObjectPtr<APlayerCharacter>> DamagedPlayers;
	//이번 턴에서 죽은 플레이어들의 배열
	TArray<TWeakObjectPtr<APlayerCharacter>> DeadPlayers;

	

	FTimerHandle SwitchCameraTimerHandler;
	FTimerHandle WorldCameraTimerHandle;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float LookGraveDuration = 1.5f;

private:
	bool CheckDamagedPlayer(TWeakObjectPtr<APlayerCharacter> Player);

	//팀의 수, 팀 당 인원 수 설정
	void InitTeamData();
	
public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	TSubclassOf<UUserWidget> HUDWidget;

	UPROPERTY()
	UUserWidget* currentWidget;

	UPROPERTY()
	UMyHUD* CastedHUD;

	UPROPERTY()
	UItemActorManager* ItemActorManager;

	UPROPERTY(BlueprintReadWrite)
	USoundManager* SoundManager;

	UPROPERTY()
	UWPDataManager* DataManager;

public:
	void AddDamagedPlayer(TWeakObjectPtr<APlayerCharacter> Player);
	void AddDeadPlayer(TWeakObjectPtr<APlayerCharacter> Player);

	void RemoveDamagedPlayer(TWeakObjectPtr<APlayerCharacter> Player);

	bool ChangeCharacter(APlayerCharacter* PlayerCharacter);
	void InitWorldData();

	//Called when Game Start
	UFUNCTION()
	void SpawnAllCharacters();

	//Check Spawn Location, whether to die or not
	bool CheckHeight(float X, float Y, FVector& OutSpawnLocation);

	void NextTurn(); 
	void SwitchCamera(AActor* Camera);

	//Switch Camera between Characters
	void SwitchMultiCamera(float Duration, float BlendTime = 0.f);

	UFUNCTION(BlueprintCallable)
	void SwitchDamagedCharacterCamera(float Duration, float BlendTime = 0.75f);

	void SwitchDeadCharacterCamera(float Duration, float BlendTime = 0.75f);


	void UseWorldCamera(const FVector& WorldLocation, const FRotator& Rotator, const AActor* TargetActor = nullptr, float Duration = 1.0f, float BlendTime = 0.4f);

	void DieCharacter(TWeakObjectPtr<APlayerCharacter> Player);

	//Bind Function to VoxelWorld Delegate
	void TerrainInit();

	void CameraInit();

	void InitEntry(int TeamIdx);

	virtual void PostLoad() override;
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	void GameEnd();

	UFUNCTION(BlueprintCallable)
	void ClearTimer();
	

	UFUNCTION(BlueprintCallable)
	FString GetVictoryTeamPhrase();
protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaSeconds) override;
};



