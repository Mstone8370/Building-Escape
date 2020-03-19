// Copyright test.


#include "RotatorTest.h"

// Sets default values for this component's properties
URotatorTest::URotatorTest()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URotatorTest::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URotatorTest::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	FRotator Rotation = GetOwner()->GetActorRotation();
	UE_LOG(LogTemp, Warning, TEXT("%f"), Rotation.Yaw);

	Rotation.Yaw += val;
	GetOwner()->SetActorRotation(Rotation);
}

