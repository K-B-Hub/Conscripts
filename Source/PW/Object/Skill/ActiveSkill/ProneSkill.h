//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "ProneSkill.generated.h"

class UAnimMontage;

//포복 토글 스킬, 자신에게 시전 — 엎드리면 시야 눈높이가 낮아져 낮은 엄폐물에 은폐되고 이동력 소모가 2배
//서있다가 진입 시에만 행동력 1 소모, 일어서기는 무료
UCLASS()
class PW_API UProneSkill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	UProneSkill();

	//일어서기(이미 포복 중)는 행동력 검사 없이 항상 가능
	virtual bool CanExecute() const override;

	//진입 시에만 행동력 소모 후 자세 토글, 방향별 전환 몽타주 재생, 일어서기는 무료
	virtual void BeginUse() override;

protected:
	//서있음→포복 전환(엎드리기) 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> lieDownMontage;
	//포복→서있음 전환(일어서기) 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> standUpMontage;
};