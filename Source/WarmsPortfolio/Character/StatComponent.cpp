// Fill out your copyright notice in the Description page of Project Settings.


#include "StatComponent.h"

// Sets default values for this component's properties
UStatComponent::UStatComponent() : m_MaxHp(100.f),m_Hp(m_MaxHp)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	// ...
}


// Called when the game starts
void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UStatComponent::InitializeComponent()
{
	Super::InitializeComponent();

}


// Called every frame
void UStatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

int UStatComponent::GetHp()
{
	return m_Hp;
}

float UStatComponent::GetHpRatio()
{
	return m_Hp / m_MaxHp;
}

float UStatComponent::GetMaxHp()
{
	return m_MaxHp;
}

void UStatComponent::SetHp(float Hp)
{
	//¹Ý¿Ã¸²
	m_Hp = roundf(Hp);
	
	if (m_Hp < 0)
		m_Hp = 0;

	HpChanged.Broadcast();
}
