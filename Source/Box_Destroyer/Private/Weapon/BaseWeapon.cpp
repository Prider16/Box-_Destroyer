#include "Weapon/BaseWeapon.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/PlayerController.h"
#include "DrawDebugHelpers.h"

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

	UE_LOG(LogTemp, Log, TEXT("Current. Ammo: %d"), CurrentAmmo);

	// Call Linetrace Function
	ABaseWeapon::PerformLineTrace();
}

// Reload Function
void ABaseWeapon::Reload()
{
	// Set the current ammo = max ammo
	CurrentAmmo = MaxAmmo;
	UE_LOG(LogTemp, Log, TEXT("Reloaded. Ammo: %d"), CurrentAmmo);
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
			//Apply damage to hit Actor.
			UGameplayStatics::ApplyPointDamage(HitActor, Damage, Rotation.Vector(), Hit, PlayerController, this, nullptr);
			UE_LOG(LogTemp, Log, TEXT("Hit %s for %f damage"), *HitActor->GetName(), Damage);
		}
	}

	// Debug line
	DrawDebugLine(GetWorld(), Location, End, FColor::Red, false, 2.0f, 0, 1.0f);
}
