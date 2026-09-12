//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AllyRunState.generated.h"

class AAllyCharacterBase;
class USkillBase;

//스테이지 간 이월되는 아군 1명의 상태 스냅샷
//레벨업 스탯 상승이 확률 기반(LevelUp)이라 재현이 불가능하므로 최종 수치를 그대로 보관
//파생 스탯(accuracy/evasion/critical)은 SetDefaultStats가 재계산하므로 저장하지 않는다
USTRUCT(BlueprintType)
struct FAllyRunState
{
	GENERATED_BODY()

	//스폰할 직업 클래스
	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<AAllyCharacterBase> AllyClass;

	//편성·출격 위젯에 표시할 개체 이름, 직업명과 별개
	UPROPERTY(BlueprintReadWrite)
	FString DisplayName;

	//레벨 진행도
	UPROPERTY(BlueprintReadWrite)
	int32 Level = 1;
	UPROPERTY(BlueprintReadWrite)
	float Exp = 0.f;
	UPROPERTY(BlueprintReadWrite)
	float MaxExp = 100.f;

	//베이스 스탯, 확률 성장과 강화 누적이 모두 반영된 최종 수치
	UPROPERTY(BlueprintReadWrite)
	int32 MaxHp = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 Atk = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 Speed = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 Skill = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 Def = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 Mentality = 1;
	UPROPERTY(BlueprintReadWrite)
	float MovingPoint = 9.f;
	UPROPERTY(BlueprintReadWrite)
	int32 ActionPoint = 2;
	UPROPERTY(BlueprintReadWrite)
	int32 MaxStress = 100;
	UPROPERTY(BlueprintReadWrite)
	int32 MaxBattleResource = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 DamageReduction = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 DamageAmplification = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 Penetration = 0;
	UPROPERTY(BlueprintReadWrite)
	float Sight = 15.f;
	//파생 공식이 없어 SetDefaultStats 재계산을 타지 않으므로 저장 대상
	UPROPERTY(BlueprintReadWrite)
	float CriticalDamage = 2.f;

	//소모성 상태
	UPROPERTY(BlueprintReadWrite)
	int32 Hp = 10;
	UPROPERTY(BlueprintReadWrite)
	int32 Stress = 0;
	UPROPERTY(BlueprintReadWrite)
	int32 BattleResource = 10;

	//습득한 강화(액티브·패시브) 클래스, 습득 순서 그대로
	//스탯 기여분은 위 수치에 이미 반영되어 있고 이 목록은 스킬·패시브 인스턴스 복원용
	//1회성 즉시효과(URestBase)는 인스턴스가 남지 않으므로 기록하지 않는다
	UPROPERTY(BlueprintReadWrite)
	TArray<TSubclassOf<USkillBase>> AcquiredUpgrades;

	//AllyCharacterBase가 전방 선언뿐이라 TSubclassOf 비교를 헤더에서 할 수 없어 .cpp에 정의
	bool IsValid() const;

	//직업 CDO의 기본 스탯으로 새 로스터 항목 생성, 편성과 중간 영입이 공유
	static FAllyRunState MakeFromClass(TSubclassOf<AAllyCharacterBase> allyClass, const FString& displayName);
};