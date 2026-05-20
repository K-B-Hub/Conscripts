// Fill out your copyright notice in the Description page of Project Settings.

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

// 패시브 스킬 보유/관리 컴포넌트
// - Stat: 등록 즉시 ApplyPassiveStatDelta(+1), 제거 시 -1 (영구 적용, 만료 없음)
// - Reactive/Conditional: 이벤트 디스패치 시 매칭되는 인스턴스의 Execute 호출 (추후 구현)
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PW_API UPassiveSkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPassiveSkillComponent();

	virtual void BeginPlay() override;
	
	// 패시브 스킬 등록 — NewObject로 인스턴스 생성 후 타입에 따라 처리
	// Stat이면 즉시 스탯 델타 적용, Reactive/Conditional은 목록 보관만
	void AddPassive(TSubclassOf<UPassiveSkillBase> passiveClass);

	// 인덱스로 패시브 제거 (Stat은 델타 되돌림)
	void RemovePassiveAt(int32 Index);

	// 클래스로 패시브 제거 — 첫 매칭 1건
	void RemovePassiveByClass(TSubclassOf<UPassiveSkillBase> passiveClass);

	const TArray<TObjectPtr<UPassiveSkillBase>>& GetActivePassives() const { return activePassives; }

	// Reactive 디스패치 — 데미지 계산 직전, 캐스터(=owner) 측 BeforeDamageCalc 패시브 일괄 평가
	// 조건 충족된 패시브의 보너스 필드를 ref 인자에 합산 (합연산만)
	// damageType이 Area면 후처리로 crit=0 강제 (Area는 치명타 불가, 패시브 합산이 부활시키는 것 차단)
	void DispatchBeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType,
		float& dmg, int32& amp, int32& pen, float& acc, float& crit);

	// Reactive 디스패치 — 데미지 적용 직후, 공격자(=owner) 측 AfterDamage 패시브 일괄 호출
	void DispatchAfterDamage(ACharacterBase* target);

	// Reactive 디스패치 — 처치 직후, 공격자(=owner) 측 AfterSlay 패시브 일괄 호출
	// slainLocation: 죽은 캐릭터의 위치
	void DispatchAfterSlay(const FVector& slainLocation);

	// Reactive 디스패치 — 소지자(=owner) 이동 상태 전이 시
	// bIsMoved: 어느 상태에 해당하는 보너스를 대상으로 할지
	// sign: +1=apply, -1=revert
	void DispatchBeforeMove(bool bIsMoved, int32 sign);

	// Conditional 디스패치 — 외부 이벤트 발생 시 호출
	// 매칭되는 conditionType의 패시브 인스턴스만 Execute_Conditional() 호출
	// 로컬 이벤트(TurnStart/TurnEnd/Damaged): CharacterBase가 본인 컴포넌트에 호출
	// 전역 이벤트(RoundStart/MoveComplete/AllyDeath/EnemyDeath): BattleGameMode가 모든 생존자에 호출
	void DispatchConditional(EConditionalType type);

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

	// 마스터 리스트 — 인덱스/클래스 기반 제거 및 외부 조회용
	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> activePassives;

	// 타입별 분류 배열 — 디스패치 시 순회 비용 절감
	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> statPassives;

	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> reactivePassives;

	UPROPERTY()
	TArray<TObjectPtr<UPassiveSkillBase>> conditionalPassives;
};
