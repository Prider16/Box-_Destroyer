// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Boxes/BaseBox.h"
#include "BoxSpawnActor.generated.h"

// Struct Function to store the data that we get from JSON
// For Details
USTRUCT()
struct FBoxType
{
	GENERATED_BODY()

	UPROPERTY()
	FString Name;

	UPROPERTY()
	FLinearColor Color;

	UPROPERTY()
	float Health;

	UPROPERTY()
	int32 Score;
};

// For transform
USTRUCT()
struct FBoxObject
{
	GENERATED_BODY()

	UPROPERTY()
	FString Type;

	UPROPERTY()
	FTransform Transform;
};

UCLASS()
class BOX_DESTROYER_API ABoxSpawnActor : public AActor
{
	GENERATED_BODY()

public:
	ABoxSpawnActor();

protected:
	virtual void BeginPlay() override;

private:
	// HTTP Functions
	void FetchBoxData();
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

	// JSON parsing functions
	bool ParseJson(const FString& JsonString);
	FLinearColor ConvertColorArray(const TArray<TSharedPtr<FJsonValue>>& ColorArray, bool& bSuccess);
	FTransform ConvertJsonToTransform(const TSharedPtr<FJsonObject>& TransformJson, bool& bSuccess);

	// Data Storage
	UPROPERTY()
	TMap<FString, FBoxType> BoxTypeMap;

	UPROPERTY()
	TArray<FBoxObject> BoxObjects;

	// Spawn Function
	void SpawnAllBoxes();

	// Blueprint box to spawn
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<ABaseBox> BoxToSpawn;
};