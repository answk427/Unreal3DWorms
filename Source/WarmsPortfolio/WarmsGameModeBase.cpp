// Fill out your copyright notice in the Description page of Project Settings.


#include "WarmsGameModeBase.h"

#include "ItemActorManager.h"
#include "Character/PlayerCharacter.h"
#include "UI/MyHUD.h"
#include "WPGameInstance.h"
#include "WPGameState.h"
#include "EngineUtils.h"
#include "WorldData.h"
#include "Camera/CameraComponent.h"
#include "Character/PlayerAnimation.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Voxel/Public/VoxelWorld.h"
#include "CinematicCamera/Public/CineCameraActor.h"
#include "GraveActor.h"
#include "PlayerStateWidget.h"
#include "SoundManager.h"
#include "CaptureHelper.h"
#include "WPGameInstance.h"

AWarmsGameModeBase::AWarmsGameModeBase()
{
	DataManager = CreateDefaultSubobject<UWPDataManager>(TEXT("DataManager"));

	//static ConstructorHelpers::FClassFinder<APlayerCharacter> BP_Character(TEXT("Blueprint'/Game/BluePrints/BP_PlayerCharacter_.BP_PlayerCharacter__C'"));

	//if (BP_Character.Succeeded())
		//DefaultPawnClass = BP_Character.Class;
	DefaultPawnClass = nullptr;

	GameStateClass = AWPGameState::StaticClass();

	static ConstructorHelpers::FClassFinder<UMyHUD> UI_HUD(TEXT("WidgetBlueprint'/Game/UI/HUD.HUD_C'"));
	
	if (UI_HUD.Succeeded())
	{
		HUDWidget = UI_HUD.Class;
		currentWidget = CreateWidget(GetWorld(), HUDWidget);
		if (currentWidget)
		{
			currentWidget->AddToViewport();
			CastedHUD = Cast<UMyHUD>(currentWidget);
		}
	}

	static ConstructorHelpers::FClassFinder<APlayerCharacter> MyCharacter(TEXT("Blueprint'/Game/BluePrints/BP_PlayerCharacter_.BP_PlayerCharacter__C'"));

	if(MyCharacter.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("-====--- MyCharacterClass Succeeded"));
		CharacterClass = MyCharacter.Class;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("-====--- MyCharacterClass Fail"));
	}
	ItemActorManager = CreateDefaultSubobject<UItemActorManager>(TEXT("ItemActorManager"));

	SoundManager = CreateDefaultSubobject<USoundManager>(TEXT("SoundManager"));

	UE_LOG(LogTemp, Warning, TEXT("WarmsGameModeBase Constructor"));

	
}

void AWarmsGameModeBase::TerrainInit()
{
	TArray<AActor*> Terrain;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), AVoxelWorld::StaticClass(),
		TEXT("Terrain"), Terrain);
	for (auto& Voxel : Terrain)
	{
		AVoxelWorld* VoxelWorld = Cast<AVoxelWorld>(Voxel);
		if (VoxelWorld)
		{
			UE_LOG(LogTemp, Error, TEXT("*************Find Voxel Terrain Success*********************"));
			VoxelWorld->OnWorldLoaded.AddDynamic(this, &AWarmsGameModeBase::SpawnAllCharacters);

			if (VoxelWorld->OnWorldLoaded.IsBound())
			{
				UE_LOG(LogTemp, Error, TEXT("*************VoxelWorld OnGenerateWorld Bound Success*********************"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("*************VoxelWorld OnGenerateWorld Bound Failed*********************"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("*************Find Voxel Terrain Success*********************"));
		}
	}
}

void AWarmsGameModeBase::CameraInit()
{
	TArray<AActor*> CineCamera;
	UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ACineCameraActor::StaticClass(),
		TEXT("WorldCamera"), CineCamera);
	for (auto& Cam : CineCamera)
	{
		ACineCameraActor* CineCam = Cast<ACineCameraActor>(Cam);
		if (CineCam)
		{
			UE_LOG(LogTemp, Error, TEXT("*************Find CineCamera Success*********************"));
			WorldCamera = CineCam;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("*************Find CineCamera Failed*********************"));
		}
	}
}

void AWarmsGameModeBase::InitEntry(int TeamIdx)
{
	auto MyGameState = GetGameState<AWPGameState>();

	Team team;
	MyGameState->GetTeam(TeamIdx, team);

	auto MyHUD = Cast<UMyHUD>(currentWidget);

	TArray<TWeakObjectPtr<APlayerCharacter>> EntryDatas;

	for (auto Player : team)
	{
		if(IsValid(Player))
			EntryDatas.Add(Player);
	}
	MyHUD->PlayerStateList->InitEntries(EntryDatas);
}

void AWarmsGameModeBase::BeginPlay()
{
	UE_LOG(LogTemp, Error, TEXT("*************%s BeginPlay*********************"), *GetName());

	UCaptureHelper::Instance()->Init(GetWorld());
	DataManager->InitDataTables();
	InitWorldData();
	CameraInit();
	InitTeamData();
		

	//������ ���̺�� �ִ� ��� �����ۿ� ���� �����ۿ���(����X, �����) ����
	for(int i=0; i<EObjectTypeName::UnknownItem; ++i)
	{
		auto Items = DataManager->GetAllItem((EObjectTypeName)i);
		for(auto& Item : Items)
		{
			ItemActorManager->AddItem(Item);
		}
	}

	//���� �ð����� ������ ���������� ����
	
	ItemActorManager->SetSpawnTimer(GetWorld(), 10.0f, WorldData->MapMin,
		WorldData->MapMax);
		
	TerrainInit();
}

bool AWarmsGameModeBase::CheckDamagedPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
	if (!Player.IsValid())
		return false;

	//공격받은 캐릭터가 멈추면 더 이상 카메라로 비추지 않음.
	if (Player->GetVelocity().IsNearlyZero())
	{
		Player->bTakingDamage = false;
		return false;
	}
		
	return true;
}

void AWarmsGameModeBase::InitTeamData()
{
	auto GameInstance = Cast<UWPGameInstance>(GetGameInstance());
	if (GameInstance == nullptr)
		return;

	auto GS = GetGameState<AWPGameState>();
	GS->SetTeamNum(GameInstance->GetTeamNum());
	GS->SetCharacterNum(GameInstance->GetPawnNumInTeam());
}

void AWarmsGameModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

}

void AWarmsGameModeBase::AddDamagedPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
	if (!Player.IsValid())
		return;

	//이미 추가되어있으면 리턴
	if (DamagedPlayers.Find(Player) != INDEX_NONE)
		return;  

	DamagedPlayers.Add(Player);
	++DamagedPlayerNum;

	if (DamagedPlayerNum == 1)
		SwitchMultiCamera(2.f, 0.7f);
}

void AWarmsGameModeBase::AddDeadPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
	if (!Player.IsValid())
		return;

	//이미 추가되어있으면 리턴
	if (DeadPlayers.Find(Player) != INDEX_NONE)
		return;

	DeadPlayers.Add(Player);
}

void AWarmsGameModeBase::RemoveDamagedPlayer(TWeakObjectPtr<APlayerCharacter> Player)
{
	if (!Player.IsValid())
		return;

	int Idx = -1;
	//TArray에 없으면 리턴
	if (!DamagedPlayers.Find(Player, Idx))
		return;

	
	DamagedPlayers[Idx].Reset();
	--DamagedPlayerNum;

	if (DamagedPlayerNum < 0)
		DamagedPlayerNum = 0;

	if (DamagedPlayerNum == 0)
		DamagedPlayers.Reset();
}

bool AWarmsGameModeBase::ChangeCharacter(APlayerCharacter* PlayerCharacter)
{
	if (!IsValid(PlayerCharacter))
		return false;
	auto PlController = GetWorld()->GetFirstPlayerController();

	if (!IsValid(PlController))
		return false;

	PlController->Possess(PlayerCharacter);

	PlayerCharacter->ActiveOneCamera(0);

	InitEntry(PlayerCharacter->TeamNumber);

	return true;
}

void AWarmsGameModeBase::InitWorldData()
{
	for(TActorIterator<AWorldData> It(GetWorld());  It; ++It)
	{
		WorldData = *It;
	}
}

void AWarmsGameModeBase::SpawnAllCharacters()
{
	auto MyGameState = GetGameState<AWPGameState>();

	int TeamNum = MyGameState->GetTeamNum();
	int CharacterNum = MyGameState->GetCharacterNum();

	
	Team Characters;
	FTransform Tr;
	float ZOffset = 100.f;

	
	//loop until a valid position is found.
	for(int i=0; i<TeamNum*CharacterNum; ++i)
	{
		bool ValidLocation = false;
		while(!ValidLocation)
		{
			FMath::RandInit(FDateTime::Now().GetTicks());
			float X, Y;
			X = FMath::FRandRange(WorldData->MapMin.X, WorldData->MapMax.X);
			Y = FMath::FRandRange(WorldData->MapMin.Y, WorldData->MapMax.Y);

			FVector SpawnLocation;
			ValidLocation = CheckHeight(X, Y, SpawnLocation);
			SpawnLocation.Z += ZOffset;
			Tr.SetLocation(SpawnLocation);
		}

	
		APlayerCharacter* SpawnedCharacter = 
			(APlayerCharacter*)GetWorld()->SpawnActor(CharacterClass, &Tr);
				
		
		Characters.Add(SpawnedCharacter);

		//GetWorld()->GetFirstPlayerController()->Possess(SpawnedCharacter);
	}
	MyGameState->InitTeams(Characters);
	ChangeCharacter(MyGameState->GetRandomCharacterInTeam(0));
	//GetWorld()->GetFirstPlayerController()->Possess(MyGameState->GetRandomCharacterInTeam(0));
	
}

bool AWarmsGameModeBase::CheckHeight(float X, float Y, FVector& OutSpawnLocation)
{
	FHitResult OutHit;
	//up to down
	FVector Start(X,Y, WorldData->SpawnMaxHeight);
	FVector End(X, Y, WorldData->SpawnMinHeight);

	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);


	/*if(GetWorld()->LineTraceSingleByProfile(OutHit, Start, End, FName(TEXT("WaterBodyCollision"))))
	{
		UE_LOG(LogTemp, Warning, TEXT("LineTrace Ocean"));
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Blue, 10.0f, 10.0f);
		return false;
	}*/
	
	if (GetWorld()->LineTraceSingleByObjectType(OutHit, Start, End, ObjectQueryParams))
	{
		OutSpawnLocation = OutHit.ImpactPoint;
		UE_LOG(LogTemp, Warning, TEXT("CheckHeight Success"));
		UKismetSystemLibrary::DrawDebugLine(GetWorld(), Start, End, FLinearColor::Red, 10.0f, 10.0f);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CheckHeight Failed"));
	}
	return false;

}

void AWarmsGameModeBase::NextTurn()
{
	auto MyGameState = GetGameState<AWPGameState>();

	//남은 팀 수
	int AliveTeamNum;
	//마지막으로 남은 팀
	int VictoryTeam;

	MyGameState->GetVictoryTeam(AliveTeamNum, VictoryTeam);

	//한팀이 승리
	if(AliveTeamNum == 1)
	{
		GameEnd();
		return;
	}
	//무승부
	else if(AliveTeamNum == 0)
	{
		return;
	}
	 
	//Change Character in NextTeam
	APlayerCharacter* NextCharacter = nullptr;
	int NextTeam = MyGameState->NextTurn();
	do
	{
		NextCharacter = MyGameState->GetRandomCharacterInTeam(NextTeam);
		UE_LOG(LogTemp, Warning, TEXT("NextTurn"));
		
	} while (!ChangeCharacter(NextCharacter));

	//Cast<UMyHUD>(currentWidget)->PlayerStateList->UpdateEntries();
}

void AWarmsGameModeBase::SwitchCamera(AActor* Camera)
{
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(Camera, 0.75f);
}

void AWarmsGameModeBase::SwitchMultiCamera(float Duration, float BlendTime)
{
	/*FLatentActionInfo ActionAfterDelay;
	ActionAfterDelay.CallbackTarget = this;
	ActionAfterDelay.ExecutionFunction = TEXT("SwitchDamagedCharacterCamera");
	UKismetSystemLibrary::Delay(GetWorld(), Duration + BlendTime, ActionAfterDelay);*/

	UE_LOG(LogTemp, Error, TEXT("SwitchMultiCamera Duration : %f"), Duration);
	GetWorld()->GetTimerManager().SetTimer(SwitchCameraTimerHandler, FTimerDelegate::CreateLambda(
		[&, Duration]()
	{
		//Switch Other Character's Camerae
		SwitchDamagedCharacterCamera(Duration);
		UE_LOG(LogTemp, Error, TEXT("SwitchMultiCamera In Timer"));
	}), Duration + BlendTime, true, 0.75f);
}

void AWarmsGameModeBase::SwitchDamagedCharacterCamera(float Duration, float BlendTime)
{
	static int CurrentIdx = -1;
	UE_LOG(LogTemp, Error, TEXT("SwitchDamagedCharacterCamera Duration : %f"), Duration);
	
	
	if (DamagedPlayerNum == 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(SwitchCameraTimerHandler);
		UE_LOG(LogTemp, Error, TEXT("SwitchDamagedCharacterCamera ClearTimer"));
		CurrentIdx = -1;

		//이번턴에 죽은 플레이어가 있을 때 죽는장면 화면전환
		if(DeadPlayers.Num() != 0)
			SwitchDeadCharacterCamera(Duration, BlendTime);
		else
			NextTurn();

		return;
	}

	for(int i=0; i<DamagedPlayers.Num(); ++i)
	{
		UE_LOG(LogTemp, Error, TEXT("SwitchDamagedCharacterCamera for"));

		auto Character = DamagedPlayers[i];

		if (!CheckDamagedPlayer(Character))
		{
			RemoveDamagedPlayer(Character);
		}
		else
		{
			CurrentIdx = i;
			break;
		}
	}
	
	//데미지를 입은 캐릭터 중, 화면전환에 적절한 다음 캐릭터를 찾음
	/*do
	{
		UE_LOG(LogTemp, Error, TEXT("SwitchDamagedCharacterCamera doWhile"));
		CurrentIdx = (CurrentIdx + 1) % DamagedPlayers.Num();
	} while(!CheckDamagedPlayer(DamagedPlayers[CurrentIdx]));*/


	if (CurrentIdx == -1)
		return;

	DamagedPlayers[CurrentIdx].Get()->ActiveOneCamera(1);
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(DamagedPlayers[CurrentIdx].Get(), BlendTime);
}

inline void AWarmsGameModeBase::SwitchDeadCharacterCamera(float Duration, float BlendTime)
{
	//Offset은 카메라가 한 캐릭터에 머무는 시간
	float Offset = BlendTime + Duration + LookGraveDuration;
	float FirstDelay = -Offset;
	UE_LOG(LogTemp, Error, TEXT("SwitchDeadCharacterCamera Duration : %f"), Duration);

	for(auto DeadPlayer : DeadPlayers)
	{
		if (!DeadPlayer.IsValid())
		{
			DeadPlayers.Pop();
			continue;
		}
					
		FirstDelay += Offset;

		DeadPlayer.Get()->ActiveOneCamera(1);

		//이번 턴에 죽은 모든 캐릭터들을 카메라로 촬영.
		//카메라로 비추는 캐릭터의 죽는 애니메이션을 실행
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
			[&, DeadPlayer]()
		{
				DeadPlayers.Pop();
				if(DeadPlayer.IsValid())
				{
					DeadPlayer->PlayerAnim->PlayDieMotion();
					GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(DeadPlayer.Get(), BlendTime);
				}
		}), Duration + BlendTime, false, FirstDelay);
	}

	FirstDelay += Offset;
	

	//DeadPlayers.Reset();

	
	//FTimerHandle TimerHandle;
	////모든 캐릭터를 비추고 몇초 뒤 다음턴으로 넘어감
	//GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
	//	[&]()
	//{
	//	NextTurn();
	//}), Duration + BlendTime, false, FirstDelay);

}


void AWarmsGameModeBase::UseWorldCamera(const FVector& WorldLocation, const FRotator& Rotator, const AActor* TargetActor, float Duration, float BlendTime)
{
	WorldCamera->SetActorLocation(WorldLocation);
	WorldCamera->SetActorRotation(Rotator);

	if (!TargetActor)
		WorldCamera->LookatTrackingSettings.ActorToTrack = TargetActor;

	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(WorldCamera, BlendTime);

	//FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(WorldCameraTimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		if (DamagedPlayerNum == 0 && DeadPlayers.Num() == 0)
		{
			NextTurn();
		}
			
	}), 1.f, false, Duration);
}

void AWarmsGameModeBase::DieCharacter(TWeakObjectPtr<APlayerCharacter> Player)
{
	if (!Player.IsValid())
		return;

	//묘비를 스폰하기 전 카메라 위치를 저장한 뒤
	//World Camera로 대신한다.
	APlayerCharacter* CurrPlayer = Player.Get();
	FVector CameraLoc = CurrPlayer->mCameraSky->GetComponentLocation();
	FRotator CameraRot = CurrPlayer->mCameraSky->GetComponentRotation();
	

	//죽은 캐릭터 대신 묘비 스폰
	const FVector SpawnLocation = Player->GetActorLocation();
	const FRotator SpawnRotator = Player->GetActorRotation();
	AGraveActor* Grave = (AGraveActor*)GetWorld()->SpawnActor(AGraveActor::StaticClass(), &SpawnLocation, &SpawnRotator);
	Grave->SetMeshColor(Player->TeamColor.ToFColor(true));

	UseWorldCamera(CameraLoc, CameraRot, nullptr, LookGraveDuration, 0.f);
	
		

	//1초 뒤 캐릭터 삭제
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&, CurrPlayer, Grave]()
	{
		auto MyGameState = GetGameState<AWPGameState>();
		MyGameState->RemoveCharacter(CurrPlayer);

		if(IsValid(CurrPlayer))
			CurrPlayer->Destroy();
		Grave->GraveMesh->SetSimulatePhysics(true);
				
	}), 1.f, false, 1.0f);

}

void AWarmsGameModeBase::PostLoad()
{
	Super::PostLoad();
	UE_LOG(LogTemp, Error, TEXT("*************%s PostLoad*********************"), *GetName());
}

void AWarmsGameModeBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("*************%s PreInitializeComponents*********************"), *GetName());
}

void AWarmsGameModeBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UE_LOG(LogTemp, Error, TEXT("*************%s PostInitializeComponents*********************"), *GetName());
}

void AWarmsGameModeBase::GameEnd()
{
	//UGameplayStatics::OpenLevel(GetWorld(), TEXT("GameOverLevel"));
	FLatentActionInfo Info;

	//UGameplayStatics::UnloadStreamLevel(GetWorld(), TEXT("FirstPersonExampleMap"), Info, false);
	
	UGameplayStatics::LoadStreamLevel(GetWorld(), TEXT("GameOverLevel"), true,
		false, Info);

	auto Controller = GetWorld()->GetFirstPlayerController();
	DisableInput(Controller);
	Controller->SetShowMouseCursor(true);
	Controller->bEnableClickEvents = false;
	Controller->SetIgnoreLookInput(true);
	Controller->SetInputMode(FInputModeUIOnly());
	
}

void AWarmsGameModeBase::ClearTimer()
{
	UWorld* World = GetWorld();
	ItemActorManager->ClearSpawnTimer(World);
	World->GetTimerManager().ClearTimer(SwitchCameraTimerHandler);
	World->GetTimerManager().ClearTimer(WorldCameraTimerHandle);
}

FString AWarmsGameModeBase::GetVictoryTeamPhrase()
{
	FString Pharse = GetGameState<AWPGameState>()->GetVictoryTeamName();
	if (Pharse.IsEmpty())
		Pharse = TEXT("무승부입니다!");

	Pharse += TEXT(" 승리!");

	return Pharse;
}

void AWarmsGameModeBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	UE_LOG(LogTemp, Warning, TEXT("GameModeBase EndPlay"));
}

void AWarmsGameModeBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UE_LOG(LogTemp, Warning, TEXT("GameMode %s Tick, Addr : %p"), *GetName(), this);
}
