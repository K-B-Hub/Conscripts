//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Enum/UpgradeGrade.h"
#include "UpgradeLibrary.generated.h"

class USkillBase;
class UUpgradeTableData;
class ACharacterBase;

//강화효과 추첨 로직 모음, 아군 레벨업과 적 AI 강화가 공유하는 무상태 헬퍼
UCLASS()
class PW_API UUpgradeLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	//등급 확률 추첨, 55/30/10/5
	static EUpgradeGrade RollGrade();

	//강화 후보 count개를 추출, character는 보유 중복 필터용
	//뽑힌 등급 후보가 부족하면 하위 등급으로 보충, 등급 내 공용/직업 분배는 랜덤이며 부족 시 반대 풀에서 보충
	static TArray<TSubclassOf<USkillBase>> BuildChoices(
		const ACharacterBase* character,
		const UUpgradeTableData* classTable,
		const UUpgradeTableData* commonTable,
		int32 count);

private:
	//해당 스킬을 이 캐릭터가 지금 습득 가능한지, 액티브/중복불가 패시브 보유 시 false
	static bool CanAcquire(const ACharacterBase* character, TSubclassOf<USkillBase> skillClass);

	//pool에서 이미 뽑힌 것과 습득 불가를 제외한 후보만 수집
	static void CollectAcquirable(
		const ACharacterBase* character,
		const TArray<TSubclassOf<USkillBase>>& pool,
		const TArray<TSubclassOf<USkillBase>>& already,
		TArray<TSubclassOf<USkillBase>>& outCandidates);

	//candidates에서 무작위 n개를 뽑아 out에 추가, 뽑힌 항목은 candidates에서 제거
	static void PickRandom(TArray<TSubclassOf<USkillBase>>& candidates, int32 n, TArray<TSubclassOf<USkillBase>>& out);
};
