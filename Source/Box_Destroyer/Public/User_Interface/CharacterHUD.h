// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUD.generated.h"

class ABaseWeapon;

UCLASS()
class BOX_DESTROYER_API UCharacterHUD : public UUserWidget
{
	GENERATED_BODY()


public:
	// Called to set the score and ammo
	UFUNCTION(BlueprintCallable, Category = "UI")
	void GetValues();

	// Called to set the rifle refrence
	UFUNCTION()
	void setRifleRefrence(ABaseWeapon* Weapon);

protected:
	// Called every frame
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Variable for Score
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentScore;

	// Variable for Ammo
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentAmmo;

	// Function for Score
	UFUNCTION(BlueprintPure, Category = "UI")
	FText GetScoreText() const;

	// Function for Ammo
	UFUNCTION(BlueprintPure, Category = "UI")
	FText GetAmmoText() const;

private:

	// Rifle Variable for Furthus use
	UPROPERTY()
	ABaseWeapon* RifleRef;
};