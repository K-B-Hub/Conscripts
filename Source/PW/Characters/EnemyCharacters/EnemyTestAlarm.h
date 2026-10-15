// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/EnemyCharacters/EnemyTest.h"
#include "EnemyTestAlarm.generated.h"

//알람 스킬 보유 테스트용 적, 이 클래스만 알람을 갖고 소수 배치하는 전제
//알람 스킬은 BP의 defaultSkills에서 지정한다
UCLASS()
class PW_API AEnemyTestAlarm : public AEnemyTest
{
	GENERATED_BODY()
};
