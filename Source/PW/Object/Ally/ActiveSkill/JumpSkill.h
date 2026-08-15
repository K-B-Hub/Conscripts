//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "JumpSkill.generated.h"

//점프 스킬, 지면 지점 선택 — 포물선 궤적으로 이동해 낮은 장애물·지형을 넘는다
//이동력을 수평거리의 1.5배 소모(행동력은 사용 안 함), 실행부는 BattleController가 궤적을 캐릭터에 전달
UCLASS()
class PW_API UJumpSkill : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	UJumpSkill();

	//포복 중에는 점프 불가
	virtual bool CanExecute() const override;

	//실행 시 시전자를 궤적을 따라 이동시킴
	virtual bool UsesArcMovement() const override { return true; }
};