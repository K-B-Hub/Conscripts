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

	const TArray<TObjectPtr<UPassiveSkillBase>>& GetActivePassives() const { return activePassives; }

	//피해 계산 전 패시브 실행
	void DispatchBeforeDamageCalc(ACharacterBase* target, ESkillType skillType, EDamageType damageType,
		float& dmg, int32& amp, int32& pen, float& acc, float& crit);

	//피해 적용 후 패시브 실행
	void DispatchAfterDamage(ACharacterBase* target);

	//처치 후 패시브 실행
	void DispatchAfterSlay(const FVector& slainLocation);

	//이동 상태 변경 패시브 실행
	void DispatchBeforeMove(bool bIsMoved, int32 sign);

	//Conditional 패시브 실행
	void DispatchConditional(EConditionalType type);

private:
	UPROPERTY()
	TObjectPtr<ACharacterBase> ownerCharacter = nullptr;

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
