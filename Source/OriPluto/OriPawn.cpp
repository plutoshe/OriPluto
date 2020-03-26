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
	BorderStatus.Empty();
	BorderStatus.Add(EBorder::Left, false);
	BorderStatus.Add(EBorder::Right, false);
	BorderStatus.Add(EBorder::Top, false);
	BorderStatus.Add(EBorder::Down, false);
	
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
	PreviousObjectStatus = ObjectStatus;
	ObjectStatus = EPhysicsStatus::SkyDash;
	SkyDashTime = 0.1f;
	Direction.Normalize();
	SkyDashVelocity = Direction * SkyDashMagnitude;
	SkyDashAccelration = (SkyDashVelocity - SkyDashVelocity * 0.2f) / SkyDashTime;
	
}

void AOriPawn::JumpStart()
{
	switch (ObjectStatus)
	{
	case EPhysicsStatus::InSky:
		ExternalAccelerationsDuration.Add(FExternalAccelerationDuration(DoubleJumpMomentum, DoubleJumpDuration));
		Speed.Z = InitialDoubleJumpSpeed.Z;
		SetPhyscisStatus(EPhysicsStatus::Falling);
		break;
	case EPhysicsStatus::OnGround:
		Speed.Z = InitialJumpSpeed.Z;
		ExternalAccelerations["NormalJump"].IsActivated = true;
		break;
	case EPhysicsStatus::OnWall:
		Speed.Z = InitialClimbingJumpSpeed.Z;
		break;
	case EPhysicsStatus::Gliding:
		break;
	case EPhysicsStatus::SkyDash:
		break;
	default:
		break;
	}
	JumpTimes++;
}

void AOriPawn::JumpEnd()
{
	switch (ObjectStatus)
	{
	case EPhysicsStatus::InSky:
		ExternalAccelerations["NormalJump"].IsActivated = false;
		break;
	case EPhysicsStatus::OnGround:
		break;
	case EPhysicsStatus::OnWall:
		break;
	case EPhysicsStatus::Gliding:
		break;
	case EPhysicsStatus::SkyDash:
		break;
	default:
		break;
	}
	
}

void AOriPawn::SetPhyscisStatus(EPhysicsStatus status)
{
	PreviousObjectStatus = ObjectStatus;
	ObjectStatus = status;
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
	//if (Speed.Z > 0 && BorderStatus[EBorder::Top])
	//{
	//	Speed.Z = 0;
	//}
	//if (Speed.Z < 0 && BorderStatus[EBorder::Down])
	//{
	//	Speed.Z = 0;
	//}
	//if (Speed.Y < 0 && BorderStatus[EBorder::Left])
	//{
	//	Speed.Y = 0;
	//}
	//if (Speed.Y > 0 && BorderStatus[EBorder::Right])
	//{
	//	Speed.Y = 0;
	//}
	if (PositionDirectionMovement(ZForwardVector * Speed.Z * DeltaTime,
		ZForwardVector * (Speed.Z > 0)* BoundsExtent.Z - ZForwardVector * (Speed.Z < 0) * BoundsExtent.Z))

	{
		isOnGround = true;
		isJump = false;
		isDoubleJump = false;
		Speed.Z = 0;
	}
	if (PositionDirectionMovement(YForwardVector * Speed.Y * DeltaTime,
		YForwardVector * (Speed.Y > 0)* BoundsExtent.Y - YForwardVector * (Speed.Y < 0) * BoundsExtent.Y))
	{
		Speed.Y = 0;
	}
	//Position += DeltaTime * Speed;
	SetActorLocation(Position);
}

void AOriPawn::UpdateStatus()
{
	
	if (BorderStatus[EBorder::Left] || BorderStatus[EBorder::Right])
	{
		SetPhyscisStatus(EPhysicsStatus::OnWall);
	}
	else if(BorderStatus[EBorder::Down])
	{
		SetPhyscisStatus(EPhysicsStatus::OnGround);		
	}
	else if (ObjectStatus == EPhysicsStatus::OnGround || ObjectStatus == EPhysicsStatus::OnWall)
	{
		SetPhyscisStatus(EPhysicsStatus::InSky);
	}
}

// Called every frame
void AOriPawn::Tick(float DeltaTime)
{
	if (!isPause)
	{
		Super::Tick(DeltaTime);
		UpdateStatus();
		switch (ObjectStatus)
		{
		case EPhysicsStatus::Falling:
			UpdateSpeed(DeltaTime);
			break;
		case EPhysicsStatus::InSky:
			UpdateSpeed(DeltaTime);
			break;
		case EPhysicsStatus::OnGround:
			UpdateSpeed(DeltaTime);
			break;
		case EPhysicsStatus::OnWall:
			UpdateSpeed(DeltaTime);
			break;
		case EPhysicsStatus::Gliding:
			UpdateSpeed(DeltaTime);
			break;
		case EPhysicsStatus::SkyDash:
		{
			SkyDashTime -= DeltaTime;
			float dashTime = FMath::Min(SkyDashTime, DeltaTime);
			Speed = SkyDashVelocity;
			SkyDashVelocity -= SkyDashAccelration * dashTime;
			if (SkyDashTime < 0)
			{
				ObjectStatus = PreviousObjectStatus;
			}
			break;
		}
		default:
			break;
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

void AOriPawn::SetBorderStatus(EBorder border, bool status)
{
	BorderStatus[border] = status;
}