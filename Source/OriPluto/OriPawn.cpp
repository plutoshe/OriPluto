// Fill out your copyright notice in the Description page of Project Settings.


#include "OriPawn.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/ShapeComponent.h"

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
	auto components = GetComponentsByTag(UShapeComponent::StaticClass(), FName(TEXT("bound")));
	//for (auto component : components)
	//{
	//	auto shape = Cast<UShapeComponent>(component);
	//	BoundsExtent = FMath::Max(BoundsExtent, shape->Bounds.BoxExtent);
	//}
	isPause = false;
	isOnGround = false;
	HasGravity = true;
	Position = GetActorLocation();
	ExternalAccelerations.Empty();
	ExternalAccelerations.Add("Gravity", FExternalAcceleration(GravityAcceleration, false));
	ExternalAccelerations.Add("NormalJump", FExternalAcceleration(JumpAcceleration, false));
}

bool AOriPawn::PositionDirectionMovement(FVector Movement, FVector PositionOffset)
{
	FHitResult OutHit;
	FVector End = Position + PositionOffset + Movement;
	FCollisionQueryParams CollisionParams(FName(TEXT("TraceUsableActor")), true, this);
	//DrawDebugLine(GetWorld(), Position, End, FColor::Green, false, 1, 0, 5);

	auto world = GetWorld();

	if (world && world->LineTraceSingleByChannel(OutHit, Position, End,
		ECC_GameTraceChannel1, CollisionParams))
	{
		Position += (OutHit.Distance - FVector::Distance(Position + PositionOffset, Position)) / FVector::Distance(Position + PositionOffset, End) * Movement;
		return true;
	}
	else
	{
		Position += Movement;
	}
	return false;
}

void AOriPawn::DoubleJump()
{
	isDoubleJump = true;
	ExternalAccelerationsDuration.Add(FExternalAccelerationDuration(DoubleJumpMomentum, DoubleJumpDuration));
	Speed += InitialDoubleJumpSpeed;
}

bool AOriPawn::IsOnGround()
{
	return isOnGround;
}

void AOriPawn::Bash()
{

}
AActor* AOriPawn::ObjectDashBegin(TArray<AActor*> actors)
{
	AActor* ChooseActor = nullptr;
	if (actors.Num() > 0)
	{
		ChooseActor = actors[0];
		for (auto obj : actors)
		{
			auto actor = Cast< AActor>(obj);

			auto dist = FVector::Distance(Position, actor->GetActorLocation());
			if (dist < FVector::Distance(Position, ChooseActor->GetActorLocation()))
			{
				ChooseActor = actor;
			}
		}
	}
	return ChooseActor;
}

void AOriPawn::ObjectDash(FVector Direction)
{
	isSkyDash = true;
	SkyDashTime = 0.1f;
	Direction.Normalize();
	SkyDashVelocity = Direction * 30;
	SkyDashAccelration = (SkyDashVelocity - SkyDashVelocity * 0.2f) / SkyDashTime;
	
}

void AOriPawn::NormalJump()
{	
	isJump = true;
	isOnGround = false;
	Speed += InitialJumpSpeed;
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
		Speed += GravityAcceleration * DeltaTime;
	}
}

void AOriPawn::UpdatePosition(float DeltaTime)
{
	FVector ZForwardVector(0, 0, 1);
	FVector YForwardVector(0, 1, 0);

	if (PositionDirectionMovement(ZForwardVector * Speed.Z,
		ZForwardVector * (Speed.Z > 0)* BoundsExtent.Z - ZForwardVector * (Speed.Z < 0) * BoundsExtent.Z))
	{
		isOnGround = true;
		isJump = false;
		isDoubleJump = false;
		Speed.Z = 0;
	}
	if (PositionDirectionMovement(YForwardVector * Speed.Y,
		YForwardVector * (Speed.Y > 0)* BoundsExtent.Y - YForwardVector * (Speed.Y < 0) * BoundsExtent.Y))
	{
		Speed.Y = 0;
	}
	SetActorLocation(Position);
}

// Called every frame
void AOriPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!isPause)
	{
		if (!isSkyDash)
		{
			UpdateSpeed(DeltaTime);
		}
		else
		{
			SkyDashTime -= DeltaTime;
			auto dashTime = FMath::Min(SkyDashTime, DeltaTime);
			Speed = SkyDashVelocity;
			SkyDashVelocity -= SkyDashAccelration * dashTime;
			if (SkyDashTime < 0)
			{
				isSkyDash = false;
			}
		}
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
