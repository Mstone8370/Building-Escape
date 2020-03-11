// Copyright test.

#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Grabber.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...
	// UE_LOG(LogTemp, Warning, TEXT("test"));
	PhysicsHandle = this->GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("PhysicsHandle found"));
	} else {
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle found on %s"), *GetOwner()->GetName());
	}

	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Input Component found"));
		InputComponent->BindAction(TEXT("Grab"), IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction(TEXT("Grab"), IE_Released, this, &UGrabber::Release);
	} else {
		UE_LOG(LogTemp, Error, TEXT("No Input Component found on %s"), *GetOwner()->GetName());
	}
}

void UGrabber::Grab() {
	UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));
}

void UGrabber::Release() {
	UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	this->GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	// UE_LOG(LogTemp, Warning, TEXT("Location: %s, Rotation: %s"), 
	// 	*PlayerViewPointLocation.ToString(),
	// 	*PlayerViewPointRotation.ToString()
	// );

	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;

	DrawDebugLine(
		this->GetWorld(),
		PlayerViewPointLocation,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.f,
		0,
		5.f
	);

	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this->GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		PlayerViewPointLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	AActor* ActorHit = Hit.GetActor();

	// if(ActorHit != nullptr) {
	// 	UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: %s"), *ActorHit->GetName());
	// } else {
	// 	UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: Nothing"));
	// }
	if(ActorHit != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: %s"), *ActorHit->GetName());
	}

}

