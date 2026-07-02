//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AilmentBase.generated.h"

class ACharacterBase;

//모든 상태이상의 공통 기반
UCLASS(Abstract, BlueprintType)
class PW_API UAilmentBase : public UObject
{
	GENERATED_BODY()
public:
	//초기 지속 턴수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	int32 ailmentTurn = 3;

	//현재 남은 턴수
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Ailment")
	int32 remainingTurn = 0;

	//턴 시작시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	bool isStart = false;

	//턴 종료시 Execute 작동 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	bool isEnd = false;

	//중복 적용 가능 여부
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	bool isStackable = false;

	//적용 직후 1회 호출
	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster);

	//턴 시작/종료 효과
	virtual void Execute(ACharacterBase* affected);
};
