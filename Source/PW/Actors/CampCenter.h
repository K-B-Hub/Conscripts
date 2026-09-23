//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampCenter.generated.h"

class UStaticMeshComponent;

//야영지의 메인 캠프, 클릭하면 정비 선택지가 열린다
//맵에 하나만 배치한다
UCLASS()
class PW_API ACampCenter : public AActor
{
	GENERATED_BODY()

public:
	ACampCenter();

protected:
	//모닥불·천막 등 외형, BP에서 메시를 지정한다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camp")
	TObjectPtr<UStaticMeshComponent> campMesh;
};