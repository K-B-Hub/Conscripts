// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AINavigationHelper.generated.h"

class ACharacterBase;

// AI 후보 열거 단계에서 자유위치 도달 판정에 사용하는 헬퍼.
// 자유위치 + 다수 후보 평가 패턴이라 호출 빈도가 높음 — 사전 컷이 핵심.
UCLASS()
class PW_API UAINavigationHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 지정 지점까지 NavMesh 도달 가능 여부 + 경로 길이(cm) 반환.
	// 사전 컷: 직선거리 > Mover의 잔여 이동력이면 패스파인딩 생략.
	//   (NavPath 길이는 항상 직선거리 이상이라 거짓 음성 없는 안전한 가지치기)
	// 호출자는 더 큰 사전 컷(예: MoveRange + Skill.Range)을 별도로 적용 가능.
	// 단위 주의: Mover의 currentMovingPoint는 미터 단위 → 내부에서 cm로 변환해 비교.
	static bool CanReach(const ACharacterBase* Mover, const FVector& Target, float& OutPathLengthCm);

	// FromLocation(눈 높이)에서 Target(눈 높이)까지 사선 확보 여부.
	// 캐스터 자신은 ignore. hit이 Target 본인이면 사선 확보로 판정 (EnemyAIController detection 패턴과 동일).
	// 매직 EyeHeight=80cm는 플레이어 측 AttackRangeIndicator와 일치 — 변경 시 양쪽 동기화 필요.
	static bool HasLineOfSightFrom(const ACharacterBase* Caster, const FVector& FromLocation, const ACharacterBase* Target);
};