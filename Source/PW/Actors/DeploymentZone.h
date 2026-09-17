//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DeploymentZone.generated.h"

class UBoxComponent;

//출격 시 아군을 배치할 수 있는 구획, 전투 맵에 하나 이상 배치한다
UCLASS()
class PW_API ADeploymentZone : public AActor
{
	GENERATED_BODY()

public:
	ADeploymentZone();

	//월드 지점이 이 구획 안인지, 배치 가능 판정에 사용
	bool ContainsPoint(const FVector& point) const;

	//Deploy 페이즈 동안만 구획을 드러낸다
	void SetZoneVisible(bool bVisible);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Deploy")
	TObjectPtr<UBoxComponent> zoneBox;
};