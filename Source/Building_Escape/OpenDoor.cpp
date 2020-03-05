// Copyright test.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "Math/Color.h"

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

	// ...
	InitialAngle = this->GetOwner()->GetActorRotation().Yaw;
	CurrentAngle = InitialAngle;
	OpenAngle += InitialAngle;

	if(!PressurePlate) {
		UE_LOG(LogTemp, Error, TEXT("%s has the OpenDoor component on it, but no PressurePlate set."), *this->GetOwner()->GetName());
	}

	ActorThatOpens = this->GetWorld()->GetFirstPlayerController()->GetPawn();
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	// UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetOwner()->GetActorRotation().ToString());
	// UE_LOG(LogTemp, Warning, TEXT("Yaw is: %f"), this->GetOwner()->GetActorRotation().Yaw);


	if(PressurePlate != nullptr) {
		if(PressurePlate->IsOverlappingActor(ActorThatOpens)) {
			// UE_LOG(LogTemp, Warning, TEXT("Enter"));
			OpenDoor(DeltaTime);
			DoorLastOpened = this->GetWorld()->GetTimeSeconds();
		} else if((this->GetWorld()->GetTimeSeconds() - DoorLastOpened) > DoorCloseDelay) {
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime) {
	CurrentAngle = FMath::Lerp(CurrentAngle, OpenAngle, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = this->GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentAngle;
	this->GetOwner()->SetActorRotation(DoorRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime) {
	CurrentAngle = FMath::Lerp(CurrentAngle, InitialAngle, DeltaTime * DoorCloseSpeed);
	FRotator DoorRotation = this->GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentAngle;
	this->GetOwner()->SetActorRotation(DoorRotation);
}
