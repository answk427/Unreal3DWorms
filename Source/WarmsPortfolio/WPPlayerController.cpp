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