// Fill out your copyright notice in the Description page of Project Settings.


#include "GraveActor.h"

// Sets default values
AGraveActor::AGraveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	//묘비 메테리얼을 찾음
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> Mat(TEXT("Material'/Game/Mesh/Mat_Grave.Mat_Grave'"));
	check(Mat.Succeeded());
	GraveMaterial = Mat.Object;

	//묘비 메쉬를 찾아서 설정
	GraveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Mesh(TEXT("StaticMesh'/Game/Mesh/TombStone.TombStone'"));
	check(Mesh.Succeeded());
	GraveMesh->SetStaticMesh(Mesh.Object);

}

// Called when the game starts or when spawned
void AGraveActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AGraveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGraveActor::SetMeshColor(FColor Color)
{
	UMaterialInstanceDynamic* Material =
	UMaterialInstanceDynamic::Create(GraveMaterial, this);
	Material->SetVectorParameterValue(ColorParamName, Color);
	GraveMesh->SetMaterial(0, Material);
}

