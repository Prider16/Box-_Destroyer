// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"
#include "Boxes/BaseBox.h"

// Sets default values
ABaseWeapon::ABaseWeapon()
{
	PrimaryActorTick.bCanEverTick = true;
	CurrentAmmo = MaxAmmo;
}

// Called when the game starts or when spawned
void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Fire Function
void ABaseWeapon::Fire()
{
	Firing = true;
	if (CurrentAmmo <= 0)
	{
		// If we have 0 ammo then we can't shoot, We need to Reload
		UE_LOG(LogTemp, Warning, TEXT("Out of Ammo!"));
		return;
	}

	// Else Decreas 1 ammo
	CurrentAmmo--;

	// Call Linetrace Function
	ABaseWeapon::PerformLineTrace();
}

// Reload Function
void ABaseWeapon::Reload()
{
	// Set the current ammo = max ammo
	CurrentAmmo = MaxAmmo;
}

// Linetrace Function
void ABaseWeapon::PerformLineTrace()
{
	// Get the player Controller
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (!PlayerController) return;

	// Getting the player Viewpoint
	FVector Location;
	FRotator Rotation;
	PlayerController->GetPlayerViewPoint(Location, Rotation);

	FVector End = Location + Rotation.Vector() * MaxRange;

	FHitResult Hit;
	FCollisionQueryParams Params;
	// Ignore weapon
	Params.AddIgnoredActor(this);

	// Do linetrace and store in bool to check if we did hit or not
	bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Location, End, ECC_Visibility, Params);

	// we linetrace hit something...
	if (bHit)
	{
		// ... We get the hit actor
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			// We then check if the hitActor is a Box or not
			ABaseBox* HitBox = Cast<ABaseBox>(HitActor);
			if (HitBox)
			{
				// We then call the DamageBox function from the Box and Apply Damage
				HitBox->DamageBox(Damage);

				// If the health is 0 then...
				if (HitBox->HealthGetter() <= 0.f)
				{
					//... We get the Score.
					Score += HitBox->ScoreGetter();
				}
			}
			else
			{
				// else, print the hitActor name in the Log
				UE_LOG(LogTemp, Log, TEXT("Hit %s "), *HitActor->GetName());
			}
		}
	}

	// Debug line
	//DrawDebugLine(GetWorld(), Location, End, FColor::Red, false, 2.0f, 0, 1.0f);
}

// Setter for Score
void ABaseWeapon::ScoreSetter(int32 value)
{
	Score = value;
}

// Getter for Score
int32 ABaseWeapon::ScoreGetter()
{
	return Score;
}

// Setter for Ammo
void ABaseWeapon::AmmoSetter(int32 value)
{
	CurrentAmmo = value;
}

// Getter for Ammo
int32 ABaseWeapon::AmmoGetter()
{
	return CurrentAmmo;
}