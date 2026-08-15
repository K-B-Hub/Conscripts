//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/TimerHandle.h"
#include "AilmentBase.generated.h"

class ACharacterBase;

//모든 상태이상의 공통 기반
UCLASS(Abstract, BlueprintType)
class PW_API UAilmentBase : public UObject
{
	GENERATED_BODY()
public:
	//상태이상 이름 (UI 표시용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ailment|Identity")
	FText ailmentName;

	//상태이상 설명 (UI 툴팁용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ailment|Identity")
	FText ailmentDescription;

	//상태이상 아이콘 (UI 표시용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ailment|Identity")
	TObjectPtr<UTexture2D> ailmentIcon;

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

	//AI 평가용, 턴당 기대 대미지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float aiDamagePerTurn = 0.f;

	//AI 평가용, 대상 턴당 기대 대미지에 곱할 계수 (1.0=완전 행동 불가, 0.7=약한 패널티)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	float aiControlCoefficient = 0.f;

	//적용 직후 1회 호출
	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster);

	//턴 시작/종료 효과
	virtual void Execute(ACharacterBase* affected);

protected:
	//행동 연출 딜레이, 상태이상 행동이 한 프레임에 끝나 보이지 않는 것 방지
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ailment|Pacing")
	float pacingDelay = 0.8f;

	//pacingDelay 후 action 실행, 딜레이 중 상태이상·대상이 사라지면 취소
	void ScheduleWithPacing(ACharacterBase* affected, TFunction<void(ACharacterBase*)> action);

private:
	FTimerHandle pacingTimerHandle;
};
