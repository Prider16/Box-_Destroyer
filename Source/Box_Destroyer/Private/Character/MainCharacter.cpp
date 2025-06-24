// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Weapon/BaseWeapon.h"
#include "Blueprint/UserWidget.h"
#include "User_Interface/CharacterHUD.h"


AMainCharacter::AMainCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create the first person mesh that will be viewed only by this character's owner
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));

	FirstPersonMesh->SetupAttachment(GetMesh());
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;
	FirstPersonMesh->SetCollisionProfileName(FName("NoCollision"));

	// Create the Camera Component	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("First Person Camera"));
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMesh, FName("head"));
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FVector(-2.8f, 5.89f, 0.0f), FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = 70.0f;
	FirstPersonCameraComponent->FirstPersonScale = 0.6f;

	// configure the character comps
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	GetCapsuleComponent()->SetCapsuleSize(34.0f, 96.0f);

	// Configure character movement
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->AirControl = 0.5f;

}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// check if we have a UserWidget or not
	if (CharacterHUDClass)
	{
		// if yes then Create a Widget
		CharacterHUD = CreateWidget<UCharacterHUD>(GetWorld(), CharacterHUDClass);
		if (CharacterHUD)
		{
			// if created Successfully then Add to viewport
			CharacterHUD->AddToViewport();
		}
	}

	// Spawning Rifle at the start
	AMainCharacter::SpawnRifle();
}

void AMainCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// For player Input
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);

		// Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMainCharacter::StartSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMainCharacter::StopSprint);

		// Shooting
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AMainCharacter::Shoot);

		// Reload
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMainCharacter::Reload);

	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AMainCharacter::StartSprint(const FInputActionValue& Value)
{
	// Changing speed for Sprinting
	isSprint = true;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
}

void AMainCharacter::StopSprint(const FInputActionValue& Value)
{
	// Setting Speed Back to normal after Sprint
	isSprint = false;
	GetCharacterMovement()->MaxWalkSpeed = 400.0f;
}

void AMainCharacter::Shoot(const FInputActionValue& Value)
{
	// Calling Fire Function from Rifle
	RifleRefrence->Fire();

}

void AMainCharacter::Reload(const FInputActionValue& Value)
{
	// Calling Reload Function from Rifle
	RifleRefrence->Reload();
}

void AMainCharacter::SpawnRifle()
{
	// if Rifle not present then return
	if (!RifleBlueprint) return;

	// getting the world
	UWorld* World = GetWorld();
	if (!World) return;

	// SpawnParams
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// Spawning the Rifle in the scene
	ABaseWeapon* SpawnedRifle = World->SpawnActor<ABaseWeapon>(RifleBlueprint, GetActorTransform(), SpawnParams);

	// Creating a Rifle Refrence for furthur use
	RifleRefrence = SpawnedRifle;

	CharacterHUD->setRifleRefrence(SpawnedRifle);

	// if the Rifle is spawned...
	if (SpawnedRifle)
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		//... Attaching it to the component
		SpawnedRifle->AttachToComponent(GetMesh(), AttachmentRules, TEXT("HandGrip_R"));
		SpawnedRifle->AttachToComponent(FirstPersonMesh, AttachmentRules, TEXT("HandGrip_R"));
	}
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

