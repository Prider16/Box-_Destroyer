
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Variables)
	bool Firing = false;

	UFUNCTION(BlueprintCallable)
	void Fire();

	UFUNCTION(BlueprintCallable)
	void Reload();

private:
	void PerformLineTrace();

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float MaxRange = 20000.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmo = 30;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	int32 CurrentAmmo;
};
