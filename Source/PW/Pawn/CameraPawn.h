// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

//전투 카메라 전용 폰, BattleController가 상시 빙의해 턴 캐릭터를 추적
UCLASS()
class PW_API ACameraPawn : public APawn
{
	GENERATED_BODY()

public:
	ACameraPawn();

	USpringArmComponent* GetSpringArm() const { return springArmComponent; }

protected:
	//카메라 피벗 루트
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USceneComponent> rootScene;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> springArmComponent;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> cameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraArmLength = 1400.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraPitchAngle = -55.f;
};
