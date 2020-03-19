// Copyright test.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	FindPhysicsHandle();
	SetupInputComponent();
	PrevPlayerYaw = GetPlayersRot().Yaw;
}

void UGrabber::FindPhysicsHandle() {
	PhysicsHandle = this->GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if(PhysicsHandle == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No PhysicsHandle found on %s"), *GetOwner()->GetName());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("PhysicsHandle found"));
}

void UGrabber::SetupInputComponent() {
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if(InputComponent == nullptr) {
		UE_LOG(LogTemp, Error, TEXT("No Input Component found on %s"), *GetOwner()->GetName());
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Input Component found"));
	InputComponent->BindAction(TEXT("Grab"), IE_Pressed, this, &UGrabber::Grab);
	InputComponent->BindAction(TEXT("Grab"), IE_Released, this, &UGrabber::Release);
}

void UGrabber::Grab() {
	if(!PhysicsHandle) return;

	UE_LOG(LogTemp, Warning, TEXT("Grabber Pressed"));
	
	FHitResult HitResult = GetFirstPhysicsBodyInReach();
	UPrimitiveComponent* ComponentToGrab = HitResult.GetComponent();

	if(HitResult.GetActor() != nullptr) {
		HitResult.GetActor()->SetActorLocation(GetPlayersReach());

		/*
		 *	Offset이 90도인 경우 한바퀴 돌음. 만약 Offset이 90인 경우 GetOffsetYaw에서 0을 return 해야 함
		 *	GetOffsetYaw에서 90으로 나눈 값을 return하는 경우 다른 방향에서 바라볼 때 같은 현상 나타남.
		*/
		FRotator TempRotation = ComponentToGrab->GetOwner()->GetActorRotation();
		UE_LOG(LogTemp, Warning, TEXT("Original: %f, Player: %f"), TempRotation.Yaw, GetPlayersRot().Yaw);
		TempRotation.Yaw = GetOffsetYaw(GetPlayersRot().Yaw, TempRotation.Yaw);
		UE_LOG(LogTemp, Warning, TEXT("Offset: %f"), TempRotation.Yaw);
		HalfupRotation(TempRotation);
		UE_LOG(LogTemp, Warning, TEXT("After Halfup: %f"), TempRotation.Yaw);
		/* PlayerYaw와 같은 값을 저장함
		TempRotation.Yaw -= (int32)(TempRotation.Yaw / 90.f) * 90.f;
		UE_LOG(LogTemp, Warning, TEXT("divide as 90: %f"), TempRotation.Yaw);
		*/
		TempRotation.Yaw += GetPlayersRot().Yaw;
		UE_LOG(LogTemp, Warning, TEXT("Final: %f"), TempRotation.Yaw);
		GrabbedActorRotation = TempRotation;

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

float UGrabber::GetOffsetYaw(float PlayerYaw, float ObjectYaw) {
	if(ObjectYaw < 0) ObjectYaw += 360.f;
	float Offset = PlayerYaw - ObjectYaw;
	if(Offset <= -180.f) Offset += 360.f;
	return Offset;
}

void UGrabber::Release() {
	if(!PhysicsHandle) return;

	UE_LOG(LogTemp, Warning, TEXT("Grabber Released"));

	if(PhysicsHandle->GrabbedComponent != nullptr) {
		PhysicsHandle->ReleaseComponent();
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	float NewPlayerYaw = GetPlayersRot().Yaw;

	if(PhysicsHandle && PhysicsHandle->GetGrabbedComponent() != nullptr) {
		float DeltaYaw = NewPlayerYaw - PrevPlayerYaw;
		GrabbedActorRotation.Yaw += DeltaYaw;
		
		PhysicsHandle->SetTargetLocationAndRotation(
			GetPlayersReach(), 
			GrabbedActorRotation
		);
	}
	
	PrevPlayerYaw = NewPlayerYaw;
}

FHitResult UGrabber::GetFirstPhysicsBodyInReach() const {
	FHitResult Hit;
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, this->GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		Hit,
		GetPlayersWorldPos(),
		GetPlayersReach(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	// Debug
	AActor* ActorHit = Hit.GetActor();

	if(ActorHit != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("Line trace has hit: %s"), *ActorHit->GetName());
	}
	// Debug end

	return Hit;
}

FVector UGrabber::GetPlayersReach() const {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetPlayersWorldPos() const {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}

FRotator UGrabber::GetPlayersRot() const {
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerViewPointLocation,
		PlayerViewPointRotation
	);

	return PlayerViewPointRotation;
}

void UGrabber::HalfupRotation(FRotator& Rotation) {
	Rotation.Pitch = HalfupValue(Rotation.Pitch);
	Rotation.Yaw = HalfupValue(Rotation.Yaw);
	Rotation.Roll = HalfupValue(Rotation.Roll);
}

float UGrabber::HalfupValue(float val) {
	float valAbs = (val >= 0) ? val : -val;

	int32 mult = (int32)(valAbs / 90.f);
	float reminder = valAbs - (mult * 90);
	
	if(val >= 0) {
		return (reminder >= 45.f) ? (mult + 1) * 90.f : mult * 90.f;
	} else {
		return (reminder >= 45.f) ? -((mult + 1) * 90.f) : -(mult * 90.f);
	}
}