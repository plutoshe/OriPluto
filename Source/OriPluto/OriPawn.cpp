// Fill out your copyright notice in the Description page of Project Settings.


#include "OriPawn.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"

// Sets default values
AOriPawn::AOriPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AOriPawn::BeginPlay()
{
	Super::BeginPlay();
	isPause = false;
	Position = GetActorLocation();
	ExternalAccelerations.Empty();
	ExternalAccelerations.Add("Gravity", FExternalAcceleration(GravityAcceleration, false));
	ExternalAccelerations.Add("NormalJump", FExternalAcceleration(JumpAcceleration, false));
}

void AOriPawn::PositionDirectionMovement(FVector Movement)
{
	FHitResult OutHit;
	FVector End = Position + Movement;
	FCollisionQueryParams CollisionParams(FName(TEXT("TraceUsableActor")), true, this);
	DrawDebugLine(GetWorld(), Position, End, FColor::Green, false, 1, 0, 5);
	
	auto world = GetWorld();

	if (world && world->LineTraceSingleByChannel(OutHit, Position, End,
		ECC_GameTraceChannel1, CollisionParams))
	{
		Position += OutHit.Distance / FVector::Distance(Position, End) * Movement;
		/*GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Green, FString::Printf(TEXT("The Component Being Hit is: %s"), *OutHit.GetComponent()->GetName()));*/
	}
	else
	{
		Position += Movement;
	}
}

void AOriPawn::DoubleJump()
{
	ExternalAccelerationsDuration.Add(FExternalAccelerationDuration(DoubleJumpMomentum, DoubleJumpDuration));
}

bool AOriPawn::IsOnGround()
{
	return false;
}

void AOriPawn::Bash()
{

}
void AOriPawn::ObjectBash(FVector ObjectPosition)
{

}

void AOriPawn::NormalJump()
{
	ExternalAccelerations["NormalJump"].IsActivated = true;
}
void AOriPawn::NormalJumpStop()
{
	ExternalAccelerations["NormalJump"].IsActivated = false;
}

void AOriPawn::UpdateSpeed(float DeltaTime)
{
	for (auto acceleration : ExternalAccelerations)
	{
		if (acceleration.Value.IsActivated)
		{

			Speed += acceleration.Value.Acceleration * DeltaTime;
		}
	}
	for (auto i = ExternalAccelerationsDuration.Num() - 1; i>=0; i--)
	{ 
		auto acceleration = ExternalAccelerationsDuration[i];
		Speed += acceleration.Acceleration * FMath::Min(DeltaTime, acceleration.Duration);
		acceleration.Duration -= DeltaTime;
		if (acceleration.Duration < 0)
		{
			ExternalAccelerationsDuration.RemoveAt(i);
		}
	}
	
	if (HasGravity)
	{
		Speed += GravityAcceleration;
	}
}

void AOriPawn::UpdatePosition(float DeltaTime)
{
	FVector ZForwardVector(0, 0, 1);
	FVector YForwardVector(0, 1, 0);
	PositionDirectionMovement(ZForwardVector * Speed.Z);
	PositionDirectionMovement(YForwardVector * Speed.Y);
	SetActorLocation(Position);
}

// Called every frame
void AOriPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isPause)
	{
		UpdateSpeed(DeltaTime);
		UpdatePosition(DeltaTime);
	}
}

// Called to bind functionality to input
void AOriPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AOriPawn::Pause()
{
	isPause = true;
}

void AOriPawn::Continue()
{
	isPause = false;
}
