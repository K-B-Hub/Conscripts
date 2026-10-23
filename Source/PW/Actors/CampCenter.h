//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CampCenter.generated.h"

class UStaticMeshComponent;
class UMaterialInterface;
class UMaterialInstanceDynamic;

//야영지의 메인 캠프, 클릭하면 정비 선택지가 열린다
//맵에 하나만 배치한다
UCLASS()
class PW_API ACampCenter : public AActor
{
	GENERATED_BODY()

public:
	ACampCenter();

	//호버 상태 전환, 강조 강도만 바꾼다
	void SetHovered(bool bHovered);

protected:
	virtual void BeginPlay() override;

	//모닥불·천막 등 외형, BP에서 메시를 지정한다
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camp")
	TObjectPtr<UStaticMeshComponent> campMesh;

	//림 발광 오버레이 머티리얼, intensityParameterName 스칼라를 가져야 한다
	UPROPERTY(EditDefaultsOnly, Category = "Camp|Highlight")
	TObjectPtr<UMaterialInterface> highlightOverlayMaterial;

	//평소 강조 강도, 클릭할 수 있다는 것만 알릴 정도로 약하게
	UPROPERTY(EditDefaultsOnly, Category = "Camp|Highlight")
	float idleIntensity = 0.3f;

	//호버 시 강조 강도
	UPROPERTY(EditDefaultsOnly, Category = "Camp|Highlight")
	float hoveredIntensity = 1.f;

	//강도를 전달할 머티리얼 스칼라 파라미터 이름
	UPROPERTY(EditDefaultsOnly, Category = "Camp|Highlight")
	FName intensityParameterName = TEXT("Intensity");

private:
	//강도 전달용 동적 머티리얼, 머티리얼 미지정이면 생성되지 않음
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> highlightMID;
};
