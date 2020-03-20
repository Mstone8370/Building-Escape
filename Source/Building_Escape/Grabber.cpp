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

	AdjustAngle = (AdjustAngle >= 0) ? AdjustAngle : -AdjustAngle;
	FindPhysicsHandle();
	SetupInputComponent();
	PrevPlayerYaw = GetPlayersRotation().Yaw;
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

		GrabbedActorRotation = AdjustGrabbedActorRotation(HitResult.GetComponent()->GetOwner());

		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			GetPlayersReach(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);
	}
}

FRotator UGrabber::AdjustGrabbedActorRotation(AActor* GrabbedActor) {
	FRotator TempRotation = GrabbedActor->GetActorRotation();
	TempRotation.Yaw = CalcOffsetYaw(GetPlayersRotation().Yaw, TempRotation.Yaw);
	HalfupRotation(TempRotation);
	TempRotation.Yaw = GetPlayersRotation().Yaw - TempRotation.Yaw;
	
	return TempRotation;
}

float UGrabber::CalcOffsetYaw(float PlayerYaw, float ObjectYaw) {
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
	float NewPlayerYaw = GetPlayersRotation().Yaw;

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

FRotator UGrabber::GetPlayersRotation() const {
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
	Rotation.Roll = HalfupValue(Rotation.Roll);
	if(AdjustOrNot(Rotation.Yaw)) {
		// UE_LOG(LogTemp, Warning, TEXT("TRUE: %f"), Rotation.Yaw);
		Rotation.Yaw = HalfupValue(Rotation.Yaw);
	} else {
		// UE_LOG(LogTemp, Warning, TEXT("FALSE: %f"), Rotation.Yaw);
	}
}

bool UGrabber::AdjustOrNot(float Yaw) {
	return (-AdjustAngle <= Yaw && Yaw <= AdjustAngle) ||
		(90.f - AdjustAngle <= Yaw && Yaw <= 90 + AdjustAngle) ||
		(-90.f - AdjustAngle <= Yaw && Yaw <= -90.f + AdjustAngle) ||
		(Yaw >= 180.f - AdjustAngle || Yaw <= -180.f + AdjustAngle);
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