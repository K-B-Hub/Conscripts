//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/SkillBase.h"
#include "RestBase.generated.h"

class ACharacterBase;

//레벨업 강화로 습득하는 1회성 즉시 효과의 공통 기반
//액티브/패시브와 달리 컴포넌트에 등록되지 않고 습득 즉시 Execute 후 버려짐
UCLASS(Abstract, BlueprintType)
class PW_API URestBase : public USkillBase
{
	GENERATED_BODY()

public:
	//습득 즉시 1회 발동, 효과는 파생 클래스가 정의
	virtual void Execute(ACharacterBase* instigator);

protected:
	//시전자와 같은 팀의 생존 캐릭터 수집, 시전자 본인 포함
	//아군이 습득하면 아군 전체, 적이 습득하면 적 전체가 대상
	static void CollectTeam(const ACharacterBase* instigator, TArray<ACharacterBase*>& outTeam);

	//비율 회복량 계산, 최소 1 보장 (base가 0 이하면 0)
	static int32 CalcRatioAmount(int32 base, float ratio);
};