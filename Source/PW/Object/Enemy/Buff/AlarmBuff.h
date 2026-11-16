//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "AlarmBuff.generated.h"

class UNiagaraSystem;
class USoundBase;

//알람 버프, 만료 턴(잔여 1턴)에 맵의 모든 적을 전투에 합류시킴
//시전자를 버프 만료 전에 처치하면 알람이 무산됨
UCLASS()
class PW_API UAlarmBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	UAlarmBuff();

	virtual void Execute(ACharacterBase* affected) override;

	//알람 발동 순간 시전자 위치에 1회 재생, 무산되면 재생 안 함
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff|Presentation")
	TObjectPtr<UNiagaraSystem> alarmEffect;

	//alarmEffect와 같은 시점·위치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Buff|Presentation")
	TObjectPtr<USoundBase> alarmSound;
};
