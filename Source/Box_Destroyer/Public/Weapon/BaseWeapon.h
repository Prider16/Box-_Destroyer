
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boxes/BaseBox.h"
#include "BaseWeapon.generated.h"

UCLASS()
class BOX_DESTROYER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

public:
	ABaseWeapon();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Firing Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
	bool Firing = false;

	// Fire Function for Shoot
	UFUNCTION(BlueprintCallable)
	void Fire();

	// Reload Function
	UFUNCTION(BlueprintCallable)
	void Reload();

	// Setter Getter for Score
	void ScoreSetter(int32 value);
	int32 ScoreGetter();

private:
	// Function to setup code for Line trace and to apply Damage
	void PerformLineTrace();

	// Max range of linetrace
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxRange = 20000.0f;

	// Damage that will be applied to other
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 1.0f;

	// Maximum Ammo
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmo = 30;

	// Current Ammo
	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(VisibleAnywhere, Category = "Variable")
	int32 Score = 0;

};
