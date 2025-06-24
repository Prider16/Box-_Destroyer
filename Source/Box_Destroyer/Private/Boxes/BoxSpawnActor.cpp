// Fill out your copyright notice in the Description page of Project Settings.


#include "Boxes/BoxSpawnActor.h"
#include "Engine/World.h"
#include "Json.h"
#include "JsonUtilities.h"
#include "Boxes/BaseBox.h"

ABoxSpawnActor::ABoxSpawnActor()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ABoxSpawnActor::BeginPlay()
{
	Super::BeginPlay();

	// Function to Fetch data from JSON
	FetchBoxData();
}

// Called to Fetch data from JSON
void ABoxSpawnActor::FetchBoxData()
{
	// Get reference to the HTTP module
	FHttpModule* Http = &FHttpModule::Get();

	// Create a new HTTP Request
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> Request = Http->CreateRequest();

	// Set the target JSON URL
	Request->SetURL(TEXT("https://raw.githubusercontent.com/CyrusCHAU/Varadise-Technical-Test/refs/heads/main/data.json"));

	// Set request type and headers
	Request->SetVerb("GET");
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// Bind the response callback
	Request->OnProcessRequestComplete().BindUObject(this, &ABoxSpawnActor::OnResponseReceived);

	// Send the request
	Request->ProcessRequest();
}

// Function for when response was received
void ABoxSpawnActor::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	// if response is not valid
	if (!bWasSuccessful || !Response.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] HTTP Request Failed."));
		return;
	}

	// If HTTP response code is not success
	int32 Code = Response->GetResponseCode();
	if (Code < 200 || Code >= 300)
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] HTTP Response returned error code: %d"), Code);
		return;
	}

	// Try parsing JSON from the response
	if (!ParseJson(Response->GetContentAsString()))
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] JSON Parsing Failed."));
		return;
	}

	// Function to spawn all Boxes
	SpawnAllBoxes();
}

bool ABoxSpawnActor::ParseJson(const FString& JsonString)
{
	// Preparing JSON object
	TSharedPtr<FJsonObject> RootObject;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);

	// if JSON is unable to parse
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

	// for every element in JSON
	for (const TSharedPtr<FJsonValue>& Entry : *TypesArray)
	{
		const TSharedPtr<FJsonObject>& TypeObject = Entry->AsObject();
		if (!TypeObject.IsValid()) continue;

		FBoxType BoxType;
		bool bColorValid = false;

		// Getting Each Field of Box
		BoxType.Name = TypeObject->GetStringField("name");
		// Function call to get color array
		BoxType.Color = ConvertColorArray(TypeObject->GetArrayField("color"), bColorValid);
		BoxType.Health = TypeObject->GetNumberField("health");
		BoxType.Score = TypeObject->GetIntegerField("score");

		// if color is not valid
		if (!bColorValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Invalid color for type: %s"), *BoxType.Name);
			continue;
		}

		// Adding the Object
		BoxTypeMap.Add(BoxType.Name, BoxType);
	}

	// Parsing the object for transform
	const TArray<TSharedPtr<FJsonValue>>* ObjectsArray;

	// if Object is missing
	if (!RootObject->TryGetArrayField("objects", ObjectsArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] 'objects' array is missing in JSON."));
		return false;
	}

	// For every element in Object
	for (const TSharedPtr<FJsonValue>& Entry : *ObjectsArray)
	{
		const TSharedPtr<FJsonObject>& Object = Entry->AsObject();
		if (!Object.IsValid()) continue;

		FBoxObject BoxObject;
		bool bTransformValid = false;

		// getting the elements
		BoxObject.Type = Object->GetStringField("type");
		BoxObject.Transform = ConvertJsonToTransform(Object->GetObjectField("transform"), bTransformValid);

		// If we get a type that is not defined in Box Details.
		if (!BoxTypeMap.Contains(BoxObject.Type))
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Object type not found in types list: %s"), *BoxObject.Type);
			continue;
		}

		// If transform is not valid
		if (!bTransformValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Invalid transform for object of type: %s"), *BoxObject.Type);
			continue;
		}

		// Adding the BoxObject
		BoxObjects.Add(BoxObject);
	}

	return true;
}

// Function to Convert the color
FLinearColor ABoxSpawnActor::ConvertColorArray(const TArray<TSharedPtr<FJsonValue>>& ColorArray, bool& bSuccess)
{
	// check if the color has 3 Numbers or not
	bSuccess = ColorArray.Num() == 3;
	if (!bSuccess) return FLinearColor::White;

	// Returning the color Array in range of 0.0 - 1.0
	return FLinearColor(
		ColorArray[0]->AsNumber() / 255.0f,
		ColorArray[1]->AsNumber() / 255.0f,
		ColorArray[2]->AsNumber() / 255.0f,
		1.0f
	);
}

// Function to convert Transfornm from JSON
FTransform ABoxSpawnActor::ConvertJsonToTransform(const TSharedPtr<FJsonObject>& TransformJson, bool& bSuccess)
{
	bSuccess = false;

	// if Transform is not valid then return default
	if (!TransformJson.IsValid()) return FTransform();

	// Setting the Location. Rotation and Scale in array
	TArray<TSharedPtr<FJsonValue>> Loc = TransformJson->GetArrayField("location");
	TArray<TSharedPtr<FJsonValue>> Rot = TransformJson->GetArrayField("rotation");
	TArray<TSharedPtr<FJsonValue>> Scale = TransformJson->GetArrayField("scale");

	// if Any of them is not equal to 3 then return default
	if (Loc.Num() != 3 || Rot.Num() != 3 || Scale.Num() != 3) return FTransform();

	// Setting the Location. Rotation and Scale
	FVector Location(Loc[0]->AsNumber(), Loc[1]->AsNumber(), Loc[2]->AsNumber());
	FRotator Rotation(Rot[1]->AsNumber(), Rot[2]->AsNumber(), Rot[0]->AsNumber()); // Pitch, Yaw, Roll
	FVector Scale3D(Scale[0]->AsNumber(), Scale[1]->AsNumber(), Scale[2]->AsNumber());

	// Making a transform
	FTransform Transform;
	Transform.SetLocation(Location);
	Transform.SetRotation(FQuat(Rotation));
	Transform.SetScale3D(Scale3D);

	// Returning the transform
	bSuccess = true;
	return Transform;
}

// Function to spawn all Boxes
void ABoxSpawnActor::SpawnAllBoxes()
{
	// Check if the Box it set from the editor or not
	if (!BoxToSpawn)
	{
		UE_LOG(LogTemp, Error, TEXT("[BoxSpawner] BoxToSpawn class is not set in editor."));
		return;
	}

	// For each element Transform in BocObject
	for (const FBoxObject& Obj : BoxObjects)
	{
		// Finding Box of the respective type
		const FBoxType* TypeData = BoxTypeMap.Find(Obj.Type);
		if (!TypeData) continue;

		// Spawning the Box
		ABaseBox* SpawnedBox = GetWorld()->SpawnActor<ABaseBox>(BoxToSpawn, Obj.Transform);

		// if Box Spawning fail
		if (!SpawnedBox)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Failed to spawn box actor."));
			continue;
		}

		// Getting the Mesh Component from the Spawned Box
		UStaticMeshComponent* MeshComp = SpawnedBox->FindComponentByClass<UStaticMeshComponent>();
		if (MeshComp)
		{
			// Then Getting The Material From static Mesh
			UMaterialInstanceDynamic* DynMaterial = MeshComp->CreateAndSetMaterialInstanceDynamic(0);
			if (DynMaterial)
			{
				// Changing the Color of the Material to Box's color that we get from JSON
				DynMaterial->SetVectorParameterValue("Base Color", TypeData->Color);
			}
		}

		// getting the Score and Setting it in the box
		int32 boxscore = TypeData->Score;
		if (!boxscore)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Failed to get box actor Score."));
			continue;
		}
		else
		{
			SpawnedBox->ScoreSetter(boxscore);
		}

		// getting the Health and Setting it in the Box
		float Boxhealth = TypeData->Health;
		if (!Boxhealth)
		{
			UE_LOG(LogTemp, Warning, TEXT("[BoxSpawner] Failed to get box actor Health."));
			continue;
		}
		else
		{
			SpawnedBox->HealthSetter(Boxhealth);
		}
	}
}