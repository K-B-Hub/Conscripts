//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "RifleLeadership.generated.h"

//중급 랜덤 강화, 주변 아군에게 공격력을 나눠주는 오라
//버프가 아니라 스탯 델타를 직접 걸고 회수해 범위를 벗어나면 즉시 사라진다
UCLASS()
class PW_API URifleLeadership : public UPassiveSkillBase
{
	GENERATED_BODY()

public:
	URifleLeadership();

	virtual void Execute_Conditional() override;

protected:
	//오라 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Aura")
	float auraRange = 5.f * CmPerMeter;

private:
	//현재 보너스를 받고 있는 아군, 범위 이탈 시 회수 대상을 추적한다
	TArray<TWeakObjectPtr<ACharacterBase>> buffedAllies;
};