//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AINavigationHelper.generated.h"

class ACharacterBase;

//AI 이동 판정 헬퍼
UCLASS()
class PW_API UAINavigationHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//지정 지점까지 도달 가능 여부와 경로 길이 반환
	static bool CanReach(const ACharacterBase* Mover, const FVector& Target, float& OutPathLengthCm);

	//지정 위치에서 대상까지 시야선 확인
	static bool HasLineOfSightFrom(const ACharacterBase* Caster, const FVector& FromLocation, const ACharacterBase* Target);
};
