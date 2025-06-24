// Fill out your copyright notice in the Description page of Project Settings.


#include "User_Interface/CharacterHUD.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Character/MainCharacter.h"
#include "Weapon/BaseWeapon.h"

void UCharacterHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Update score and ammo if we have a valid weapon
	if (RifleRef)
	{
		CurrentScore = RifleRef->ScoreGetter();
		CurrentAmmo = RifleRef->AmmoGetter();
	}
}

// Function to Bind from blueprint for Score
FText UCharacterHUD::GetScoreText() const
{
	return FText::FromString(FString::Printf(TEXT("Score: %d"), CurrentScore));
}

// Function to Bind from blueprint for Ammo
FText UCharacterHUD::GetAmmoText() const
{
	return FText::FromString(FString::Printf(TEXT("% d"), CurrentAmmo));
}

// Function to get the initial Value of Score and Weapon
void UCharacterHUD::GetValues()
{
	if (RifleRef)
	{
		UE_LOG(LogTemp, Error, TEXT("RifleRef is not null"));
		CurrentScore = RifleRef->ScoreGetter();
		CurrentAmmo = RifleRef->AmmoGetter();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RifleRef is null"));
	}
}

// Setting up the Rifle Refrence
void UCharacterHUD::setRifleRefrence(ABaseWeapon* Weapon)
{
	RifleRef = Weapon;
	UE_LOG(LogTemp, Error, TEXT("Rifle is set"));
	GetValues();
}
