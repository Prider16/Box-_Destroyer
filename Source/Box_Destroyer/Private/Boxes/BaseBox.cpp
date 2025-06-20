// Fill out your copyright notice in the Description page of Project Settings.


#include "Boxes/BaseBox.h"

// Sets default values
ABaseBox::ABaseBox()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoxStaticMesh"));
	RootComponent = BoxStaticMesh;

}

// Called when the game starts or when spawned
void ABaseBox::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseBox::ScoreSetter(int32 value)
{
	Score = value;
}

int32 ABaseBox::ScoreGetter()
{
	return Score;
}

void ABaseBox::HealthSetter(float value)
{
	Health = value;
}

float ABaseBox::HealthGetter()
{
	return Health;
}

void ABaseBox::DamageBox(float Damage)
{
	Health -= Damage;

	if (Health <= 0.f)
	{
		Destroy();
	}
}

