// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "OriPawn.generated.h"
UENUM(BlueprintType)
enum class EPhysicsStatus : uint8
{
	InSky 	UMETA(DisplayName = "Exploration"),
	OnGround 	UMETA(DisplayName = "Building"),
	Climbing UMETA(DisplayName = "Exploration"),
	Gliding UMETA(DisplayName = "Exploration"),
	SkyDash UMETA(DisplayName = "Exploration"),
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
	EPhysicsStatus PreviousObjectStatus;
	EPhysicsStatus ObjectStatus;

	TMap<FString, FExternalAcceleration> ExternalAccelerations;
	TArray<FExternalAccelerationDuration> ExternalAccelerationsDuration;
	void UpdateSpeed(float DeltaTime);
	void UpdatePosition(float DeltaTime);
	bool PositionDirectionMovement(FVector Movement, FVector PositionOffset);

#pragma region PlayerOperation
	UFUNCTION(BlueprintCallable)
		void Pause();
	UFUNCTION(BlueprintCallable)
		void Continue();

	UFUNCTION(BlueprintCallable)
		void NormalJump();
	UFUNCTION(BlueprintCallable)
		void NormalJumpStop();
	UFUNCTION(BlueprintCallable)
		void DoubleJump();
	UFUNCTION(BlueprintCallable)
		void Bash();
	UFUNCTION(BlueprintCallable)
		AActor* ObjectDashBegin(TArray<AActor*> actors);
	UFUNCTION(BlueprintCallable)
		void ObjectDash(FVector Direction);
	UFUNCTION(BlueprintCallable)
		bool IsOnGround();
#pragma endregion
};
