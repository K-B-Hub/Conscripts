//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "RifleReload.generated.h"

//소총수 기본 스킬, 탄창을 보충한다
UCLASS()
class PW_API URifleReload : public UActiveSkillBase
{
	GENERATED_BODY()

public:
	URifleReload();

	//대상이 없는 자기 효과라 대상 루프가 아닌 커밋 시점에 1회 보충
	virtual void OnCommit() override;

protected:
	//한 번에 보충할 탄창 수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Reload")
	int32 reloadAmount = 3;
};
