//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PW.h"
#include "Object/Ailment/AilmentBase.h"
#include "RifleStickyBombAilment.generated.h"

class UNiagaraSystem;
class USoundBase;

//점착 폭탄이 부여하는 상태이상, 대상의 다음 턴 시작에 터져 주변까지 피해를 준다
//피해량은 시전자 공격력에서 나오므로 적용 시점에 스냅샷해 둔다
UCLASS()
class PW_API URifleStickyBombAilment : public UAilmentBase
{
	GENERATED_BODY()

public:
	URifleStickyBombAilment();

	virtual void OnApply(ACharacterBase* affected, ACharacterBase* caster) override;
	virtual void Execute(ACharacterBase* affected) override;

protected:
	//폭발 반경
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	float explodeRadius = 3.f * CmPerMeter;

	//시전자 공격력에 곱할 피해 하한·상한 계수
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	float damageRatio = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat|Ailment")
	float damageRatioMax = 1.5f;

	//폭발 시 폭탄이 붙은 캐릭터 위치에 출력, 여러 개를 겹친다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ailment|Effect")
	TArray<TObjectPtr<UNiagaraSystem>> explodeEffects;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ailment|Effect")
	TObjectPtr<USoundBase> explodeSound;

private:
	//적용 시점에 스냅샷한 피해 구간, 시전자가 죽거나 스탯이 바뀌어도 변하지 않는다
	UPROPERTY()
	float snappedDamage = 0.f;
	UPROPERTY()
	float snappedDamageMax = 0.f;
};
