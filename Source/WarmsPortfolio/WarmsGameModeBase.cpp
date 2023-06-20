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


AWarmsGameModeBase::AWarmsGameModeBase()
{

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

void AWarmsGameModeBase::BeginPlay()
{
	

	UE_LOG(LogTemp, Error, TEXT("*************%s BeginPlay*********************"), *GetName());
	InitWorldData();
	CameraInit();

	UWPGameInstance* GameInstance = Cast<UWPGameInstance>(GetGameInstance());
	check(GameInstance);
	auto DataManager = GameInstance->DataManager;

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
	ItemActorManager->SetSpawnTimer(GetWorld(), 5.0f, FVector(-1000.f, -1000.f, 300.f),
		FVector(1000.f, 1000.f, 350.f));
		
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

	/*	float X, Y;
		X = FMath::FRandRange(WorldData->MapMin.X, WorldData->MapMax.X);
		Y = FMath::FRandRange(WorldData->MapMin.Y, WorldData->MapMax.Y);

		FVector SpawnLocation;
		ValidLocation = CheckHeight(X, Y, SpawnLocation);
		SpawnLocation.Z += ZOffset;
		Tr.SetLocation(SpawnLocation);*/

		APlayerCharacter* SpawnedCharacter = 
			(APlayerCharacter*)GetWorld()->SpawnActor(CharacterClass, &Tr);
				

		Characters.Add(SpawnedCharacter);

		GetWorld()->GetFirstPlayerController()->Possess(SpawnedCharacter);
	}
	MyGameState->InitTeams(Characters);
	GetWorld()->GetFirstPlayerController()->Possess(MyGameState->GetRandomCharacterInTeam(0));
}

bool AWarmsGameModeBase::CheckHeight(float X, float Y, FVector& OutSpawnLocation)
{
	FHitResult OutHit;
	//up to down
	FVector Start(X,Y, WorldData->SpawnMaxHeight);
	FVector End(X, Y, WorldData->MapMin.Z);

	FCollisionObjectQueryParams ObjectQueryParams(ECollisionChannel::ECC_WorldStatic);
	
	
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
	 
	//Change Character in NextTeam
	APlayerCharacter* NextCharacter = nullptr;
	int NextTeam = MyGameState->NextTurn();
	do
	{
		
		NextCharacter = MyGameState->GetRandomCharacterInTeam(NextTeam);
	} while (!ChangeCharacter(NextCharacter));
}

void AWarmsGameModeBase::SwitchCamera(AActor* Camera)
{
	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(Camera, 1.f);
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
	}), Duration + BlendTime, true, 1.f);
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
		SwitchDeadCharacterCamera(Duration, BlendTime);
		//NextTurn();
		return;
	}
				
	//데미지를 입은 캐릭터 중, 화면전환에 적절한 다음 캐릭터를 찾음
	do
	{
		CurrentIdx = (CurrentIdx + 1) % DamagedPlayers.Num();
	} while(!CheckDamagedPlayer(DamagedPlayers[CurrentIdx]));

	DamagedPlayers[CurrentIdx].Get()->ActiveOneCamera(1);
	

	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(DamagedPlayers[CurrentIdx].Get(), BlendTime);
}

inline void AWarmsGameModeBase::SwitchDeadCharacterCamera(float Duration, float BlendTime)
{
	//Offset은 카메라가 한 캐릭터에 머무는 시간
	float Offset = BlendTime + Duration + 1.f;
	float FirstDelay = -Offset;
	UE_LOG(LogTemp, Error, TEXT("SwitchDeadCharacterCamera Duration : %f"), Duration);

	for(auto DeadPlayer : DeadPlayers)
	{
		if (!DeadPlayer.IsValid())
			continue;
		
		
		FirstDelay += Offset;

		DeadPlayer.Get()->ActiveOneCamera(1);

		//이번 턴에 죽은 모든 캐릭터들을 카메라로 촬영.
		//카메라로 비추는 캐릭터의 죽는 애니메이션을 실행
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
			[&, DeadPlayer]()
		{
				if(DeadPlayer.IsValid())
				{
					DeadPlayer->PlayerAnim->PlayDieMotion();
					GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(DeadPlayer.Get(), BlendTime);
				}

			
		}), Duration + BlendTime, false, FirstDelay);
	}

	FirstDelay += Offset;
	FTimerHandle TimerHandle;

	DeadPlayers.Reset();

	

	//모든 캐릭터를 비추고 몇초 뒤 다음턴으로 넘어감
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda(
		[&]()
	{
		NextTurn();
	}), Duration + BlendTime, false, FirstDelay);

}


void AWarmsGameModeBase::UseWorldCamera(const FVector& WorldLocation, const FRotator& Rotator, const AActor* TargetActor, float Duration, float BlendTime)
{
	WorldCamera->SetActorLocation(WorldLocation);
	WorldCamera->SetActorRotation(Rotator);

	if (!TargetActor)
		WorldCamera->LookatTrackingSettings.ActorToTrack = TargetActor;

	GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(WorldCamera, BlendTime);

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&]()
	{
		if (DamagedPlayerNum == 0)
		{
			NextTurn();
		}
			
	}), 1.f, false, BlendTime*1.5f);
}

void AWarmsGameModeBase::DieCharacter(TWeakObjectPtr<APlayerCharacter> Player)
{
	if (!Player.IsValid())
		return;

	//묘비를 스폰하기 전 카메라 위치를 저장한 뒤
	//World Camera로 대신한다.
	APlayerCharacter* CurrPlayer = Player.Get();
	FVector CameraLoc = CurrPlayer->mCamera->GetComponentLocation();
	FRotator CameraRot = CurrPlayer->mCamera->GetComponentRotation();
	

	//죽은 캐릭터 대신 묘비 스폰
	const FVector SpawnLocation = Player->GetActorLocation();
	const FRotator SpawnRotator = Player->GetActorRotation();
	AGraveActor* Grave = (AGraveActor*)GetWorld()->SpawnActor(AGraveActor::StaticClass(), &SpawnLocation, &SpawnRotator);
	Grave->SetMeshColor(FColor::Green);

	UseWorldCamera(CameraLoc, CameraRot, Grave, 0.f);
	
		

	//1초 뒤 캐릭터 삭제
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([&, CurrPlayer, Grave]()
	{
		if(IsValid(CurrPlayer))
			CurrPlayer->Destroy();
		Grave->GraveMesh->SetSimulatePhysics(true);
	}), 1.f, false, 2.0f);
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
