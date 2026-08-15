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

	//진입 시에만 행동력 소모, 일어서기는 무료
	virtual void BeginUse() override;

	//전환 방향에 맞는 몽타주 선택, 커밋 전 호출이라 IsProne은 토글 이전 상태
	virtual UAnimMontage* GetCommitMontage() const override;

	//임팩트 프레임에 자세 토글, 이후 애님BP 스테이트머신이 bIsProne을 보고 유지 자세 처리
	virtual void OnCommit() override;

protected:
	//서있음→포복 전환(엎드리기) 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> lieDownMontage;
	//포복→서있음 전환(일어서기) 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Animation")
	TObjectPtr<UAnimMontage> standUpMontage;
};