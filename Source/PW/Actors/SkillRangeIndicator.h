// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillRangeIndicator.generated.h"

class UDecalComponent;

// 스킬 사거리를 원형 데칼로 표시하는 액터
// 시전자 위치에 스폰되어 pickRange 반경을 지면에 투영
UCLASS()
class PW_API ASkillRangeIndicator : public AActor
{
	GENERATED_BODY()

public:
	ASkillRangeIndicator();

	// 사거리 설정 — 데칼 크기를 Range에 맞게 갱신
	void InitRange(float Range);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> rootScene;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UDecalComponent> rangeDecal;

	// 사거리 데칼 머티리얼 (BP에서 지정)
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	TObjectPtr<UMaterialInterface> rangeDecalMaterial;

	// 데칼 투영 깊이 (cm)
	UPROPERTY(EditDefaultsOnly, Category = "Decal")
	float decalProjectionDepth = 500.f;
};