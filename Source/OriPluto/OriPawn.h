// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OriPawn.generated.h"
UENUM(BlueprintType)
enum class EBorder : uint8
{
	Left 	UMETA(DisplayName = "Left"),
	Right 	UMETA(DisplayName = "Right"),
	Top UMETA(DisplayName = "Top"),
	Down UMETA(DisplayName = "Down"),
};
UENUM(BlueprintType)
enum class EPhysicsStatus : uint8
{
	InSkyAfterWall UMETA(DisplayName = "InSkyAfterWall"),
	InSky 	UMETA(DisplayName = "InSky"),
	OnGround 	UMETA(DisplayName = "OnGround"),
	OnWall UMETA(DisplayName = "OnWall"),
	Falling UMETA(DisplayName = "Falling"),
	Gliding UMETA(DisplayName = "Gliding"),
	SkyDash UMETA(DisplayName = "SkyDash"),
};
USTRUCT(BlueprintType)
struct FExternalAcceleration {
	GENERATED_BODY()
public:
	FExternalAcceleration() {}
	FExternalAcceleration(FVector i_Acceleration, bool i_IsActivated)
	{
		Acceleration = i_Acceleration;
		IsActivated = i_IsActivated;
	}
	FVector Acceleration;
	bool IsActivated;
	//friend bool operator==(const FExternalAcceleration& a, FExternalAcceleration& b)
	//{
	//	return a.
	//}
};

USTRUCT(BlueprintType)
struct FExternalAccelerationDuration {
	GENERATED_BODY()
public:
	FExternalAccelerationDuration() {}
	FExternalAccelerationDuration(FVector i_Acceleration, float i_Duration)
	{
		Acceleration = i_Acceleration;
		Duration = i_Duration;
	}
	FVector Acceleration;
	float Duration;
};

UCLASS()
class ORIPLUTO_API AOriPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AOriPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector GravityAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector JumpAcceleration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector InitialJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector InitialDoubleJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector BoundsExtent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector DoubleJumpMomentum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DoubleJumpDuration;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector InitialClimbingJumpSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float SkyDashMagnitude;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector ClimbJumpMomentum;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ClimbJumpDuration;
	bool isPause;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Position;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FVector Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isJump;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool isDoubleJump;

	bool HasGravity;
	bool isOnGround;
	float SkyDashTime;
	FVector SkyDashVelocity;
	FVector SkyDashAccelration;

	TMap<FString, FExternalAcceleration> ExternalAccelerations;
	TArray<FExternalAccelerationDuration> ExternalAccelerationsDuration;
	void UpdateStatus();
	void UpdateSpeed(float DeltaTime);
	void UpdatePosition(float DeltaTime);
	bool PositionDirectionMovement(FVector Movement, FVector PositionOffset);

	TMap< EBorder, TSet<FString>> BorderActorStatus;
#pragma region ObjectStatus
	EPhysicsStatus PreviousObjectStatus;
	EPhysicsStatus ObjectStatus;
	int JumpTimes;
#pragma endregion

#pragma region PlayerOperation
	UFUNCTION(BlueprintCallable)
		void Pause();
	UFUNCTION(BlueprintCallable)
		void Continue();

	UFUNCTION(BlueprintCallable)
		void JumpStart();
	UFUNCTION(BlueprintCallable)
		void JumpEnd();
	UFUNCTION(BlueprintCallable)
		void Bash();
	UFUNCTION(BlueprintCallable)
		AActor* ObjectDashBegin(TArray<AActor*> actors);
	UFUNCTION(BlueprintCallable)
		void ObjectDash(FVector Direction);
	UFUNCTION(BlueprintCallable)
		bool IsOnGround();
	UFUNCTION(BlueprintCallable)
		void SetPhyscisStatus(EPhysicsStatus status);

	UFUNCTION(BlueprintCallable)
		void UpdateBorderStatus(EBorder border, FString ActorName, bool status);
#pragma endregion
};
