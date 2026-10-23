//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AreaShapeLibrary.generated.h"

class UActiveSkillBase;

//범위 스킬의 형태 판정 단일 출처, 인디케이터와 AI 예측이 공유
//모든 판정은 수평(2D), 고저차는 무시
UCLASS()
class PW_API UAreaShapeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//형태 원점이 시전자에 고정되는가, Ray/Cone은 위치 대신 방향만 조준
	static bool IsCasterAnchored(const UActiveSkillBase* Skill);

	//후보 수집용 외접 반경(cm), 원점에서 형태가 닿는 최대 수평 거리
	static float GetOuterRadius(const UActiveSkillBase* Skill);

	//대상이 형태 내부인지 판정, Forward는 Ray/Cone의 전방(Circle은 미사용)
	static bool IsInAreaShape(const UActiveSkillBase* Skill, const FVector& Origin,
		const FVector& Forward, const FVector& TargetLocation);
};