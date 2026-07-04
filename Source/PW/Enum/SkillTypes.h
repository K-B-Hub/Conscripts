#pragma once
#include "CoreMinimal.h"
#include "SkillTypes.generated.h"

UENUM(BlueprintType)
enum class ESelectMode : uint8		//스킬 사용시 선택 방식
{
	Self        UMETA(DisplayName = "Self"),		//본인에게 시전(버프,회복, 주변 버프, 회복, 디버프 등)
	SinglePick  UMETA(DisplayName = "Single Pick"),	//한명의 대상에게 시전, 시전 대상 수 만큼 지정 가능
	GroundPoint UMETA(DisplayName = "Ground Point")	//원하는 지점에 시전 가능
};

UENUM(BlueprintType)
enum class EPickTeam : uint8		//선택 대상, SelectMode의 SinglePick일때만 진입해 확인
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
	Heal		UMETA(DisplayName = "Heal"),	//회복 전용 스킬(음수 피해량)
	Buff        UMETA(DisplayName = "Buff"),	//버프 전용 스킬(피해량 없음)
	Ailment		UMETA(DisplayName = "Ailment")	//상태이상 전용 스킬(피해량 없음)
};

UENUM(BlueprintType)
enum class EDamageType : uint8		//액티브 스킬 중 피해를 주는 스킬의 피해 유형
{
	Normal      UMETA(DisplayName = "Normal"),		//명중 기반 계산, 관통력, 피해 증가 등 모든 스탯 적용
	Area        UMETA(DisplayName = "Area")			//치명타가 적용될 수 없음
};

UENUM(BlueprintType)
enum class EPassiveType : uint8		//패시브 스킬 유형
{
	Stat		UMETA(DisplayName = "Stat"),		//단순 스탯 증가
	Conditional	UMETA(DisplayName = "Conditional"),	//턴 시작, 게임 내 캐릭터 이동 완료, 체력 감소 등의 이벤트 발생시 확인하며 적용
	Reactive	UMETA(DisplayName = "Reactive")		//본인의 행동에 반응하는 패시브 (내가 데미지를 줄 때, 액티브 스킬 사용 중간에 호출해 스킬 강화 등)
};

UENUM(BlueprintType)
enum class EReactiveType : uint8
{
	None				UMETA(DisplayName = "None"),			//반응형 패시브가 아님
	BeforeMove			UMETA(DisplayName = "BeforeMove"),		//소지자의 이동 시작 시 (실제 이동 + 인디케이터 자동이동 가상 토글)
	BeforeDamageCalc	UMETA(DisplayName = "Before Damage"),	//데미지 계산 이전
	AfterDamage			UMETA(DisplayName = "After Damage"),	//데미지 적용 후
	AfterSlay			UMETA(DisplayName = "After Slay")		//적 처치 후
};

UENUM(BlueprintType)
enum class EConditionalType : uint8
{
	None				UMETA(DisplayName = "None"),		//조건부형 패시브가 아님
	MoveComplete	UMETA(DisplayName = "Move Complete"),	//게임 내 캐릭터의 이동 완료 시(소지자 주변에 아군 혹은 적군이 몇명인지에 따라 달라지는 패시브 스킬 용도, 소지자의 움직임 아님)
	TurnStart		UMETA(DisplayName = "Turn Start"),		//턴 시작
	TurnEnd			UMETA(DisplayName = "Turn End"),		//턴 종료
	Damaged			UMETA(DisplayName = "Damaged"),			//체력 감소시
	AllyDeath		UMETA(DisplayName = "Ally Death"),		//아군 사망시
	EnemyDeath		UMETA(DisplayName = "Enemy Death"),		//적 사망시
	RoundStart		UMETA(DisplayName = "Round Start")		//라운드 시작시
};
