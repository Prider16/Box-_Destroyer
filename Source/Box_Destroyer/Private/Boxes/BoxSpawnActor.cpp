// Fill out your copyright notice in the Description page of Project Settings.


#include "Boxes/BoxSpawnActor.h"

// Sets default values
ABoxSpawnActor::ABoxSpawnActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABoxSpawnActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABoxSpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

