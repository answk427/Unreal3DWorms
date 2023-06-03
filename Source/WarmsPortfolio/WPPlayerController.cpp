// Fill out your copyright notice in the Description page of Project Settings.


#include "WPPlayerController.h"

void AWPPlayerController::SetupInputComponent() 
{
	Super::SetupInputComponent();
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