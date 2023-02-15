// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureHelper.h"
#include <Camera/CameraComponent.h>
#include "Weapons/Weapon.h"

#include <Kismet/GameplayStatics.h>
#include "Engine/DataTable.h"

#include "Engine/TextureRenderTarget2D.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Image.h"
UCaptureHelper* UCaptureHelper::Instance()
{
	static UCaptureHelper* instance = NewObject<UCaptureHelper>();
	return instance;
}


void UCaptureHelper::DrawActorToTexture(UClass* ActorClass, UTextureRenderTarget2D* RenderTarget,
	uint32 sizeX, uint32 sizeY)
{

	check(RenderTarget);
	RenderTarget->InitAutoFormat(sizeX, sizeY);
	RenderTarget->UpdateResource();

	//���� �ϳ��� �������ϱ� ���� ������ �������
	UWorld* OffscreenWorld = UWorld::CreateWorld(EWorldType::GamePreview, true);

	const FTransform ActorTr(FRotator(0.0f, 0.0f, 0.0f), FVector(0.0f, 0.0f, 0.0f));
	AActor* SpawnedActor = OffscreenWorld->SpawnActor<AActor>(ActorClass, ActorTr);

	// Create the SceneCaptureComponent
	USceneCaptureComponent2D* SceneCaptureComponent = NewObject<USceneCaptureComponent2D>(SpawnedActor);

	//FMinimalViewInfo ViewInfo;
	//ViewInfo.AspectRatio = 1.0f; //��Ⱦ��
	//ViewInfo.FOV = 90.0f; //�����þ߰�
	//SceneCaptureComponent->SetCameraView(ViewInfo);

	SceneCaptureComponent->SetWorldLocationAndRotation(FVector(0, -100, 0), FRotator(0, 0, 0));
	SceneCaptureComponent->ProjectionType = ECameraProjectionMode::Perspective;
	SceneCaptureComponent->TextureTarget = RenderTarget;
	SceneCaptureComponent->CaptureSource = SCS_SceneColorHDR;

	//�ؽ�ó�� ��ü ������ �ʱ�ȭ
	RenderTarget->ClearColor = FLinearColor(1.f, 1.f, 1.f, 1.f);
	
	SceneCaptureComponent->CaptureScene();
		   
	
	//�ӽ÷� �����ߴ� ���� �ı�
	OffscreenWorld->DestroyWorld(false);

	
}
