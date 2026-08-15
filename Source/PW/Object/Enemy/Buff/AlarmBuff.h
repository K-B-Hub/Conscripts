//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "AlarmBuff.generated.h"

//알람 버프, 만료 턴(잔여 1턴)에 맵의 모든 적을 전투에 합류시킴
//시전자를 버프 만료 전에 처치하면 알람이 무산됨
UCLASS()
class PW_API UAlarmBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UAlarmBuff();

	virtual void Execute(ACharacterBase* affected) override;
};
