// Copyright Alexander Duffy 2021

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

#define OUTPARAM

// Sets default values for this component's properties
UGrabber::UGrabber()
{	
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	
	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	Input = GetOwner()->FindComponentByClass<UInputComponent>();
	if (Input)
	{
		Input->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		Input->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandle()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle) {/*Physics is found*/ }
	else
	{
		UE_LOG(LogTemp, Error, TEXT("The Actor '%s' has a grabber component but no Physics Handle component."), *GetOwner()->GetName());
	}

}

void UGrabber::Release()
{
	PhysicsHandle->ReleaseComponent();
}
void UGrabber::Grab()
{
	FHitResult Hit = GetFirstPhysicsBodyInReach();

	UPrimitiveComponent* ComponentToGrab = Hit.GetComponent();

	
	if (Hit.GetActor())
	{
		PhysicsHandle->GrabComponentAtLocation(ComponentToGrab, NAME_None, FindReachEnd());
	}

}

FVector UGrabber::FindReachEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewpointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUTPARAM PlayerViewPointLocation, OUTPARAM PlayerViewpointRotation);
	FVector LineTraceEnd = PlayerViewPointLocation + (PlayerViewpointRotation.Vector() * PlayerReach);
	return LineTraceEnd;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewpointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(OUTPARAM PlayerViewPointLocation, OUTPARAM PlayerViewpointRotation);

	FVector LineTraceEnd = PlayerViewPointLocation + (PlayerViewpointRotation.Vector() * PlayerReach);

	FHitResult Hit;

	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams);

	return Hit;
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(FindReachEnd());
	}

}