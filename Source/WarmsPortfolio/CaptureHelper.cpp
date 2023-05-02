// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureHelper.h"
#include <Camera/CameraComponent.h>
#include "Weapons/Weapon.h"

#include <Kismet/GameplayStatics.h>
#include "Engine/DataTable.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"

#include "EditorLevelUtils.h"
#include <Engine/DirectionalLight.h>
#include <Engine/SkyLight.h>

#include "Components/LightComponent.h"
#include "EngineUtils.h"
#include "Atmosphere/AtmosphericFog.h"
#include "Atmosphere/AtmosphericFogComponent.h"
#include "Engine/LevelStreaming.h"
#include "AssetRegistryModule.h"

#include "Engine/LevelStreamingDynamic.h"



UCaptureHelper::UCaptureHelper()
{
	ConstructorHelpers::FObjectFinder<UMaterialInterface> ItemEntryMatObj(TEXT("Material'/Game/UI/Mat_WeaponCapture.Mat_WeaponCapture'"));
	check(ItemEntryMatObj.Succeeded());
	ItemEntryMat = ItemEntryMatObj.Object;


	//ConstructorHelpers::FClassFinder<ULevelStreaming> FindCaptureLevel(
	//	TEXT("World'/Game/Maps/CaptureLevel.CaptureLevel'"));

	//
	////check(FindCaptureLevel.Succeeded());

	//if (FindCaptureLevel.Succeeded())
	//	CaptureLevel = FindCaptureLevel.Class;
}

UCaptureHelper* UCaptureHelper::Instance()
{
	static UCaptureHelper* instance = NewObject<UCaptureHelper>();
	return instance;
}


void UCaptureHelper::DrawActorToTexture(UClass* ActorClass, UTextureRenderTarget2D* RenderTarget,
	uint32 sizeX, uint32 sizeY)
{
	check(RenderTarget && ActorClass);
	RenderTarget->InitAutoFormat(sizeX, sizeY);

	//텍스처의 전체 색상을 초기화
	RenderTarget->ClearColor = FLinearColor(0.f, 1.f, 0.f, 1.f);
	RenderTarget->UpdateResource();



	//액터 하나를 렌더링하기 위해 생성한 월드공간
	UWorld* OffscreenWorld = UWorld::CreateWorld(EWorldType::GamePreview, true);


	auto WorldSetting = OffscreenWorld->GetWorldSettings();

	UE_LOG(LogTemp, Warning, TEXT("OffscreenWorld : %p, WorldSetting : %p"), OffscreenWorld, WorldSetting);

	check(GetWorld());

	(WorldSetting->LightmassSettings) = (GetWorld()->GetWorldSettings()->LightmassSettings);

	//현재 월드의 평행광을 모두 복사해 새로운 월드에 추가. 같은 환경 조성
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		auto Light = *It;

		FActorSpawnParameters Params;
		Params.Template = Light;
		auto SpawnedLight = OffscreenWorld->SpawnActor<ADirectionalLight>(Params);
	}
	//현재 월드의 SkyLight를 모두 복사해 새로운 월드에 추가. 같은 환경 조성
	for (TActorIterator<ASkyLight> It(GetWorld()); It; ++It)
	{
		auto SkyLight = *It;
		FActorSpawnParameters Params;
		Params.Template = SkyLight;
		auto SpawnedLight = OffscreenWorld->SpawnActor<ASkyLight>(Params);
		SpawnedLight->SetActorRotation(FRotator(0.f, 45.f, 0.f));
	}
	for (TActorIterator<AAtmosphericFog> It(GetWorld()); It; ++It)
	{
		auto AtmosphericFog = *It;
		FActorSpawnParameters Params;
		Params.Template = AtmosphericFog;
		auto SpawnedFog = OffscreenWorld->SpawnActor<AAtmosphericFog>(Params);

	}


	//월드에 Directional Light 추가
	ADirectionalLight* DirectionalLight = OffscreenWorld->SpawnActor<ADirectionalLight>(
		FVector(100.0f, 100.0f, 300.0f), FRotator(-6.482239f, -66.309387f, 7.728075f));
	DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::White);
	DirectionalLight->GetLightComponent()->SetIntensity(2.75f);
	DirectionalLight->SetActorRotation(FRotator(0.f, 135.f, 0.f));


	//캡쳐할 엑터를 스폰시킴
	const FTransform ActorTr(FRotator(0.0f, 0.0f, 0.0f),
		FVector(0.0f, 0.0f, 0.0f));
	AActor* SpawnedActor = OffscreenWorld->SpawnActor<AActor>(ActorClass, ActorTr);



	// 캡쳐 컴포넌트 생성, 초기화
	USceneCaptureComponent2D* SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(SpawnedActor);

	SceneCaptureComponent->AttachToComponent(SpawnedActor->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform);

	FMinimalViewInfo ViewInfo;
	//ViewInfo.AspectRatio = 1.0f; //종횡비
	//ViewInfo.FOV = 90.0f; //수직시야각
	//ViewInfo.ProjectionMode = ECameraProjectionMode::Perspective;

	
	//Mesh의 크기를 얻어옴
	/*UMeshComponent* MeshComponent = SpawnedActor->FindComponentByClass<UMeshComponent>();
	FVector BoxExtent = MeshComponent->Bounds.BoxExtent;*/
	
	FVector BoxExtent;
	FVector Origin;
	SpawnedActor->GetActorBounds(false, Origin, BoxExtent);
	float MaxExtent = BoxExtent.GetAbsMax();
	
	//직교투영 설정
	ViewInfo.ProjectionMode = ECameraProjectionMode::Orthographic;
	ViewInfo.OrthoWidth = MaxExtent * 2.5f;
	ViewInfo.OrthoNearClipPlane = 0.0f;
	ViewInfo.OrthoFarClipPlane = MaxExtent*3;
		
	SceneCaptureComponent->SetCameraView(ViewInfo);
	
	float CameraDistance;
	CameraDistance = (10.0f / 8.0f)*(MaxExtent + 1);
	//카메라의 위치,속성 설정
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->SetRelativeLocation(FVector(-CameraDistance, 0.f, 0.f));
	SceneCaptureComponent->TextureTarget = RenderTarget;
	SceneCaptureComponent->CaptureSource = SCS_SceneColorHDR;
	SceneCaptureComponent->RegisterComponent();
	SpawnedActor->AddOwnedComponent(SceneCaptureComponent);

	//액터를 제외한 다른것들은 캡쳐하지 않음
	SceneCaptureComponent->ShowOnlyActorComponents(SpawnedActor);

	SceneCaptureComponent->CaptureScene();

	//임시로 생성했던 월드 파괴
	OffscreenWorld->DestroyWorld(true);

}





//void UCaptureHelper::DrawActorToTexture(UClass* ActorClass, UTextureRenderTarget2D* RenderTarget,
//	uint32 sizeX, uint32 sizeY)
//{
//
//	check(RenderTarget && ActorClass);
//	RenderTarget->InitAutoFormat(sizeX, sizeY);
//	//텍스처의 전체 색상을 초기화
//	RenderTarget->ClearColor = FLinearColor(0.f, 1.f, 0.f, 1.f);
//	RenderTarget->UpdateResource();
//
//	//액터 하나를 렌더링하기 위해 생성한 월드공간
//	UWorld* OffscreenWorld = GetWorld();
//	check(OffscreenWorld);
//	
//	//OffscreenWorld->InitWorld();
//	//월드에 Directional Light 추가
//	
//
//	const FTransform ActorTr(FRotator(0.0f, 0.0f, 0.0f),
//		FVector(0.0f, 0.0f, 300.0f));
//	AActor* SpawnedActor = OffscreenWorld->SpawnActor<AActor>(ActorClass, ActorTr);
//
//	// Create the SceneCaptureComponent
//	USceneCaptureComponent2D* SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(SpawnedActor);
//
//	SceneCaptureComponent->AttachToComponent(SpawnedActor->GetRootComponent(),
//		FAttachmentTransformRules::KeepRelativeTransform);
//
//	FMinimalViewInfo ViewInfo;
//	ViewInfo.AspectRatio = 1.0f; //종횡비
//	ViewInfo.FOV = 90.0f; //수직시야각
//	
//
//	SceneCaptureComponent->SetCameraView(ViewInfo);
//
//	//카메라의 위치,속성 설정
//
//	SceneCaptureComponent->bCaptureEveryFrame = false;
//	SceneCaptureComponent->SetRelativeLocation(FVector(-25.0f, 0.f, 0.f));
//	//SceneCaptureComponent->SetWorldLocationAndRotation(FVector(0, -100, 0), FRotator(0, 0, 0));
//	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Perspective;
//	SceneCaptureComponent->TextureTarget = RenderTarget;
//	SceneCaptureComponent->CaptureSource = SCS_FinalColorLDR;
//
//	SceneCaptureComponent->ShowOnlyActorComponents(SpawnedActor);
//
//	SceneCaptureComponent->RegisterComponent();
//	SpawnedActor->AddOwnedComponent(SceneCaptureComponent);
//
//
//	SceneCaptureComponent->CaptureScene();
//	SpawnedActor->Destroy();
//	
//}

void UCaptureHelper::Init(UWorld* world)
{
	World = world;
}

UWorld* UCaptureHelper::GetWorld() const
{
	return World;
}

void UCaptureHelper::LevelLoadedFunc()
{
	UE_LOG(LogTemp, Warning, TEXT("---------LevelLoadedFunc--------"));
	/*ULevelStreaming::ECurrentState state = DynamicLevel->GetCurrentState();

	switch (state)
	{
	case ULevelStreaming::ECurrentState::Removed:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel Removed"));
		break;
	case ULevelStreaming::ECurrentState::FailedToLoad:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel FailedToLoad"));
		break;
	case ULevelStreaming::ECurrentState::LoadedNotVisible:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel LoadedNotVisible"));
		break;
	case ULevelStreaming::ECurrentState::LoadedVisible:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel LoadedVisible"));
		break;
	case ULevelStreaming::ECurrentState::Loading:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel Loading"));
		break;
	case ULevelStreaming::ECurrentState::MakingInvisible:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel MakingInvisible"));
		break;
	case ULevelStreaming::ECurrentState::MakingVisible:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel MakingVisible"));
		break;
	case ULevelStreaming::ECurrentState::Unloaded:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel Unloaded"));
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("DynamicLevel Default"));
	}*/
}

void UCaptureHelper::LevelUnloadedFunc()
{
	UE_LOG(LogTemp, Warning, TEXT("---------LevelLoadedFunc--------"));
}
