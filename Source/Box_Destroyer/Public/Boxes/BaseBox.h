// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseBox.generated.h"

class UStaticMeshComponent;

UCLASS()
class BOX_DESTROYER_API ABaseBox : public AActor
{
	GENERATED_BODY()

	// Static Mesh Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BoxStaticMesh;

	// Variable for Score
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = "true"))
	int32 Score;

	// Variable for Health
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Variable, meta = (AllowPrivateAccess = "true"))
	float Health;

public:
	// Sets default values for this actor's properties
	ABaseBox();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Setter Getter for Score
	void ScoreSetter(int32 value);
	int32 ScoreGetter();

	// Setter Getter for health
	void HealthSetter(float value);
	float HealthGetter();

	// Function to reduce health
	void DamageBox(float Damage);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

};