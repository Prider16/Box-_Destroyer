// Fill out your copyright notice in the Description page of Project Settings.

#include "Boxes/BaseBox.h"

// Sets default values
ABaseBox::ABaseBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Creating a Static Mesh Component and making it RootComponent
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

// Setter Function for Score
void ABaseBox::ScoreSetter(int32 value)
{
	Score = value;
}

// Getter Function for Score
int32 ABaseBox::ScoreGetter()
{
	return Score;
}

// Setter Function for Health
void ABaseBox::HealthSetter(float value)
{
	Health = value;
}

// Getter Function for Health
float ABaseBox::HealthGetter()
{
	return Health;
}

// DamageBox Function
void ABaseBox::DamageBox(float Damage)
{
	// Redusing Health
	Health -= Damage;

	// if Health <= 0 then...
	if (Health <= 0.f)
	{
		// Destroying the Object.
		Destroy();
	}
}
