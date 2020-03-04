// Copyright test.


#include "OpenDoor.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
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

	// ...
	InitialYaw = this->GetOwner()->GetActorRotation().Yaw;
	CurrentYaw = InitialYaw;
	TargetYaw += InitialYaw;

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
			UE_LOG(LogTemp, Warning, TEXT("Enter"));
			OpenDoor(DeltaTime);
		} else {
			CloseDoor(DeltaTime);
		}
	}
}

void UOpenDoor::OpenDoor(float DeltaTime) {
	CurrentYaw = FMath::Lerp(CurrentYaw, TargetYaw, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = this->GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	this->GetOwner()->SetActorRotation(DoorRotation);
}

void UOpenDoor::CloseDoor(float DeltaTime) {
	CurrentYaw = FMath::Lerp(CurrentYaw, InitialYaw, DeltaTime * DoorOpenSpeed);
	FRotator DoorRotation = this->GetOwner()->GetActorRotation();
	DoorRotation.Yaw = CurrentYaw;
	this->GetOwner()->SetActorRotation(DoorRotation);
}
