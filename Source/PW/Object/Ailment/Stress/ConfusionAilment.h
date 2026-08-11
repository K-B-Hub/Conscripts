//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Ailment/AilmentBase.h"
#include "ConfusionAilment.generated.h"

class UActiveSkillBase;
class AAllyCharacterBase;

//스트레스 부정 이벤트 '착란', 2턴간 랜덤 공격 스킬을 피아 구분 없는 랜덤 대상에게 시전 후 턴 종료
UCLASS()
class PW_API UConfusionAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	UConfusionAilment();
	virtual void Execute(ACharacterBase* affected) override;

private:
	//랜덤 공격 스킬과 접근 가능한 랜덤 대상 선정, 시전 불가면 false 반환해 행동 없이 턴 종료
	bool PickRandomSkillAndTarget(AAllyCharacterBase* ally, UActiveSkillBase*& outSkill,
		ACharacterBase*& outTarget, FVector& outCastFrom) const;

	//대상에게 스킬을 쓸 수 있는 시전 지점 탐색, 잔여 이동력으로 도달 불가면 false
	bool FindCastPosition(AAllyCharacterBase* ally, const UActiveSkillBase* skill,
		const ACharacterBase* target, FVector& outCastFrom) const;

	//시전 지점 도착(또는 이동력 소진) 시 시전으로 진행
	void OnApproachCompleted();

	//선정된 스킬을 시전하고 커밋 완료 폴링 시작
	void CastAndWaitCommit();

	//시전 커밋(노티파이/몽타주 종료) 완료 감지 후 턴 종료
	void PollCommitFinished();

	FTimerHandle commitWaitHandle;

	//콜백에서 시전자·대상·스킬을 복원하기 위한 보관
	TWeakObjectPtr<AAllyCharacterBase> confusedAlly;
	TWeakObjectPtr<ACharacterBase> pendingTarget;

	UPROPERTY()
	TObjectPtr<UActiveSkillBase> pendingSkill;
};