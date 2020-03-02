// Copyright test.


#include "OpenDoor.h"
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

	// FRotator CurrentRotation = this->GetOwner()->GetActorRotation();

	// CurrentRotation.Yaw = -90.f;

	// this->GetOwner()->SetActorRotation(CurrentRotation);
}


// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	UE_LOG(LogTemp, Warning, TEXT("%s"), *this->GetOwner()->GetActorRotation().ToString());
	UE_LOG(LogTemp, Warning, TEXT("Yaw is: %f"), this->GetOwner()->GetActorRotation().Yaw);

	float CurrentYaw = this->GetOwner()->GetActorRotation().Yaw;
	// FRotator OpenDoor(0.f, TargetYaw, 0.f);
	FRotator OpenDoor(0.f, 0.f, 0.f);
	// OpenDoor.Yaw = FMath::Lerp(CurrentYaw, TargetYaw, 0.02f);
	// OpenDoor.Yaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, 45);
	OpenDoor.Yaw = FMath::FInterpTo(CurrentYaw, TargetYaw, DeltaTime, 1);
	this->GetOwner()->SetActorRotation(OpenDoor);
}

