// Copyright Alexander Duffy 2021


#include "OpenDoor.h"
#include "GameFramework/PlayerController.h" 
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	//GetOwner()->GetActorRotation();
	ClosedRotation = GetOwner()->GetActorRotation();
	ClosedYaw =ClosedRotation.Yaw;
	OpenYaw += ClosedYaw;

	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("Actor '%s' has the open door component on it but has no pressure plate set"), *GetOwner()->GetName());
	}
	ActorThatOpens = GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PressurePlate && PressurePlate->IsOverlappingActor(ActorThatOpens))
	{
		OpenDoor(DeltaTime);
		DoorLastOpened = GetWorld()->GetTimeSeconds();
	}

	else if (GetOwner()->GetActorRotation() != ClosedRotation && GetWorld()->GetTimeSeconds() > DoorLastOpened + DoorCloseDelay)
	{	
			CloseDoor(DeltaTime);	
	}
}


void UOpenDoor::OpenDoor(float DeltaTime)
{
	float CurrentYaw = GetOwner()->GetActorRotation().Yaw;

	//UE_LOG(LogTemp, Warning, TEXT("%f"), CurrentYaw);

	CurrentYaw = FMath::FInterpTo(CurrentYaw, OpenYaw, DeltaTime, 5);

	FRotator TickRotation(ClosedRotation.Pitch, CurrentYaw, ClosedRotation.Roll);
	GetOwner()->SetActorRotation(TickRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime)
{
	float CurrentYaw = GetOwner()->GetActorRotation().Yaw;

	CurrentYaw = FMath::FInterpTo(CurrentYaw, ClosedYaw, DeltaTime, 9);

	FRotator TickRotation(ClosedRotation.Pitch, CurrentYaw, ClosedRotation.Roll);

	GetOwner()->SetActorRotation(TickRotation);
}