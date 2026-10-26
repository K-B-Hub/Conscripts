//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RiflePackLeader.generated.h"

//최상급 랜덤 강화, 자신이 받고 있는 버프의 스탯 합계를 주변 아군에게 그대로 나눠준다
//합계가 유동적이라 매 평가 때 자신의 스탯 필드를 갱신해 쓴다
UCLASS()
class PW_API URiflePackLeader : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URiflePackLeader();

	virtual void Execute_Conditional() override;

protected:
	//오라 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Aura")
	float auraRange = 8.f * CmPerMeter;

private:
	//소지자의 현재 버프 스탯 합계를 자신의 필드에 기록, 나눠줄 보너스가 하나라도 있으면 true
	bool SnapshotBuffTotals(const ACharacterBase* holder);

	//현재 보너스를 받고 있는 아군, 범위 이탈 시 회수 대상을 추적한다
	TArray<TWeakObjectPtr<ACharacterBase>> sharedAllies;
};
