// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/PassiveSkill/test/TestBeforeMove.h"
#include "Characters/CharacterBase.h"

UTestBeforeMove::UTestBeforeMove()
{
	passiveType = EPassiveType::Reactive;
	reactiveType = EReactiveType::BeforeMove;
	conditionType = EConditionalType::None;

	// 테스트 보너스 — BP 파생에서 조정 가능
	atk = 5;
}

void UTestBeforeMove::Execute_BeforeMove(bool bIsMoved, int32 sign)
{
	// 이동 보너스 패턴 — bIsMoved 매칭 시 sign 그대로 적용
	if (bIsMoved)
	{
		if (ACharacterBase* o = owner.Get())
		{
			o->ApplyPassiveStatDelta(this, sign);
		}
	}
}