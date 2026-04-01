#pragma once
#include "CoreMinimal.h"
#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class ESelectMode : uint8		//스킬 사용시 선택 방식
{
	Self        UMETA(DisplayName = "Self"),		//본인에게 시전
	SinglePick  UMETA(DisplayName = "Single Pick"),	//한명의 대상에게 시전, 시전 대상 수 만큼 지정 가능
	GroundPoint UMETA(DisplayName = "Ground Point")	//원하는 지점에 시전 가능
};

UENUM(BlueprintType)
enum class EPickTeam : uint8		//선택 대상, SelectMode의 2,3일때만 진입해 확인
{
	EnemyOnly   UMETA(DisplayName = "Enemy Only"),	//적만 선택 가능
	AllyOnly    UMETA(DisplayName = "Ally Only"),	//아군만 선택 가능
	Any         UMETA(DisplayName = "Any")			//모두 선택 가능
};

UENUM(BlueprintType)
enum class EAreaTarget : uint8		//범위 공격 여부
{
	None        UMETA(DisplayName = "None"),		//단일 대상 공격
	EnemyOnly   UMETA(DisplayName = "Enemy Only"),	//범위 내 적만
	AllyOnly    UMETA(DisplayName = "Ally Only"),	//범위 내 아군만
	All         UMETA(DisplayName = "All")			//범위 내 전부
};

UENUM(BlueprintType)
enum class EAreaForm : uint8	//범위 공격의 범위 형태, EAreaTarget이 None이 아닐때만 확인
{
	Ray         UMETA(DisplayName = "Ray"),		//범위 파라미터1 : 폭, 범위 파라미터2 : 길이
	Cone        UMETA(DisplayName = "Cone"),	//범위 파라미터1 : 반지름, 범위 파라미터2 : 중심각
	Circle      UMETA(DisplayName = "Circle")	//범위 파라미터1 : 반지름
};

UENUM(BlueprintType)
enum class ESkillType : uint8		//액티브 스킬의 유형
{
	Melee       UMETA(DisplayName = "Melee"),	//근접
	Ranged      UMETA(DisplayName = "Ranged"),	//원거리
	Throw       UMETA(DisplayName = "Throw"),	//투척
	Buff        UMETA(DisplayName = "Buff")		//버프 및 디버프 포함
};

UENUM(BlueprintType)
enum class EDamageType : uint8		//액티브 스킬 중 피해를 주는 스킬의 피해 유형
{
	Normal      UMETA(DisplayName = "Normal"),		//명중 기반 계산, 관통력, 피해 증가 등 모든 스탯 적용
	Area        UMETA(DisplayName = "Area")			//피할 수 없음, 관통력 및 피해증가 등은 적용되지만 치명타가 적용될 수 없음
};

UENUM(BlueprintType)
enum class EPassiveType : uint8		//패시브 스킬 유형
{
	Stat		UMETA(DisplayName = "Stat"),		//단순 스탯 증가
	Conditional	UMETA(DisplayName = "Conditional"),	//턴 시작, 게임 내 캐릭터 이동 완료, 체력 감소 등의 이벤트 발생시 확인하며 적용
	Reactive	UMETA(DisplayName = "Reactive")		//본인의 행동에 반응하는 패시브 (내가 데미지를 줄 때, 액티브 스킬 사용 중간에 호출해 스킬 강화 등)
};