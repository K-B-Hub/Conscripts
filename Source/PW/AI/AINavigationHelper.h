//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AINavigationHelper.generated.h"

class ACharacterBase;
class ATerrainBase;

//경로가 지형과 만나는 방식 요약, AI 이동 평가용
USTRUCT()
struct FTerrainPathInfo
{
	GENERATED_BODY()

	//지형 소모 배율을 반영한 실질 이동 비용(cm), 지형이 없으면 경로 길이와 동일
	float WeightedCostCm = 0.f;

	//경로가 통과한 지형 목록, 중복 없음
	UPROPERTY()
	TArray<TObjectPtr<ATerrainBase>> PassedTerrains;

	//도착 지점이 속한 지형 목록, 중복 없음
	UPROPERTY()
	TArray<TObjectPtr<ATerrainBase>> DestTerrains;
};

//AI 이동 판정 헬퍼
UCLASS()
class PW_API UAINavigationHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//지정 지점까지 도달 가능 여부와 경로 길이 반환
	//도달 판정은 지형 배율을 반영한 실질 비용 기준, OutPathLengthCm은 거리 페널티용 실제 길이
	//OutTerrainInfo를 넘기면 지형 평가에 필요한 통과·도착 지형 정보를 함께 채움
	static bool CanReach(const ACharacterBase* Mover, const FVector& Target, float& OutPathLengthCm,
		FTerrainPathInfo* OutTerrainInfo = nullptr);

	//지정 위치에서 대상까지 시야선 확인
	static bool HasLineOfSightFrom(const ACharacterBase* Caster, const FVector& FromLocation, const ACharacterBase* Target);

	//경로 폴리라인을 일정 간격으로 샘플링해 지형 배율을 반영한 실질 비용과 지형 목록 산출
	static void ComputeTerrainPathInfo(const UWorld* World, const TArray<FVector>& PathPoints, FTerrainPathInfo& OutInfo);

	//지정 위치가 속한 지형 목록, 이동 없는 제자리 후보 평가용
	static void GetTerrainsAt(const UWorld* World, const FVector& Location, TArray<TObjectPtr<ATerrainBase>>& OutTerrains);
};