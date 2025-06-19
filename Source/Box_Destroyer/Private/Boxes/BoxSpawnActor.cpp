// Fill out your copyright notice in the Description page of Project Settings.

#include "Boxes/BoxSpawnActor.h"
#include "Engine/World.h"
#include "Json.h"
#include "JsonUtilities.h"

ABoxSpawnActor::ABoxSpawnActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABoxSpawnActor::BeginPlay()
{
	Super::BeginPlay();
	FetchBoxData();
}

void ABoxSpawnActor::FetchBoxData()
{
	FHttpModule* Http = &FHttpModule::Get();
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	Request->SetURL(TEXT("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json"));
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->OnProcessRequestComplete().BindUObject(this, &ABoxSpawnActor::OnResponseReceived);
	Request->ProcessRequest();
}

void ABoxSpawnActor::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] HTTP Request Failed."));
		return;
	}

	int32 Code = Response->GetResponseCode();
	if (Code < 200 || Code >= 300)
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] HTTP Response returned error code: %d"), Code);
		return;
	}

	if (!ParseJson(Response->GetContentAsString()))
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] JSON Parsing Failed."));
		return;
	}

	SpawnAllBoxes();
}

bool ABoxSpawnActor::ParseJson(const FString& JsonString)
{
	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	if (!FJsonSerializer::Deserialize(Reader, RootObject) || !RootObject.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] Failed to deserialize JSON string."));
		return false;
	}

	// Parse types
	const TArray<TSharedPtr<FJsonValue>>* TypesArray;
	if (!RootObject->TryGetArrayField("types", TypesArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] 'types' array is missing in JSON."));
		return false;
	}

	for (const TSharedPtr<FJsonValue>& Entry : *TypesArray)
	{
		const TSharedPtr<FJsonObject>& TypeObject = Entry->AsObject();
		if (!TypeObject.IsValid()) continue;

		FBoxType BoxType;
		bool bColorValid = false;

		BoxType.Name = TypeObject->GetStringField("name");
		BoxType.Color = ConvertColorArray(TypeObject->GetArrayField("color"), bColorValid);
		BoxType.Health = TypeObject->GetNumberField("health");
		BoxType.Score = TypeObject->GetIntegerField("score");

		if (!bColorValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Invalid color for type: %s"), *BoxType.Name);
			continue;
		}

		BoxTypeMap.Add(BoxType.Name, BoxType);
	}

	// Parse objects
	const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;
	if (!RootObject->TryGetArrayField("objects", ObjectsArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] 'objects' array is missing in JSON."));
		return false;
	}

	for (const TSharedPtr<FJsonValue>& Entry : *ObjectsArray)
	{
		const TSharedPtr<FJsonObject>& Object = Entry->AsObject();
		if (!Object.IsValid()) continue;

		FBoxObject BoxObject;
		bool bTransformValid = false;

		BoxObject.Type = Object->GetStringField("type");
		BoxObject.Transform = ConvertJsonToTransform(Object->GetObjectField("transform"), bTransformValid);

		if (!BoxTypeMap.Contains(BoxObject.Type))
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Object type not found in types list: %s"), *BoxObject.Type);
			continue;
		}

		if (!bTransformValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Invalid transform for object of type: %s"), *BoxObject.Type);
			continue;
		}

		BoxObjects.Add(BoxObject);
	}

	return true;
}

FLinearColor ABoxSpawnActor::ConvertColorArray(const TArray<TSharedPtr<FJsonValue>>& ColorArray, bool& bSuccess)
{
	bSuccess = ColorArray.Num() == 3;
	if (!bSuccess) return FLinearColor::White;

	return FLinearColor(
		ColorArray[0]->AsNumber() / 255.0f,
		ColorArray[1]->AsNumber() / 255.0f,
		ColorArray[2]->AsNumber() / 255.0f,
		1.0f
	);
}

FTransform ABoxSpawnActor::ConvertJsonToTransform(const TSharedPtr<FJsonObject>& TransformJson, bool& bSuccess)
{
	bSuccess = false;

	if (!TransformJson.IsValid()) return FTransform();

	TArray<TSharedPtr<FJsonValue>> Loc = TransformJson->GetArrayField("location");
	TArray<TSharedPtr<FJsonValue>> Rot = TransformJson->GetArrayField("rotation");
	TArray<TSharedPtr<FJsonValue>> Scale = TransformJson->GetArrayField("scale");

	if (Loc.Num() != 3 || Rot.Num() != 3 || Scale.Num() != 3) return FTransform();

	FVector Location(Loc[0]->AsNumber(), Loc[1]->AsNumber(), Loc[2]->AsNumber());
	FRotator Rotation(Rot[1]->AsNumber(), Rot[2]->AsNumber(), Rot[0]->AsNumber()); // Pitch, Yaw, Roll
	FVector Scale3D(Scale[0]->AsNumber(), Scale[1]->AsNumber(), Scale[2]->AsNumber());

	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(FQuat(Rotation));
	Transform.SetScale3D(Scale3D);

	bSuccess = true;
	return Transform;
}

void ABoxSpawnActor::SpawnAllBoxes()
{
	if (!BoxToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] BoxToSpawn class is not set in editor."));
		return;
	}

	for (const FBoxObject& Obj : BoxObjects)
	{
		const FBoxType* TypeData = BoxTypeMap.Find(Obj.Type);
		if (!TypeData) continue;

		AActor* SpawnedBox = GetWorld()->SpawnActor<AActor>(BoxToSpawn, Obj.Transform);
		UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] spawn box actor."));
		if (!SpawnedBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Failed to spawn box actor."));
			continue;
		}

		UStaticMeshComponent* MeshComp = SpawnedBox->FindComponentByClass<UStaticMeshComponent>();
		if (MeshComp)
		{
			UMaterialInstanceDynamic* DynMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
			if (DynMaterial)
			{
				DynMaterial->SetVectorParameterValue("Base Color", TypeData->Color);
			}
		}

	}
}
