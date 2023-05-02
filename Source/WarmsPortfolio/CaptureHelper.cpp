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

	//�ؽ�ó�� ��ü ������ �ʱ�ȭ
	RenderTarget->ClearColor = FLinearColor(0.f, 1.f, 0.f, 1.f);
	RenderTarget->UpdateResource();



	//���� �ϳ��� �������ϱ� ���� ������ �������
	UWorld* OffscreenWorld = UWorld::CreateWorld(EWorldType::GamePreview, true);


	auto WorldSetting = OffscreenWorld->GetWorldSettings();

	UE_LOG(LogTemp, Warning, TEXT("OffscreenWorld : %p, WorldSetting : %p"), OffscreenWorld, WorldSetting);

	check(GetWorld());

	(WorldSetting->LightmassSettings) = (GetWorld()->GetWorldSettings()->LightmassSettings);

	//���� ������ ���౤�� ��� ������ ���ο� ���忡 �߰�. ���� ȯ�� ����
	for (TActorIterator<ADirectionalLight> It(GetWorld()); It; ++It)
	{
		auto Light = *It;

		FActorSpawnParameters Params;
		Params.Template = Light;
		auto SpawnedLight = OffscreenWorld->SpawnActor<ADirectionalLight>(Params);
	}
	//���� ������ SkyLight�� ��� ������ ���ο� ���忡 �߰�. ���� ȯ�� ����
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


	//���忡 Directional Light �߰�
	ADirectionalLight* DirectionalLight = OffscreenWorld->SpawnActor<ADirectionalLight>(
		FVector(100.0f, 100.0f, 300.0f), FRotator(-6.482239f, -66.309387f, 7.728075f));
	DirectionalLight->GetLightComponent()->SetLightColor(FLinearColor::White);
	DirectionalLight->GetLightComponent()->SetIntensity(2.75f);
	DirectionalLight->SetActorRotation(FRotator(0.f, 135.f, 0.f));


	//ĸ���� ���͸� ������Ŵ
	const FTransform ActorTr(FRotator(0.0f, 0.0f, 0.0f),
		FVector(0.0f, 0.0f, 0.0f));
	AActor* SpawnedActor = OffscreenWorld->SpawnActor<AActor>(ActorClass, ActorTr);



	// ĸ�� ������Ʈ ����, �ʱ�ȭ
	USceneCaptureComponent2D* SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(SpawnedActor);

	SceneCaptureComponent->AttachToComponent(SpawnedActor->GetRootComponent(),
		FAttachmentTransformRules::KeepRelativeTransform);

	FMinimalViewInfo ViewInfo;
	//ViewInfo.AspectRatio = 1.0f; //��Ⱦ��
	//ViewInfo.FOV = 90.0f; //�����þ߰�
	//ViewInfo.ProjectionMode = ECameraProjectionMode::Perspective;

	
	//Mesh�� ũ�⸦ ����
	/*UMeshComponent* MeshComponent = SpawnedActor->FindComponentByClass<UMeshComponent>();
	FVector BoxExtent = MeshComponent->Bounds.BoxExtent;*/
	
	FVector BoxExtent;
	FVector Origin;
	SpawnedActor->GetActorBounds(false, Origin, BoxExtent);
	float MaxExtent = BoxExtent.GetAbsMax();
	
	//�������� ����
	ViewInfo.ProjectionMode = ECameraProjectionMode::Orthographic;
	ViewInfo.OrthoWidth = MaxExtent * 2.5f;
	ViewInfo.OrthoNearClipPlane = 0.0f;
	ViewInfo.OrthoFarClipPlane = MaxExtent*3;
		
	SceneCaptureComponent->SetCameraView(ViewInfo);
	
	float CameraDistance;
	CameraDistance = (10.0f / 8.0f)*(MaxExtent + 1);
	//ī�޶��� ��ġ,�Ӽ� ����
	SceneCaptureComponent->bCaptureEveryFrame = false;
	SceneCaptureComponent->SetRelativeLocation(FVector(-CameraDistance, 0.f, 0.f));
	SceneCaptureComponent->TextureTarget = RenderTarget;
	SceneCaptureComponent->CaptureSource = SCS_SceneColorHDR;
	SceneCaptureComponent->RegisterComponent();
	SpawnedActor->AddOwnedComponent(SceneCaptureComponent);

	//���͸� ������ �ٸ��͵��� ĸ������ ����
	SceneCaptureComponent->ShowOnlyActorComponents(SpawnedActor);

	SceneCaptureComponent->CaptureScene();

	//�ӽ÷� �����ߴ� ���� �ı�
	OffscreenWorld->DestroyWorld(true);

}





//void UCaptureHelper::DrawActorToTexture(UClass* ActorClass, UTextureRenderTarget2D* RenderTarget,
//	uint32 sizeX, uint32 sizeY)
//{
//
//	check(RenderTarget && ActorClass);
//	RenderTarget->InitAutoFormat(sizeX, sizeY);
//	//�ؽ�ó�� ��ü ������ �ʱ�ȭ
//	RenderTarget->ClearColor = FLinearColor(0.f, 1.f, 0.f, 1.f);
//	RenderTarget->UpdateResource();
//
//	//���� �ϳ��� �������ϱ� ���� ������ �������
//	UWorld* OffscreenWorld = GetWorld();
//	check(OffscreenWorld);
//	
//	//OffscreenWorld->InitWorld();
//	//���忡 Directional Light �߰�
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
//	ViewInfo.AspectRatio = 1.0f; //��Ⱦ��
//	ViewInfo.FOV = 90.0f; //�����þ߰�
//	
//
//	SceneCaptureComponent->SetCameraView(ViewInfo);
//
//	//ī�޶��� ��ġ,�Ӽ� ����
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
