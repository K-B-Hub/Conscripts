// Fill out your copyright notice in the Description page of Project Settings.

#include "Pawn/CameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

ACameraPawn::ACameraPawn()
{
	//이동·회전은 BattleController가 스프링암을 직접 조작
	PrimaryActorTick.bCanEverTick = false;

	rootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(rootScene);

	springArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	springArmComponent->SetupAttachment(rootScene);
	springArmComponent->TargetArmLength = cameraArmLength;
	springArmComponent->SetRelativeRotation(FRotator(cameraPitchAngle, 0.f, 0.f));
	springArmComponent->bUsePawnControlRotation = false;
	springArmComponent->bDoCollisionTest = false;

	cameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	cameraComponent->SetupAttachment(springArmComponent, USpringArmComponent::SocketName);
	cameraComponent->bUsePawnControlRotation = false;
}
