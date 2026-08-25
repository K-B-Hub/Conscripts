//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PassiveSkillComponent.generated.h"

class UPassiveSkillBase;
class ACharacterBase;
enum class EReactiveType : uint8;
enum class EConditionalType : uint8;
enum class ESkillType : uint8;
enum class EDamageType : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UPassiveSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPassiveSkillComponent();

	virtual void BeginPlay() override;
	
	//패시브 스킬 등록 및 타입별 분류
	void AddPassive(TSubclassOf<UPassiveSkillBase> passiveClass);

	//인덱스로 패시브 제거
	void RemovePassiveAt(int32 Index);

	//클래스로 패시브 제거
	void RemovePassiveByClass(TSubclassOf<UPassiveSkillBase> passiveClass);

	//해당 클래스의 패시브를 이미 보유 중인지, 강화 후보 중복 필터용
	bool HasPassiveClass(TSubclassOf<UPassiveSkillBase> passiveClass) const;

	//스트레스 이벤트를 긍정 풀로 고정하는 패시브를 보유 중인지
	bool HasStressEventAlwaysPositive() const;

	const TArray<TObjectPtr<UPassiveSkillBase>>& GetActivePassives() const { return activePassives; }

	//피해 계산 전 패시브 실행, attackerLocation은 거리 조건 패시브가 쓰는 공격 시점 좌표
	void DispatchBeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType,
		const FVector& attackerLocation,
		float& dmg, int32& amp, int32& pen, float& acc, float& crit, float& critDmg);

	//피해 적용 후 패시브 실행
	void DispatchAfterDamage(ACharacterBase* target);

	//처치 후 패시브 실행
	void DispatchAfterSlay(const FVector& slainLocation);

	//치사 피해 직전 패시브 실행, 하나라도 생존시키면 true
	bool DispatchBeforeDeath(int32 hpBefore, int32 incomingDamage);

	//이동 상태 변경 패시브 실행
	void DispatchBeforeMove(bool bIsMoved, int32 sign);

	//Conditional 패시브 실행
	void DispatchConditional(EConditionalType type);

	//패시브로 얻는 전투 파생 스탯 보너스 누적, 재계산 덮어쓰기 방지 위해 컴포넌트에 저장
	void AddCombatStatBonus(float acc, float eva, float crit)
	{
		accuracyBonus += acc;
		evasionBonus += eva;
		criticalBonus += crit;
	}
	float GetAccuracyBonus() const { return accuracyBonus; }
	float GetEvasionBonus() const { return evasionBonus; }
	float GetCriticalBonus() const { return criticalBonus; }

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	//전투 파생 스탯 보너스 누적치, 스탯 재계산 시 가산
	UPROPERTY(VisibleAnywhere)
	float accuracyBonus = 0.f;
	UPROPERTY(VisibleAnywhere)
	float evasionBonus = 0.f;
	UPROPERTY(VisibleAnywhere)
	float criticalBonus = 0.f;

	//전체 패시브 목록
	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> activePassives;

	//타입별 패시브 목록
	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> statPassives;

	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> reactivePassives;

	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> conditionalPassives;
};
