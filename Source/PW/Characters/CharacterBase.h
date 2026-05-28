// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enum/SkillTypes.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UHealthWidget;
class UStaticMeshComponent;
class UWidgetComponent;
class USkillComponent;
class UBuffComponent;
class UBuffBase;
class UAilmentComponent;
class UPassiveSkillComponent;
class UPassiveSkillBase;
class UActiveSkillBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ACharacterBase*, DeadCharacter);

// 데미지 사전 계산 결과 — PreviewDamage가 반환. RNG 적용 전 결정론적 값.
// AI 평가·인디케이터 표시·실제 데미지 적용 세 경로가 같은 산식을 통과하도록 설계의 단일 출처.
USTRUCT(BlueprintType)
struct FDamageResult
{
	GENERATED_BODY()

	// 명중 확률 (0~100). 실제 적용 시 RNG로 hit 판정.
	UPROPERTY(BlueprintReadOnly)
	float HitChance = 0.f;

	// 치명 확률 (0~100). 명중 성공 후 별도 롤.
	UPROPERTY(BlueprintReadOnly)
	float CritChance = 0.f;

	// 명중·비치명 시 피해
	UPROPERTY(BlueprintReadOnly)
	int32 NormalDamage = 0;

	// 명중·치명 시 피해 (= NormalDamage * 2, ReflectDamage 산식 기준)
	UPROPERTY(BlueprintReadOnly)
	int32 CritDamage = 0;

	// 일반 명중(비치명)으로 대상을 처치할 수 있는가 — 확정 처치 가능 여부
	UPROPERTY(BlueprintReadOnly)
	bool bCanKill = false;

	// 치명타 발동 시에만 처치 가능한가 — 운에 의존한 처치 가능 여부 (NormalDamage < hp <= CritDamage)
	UPROPERTY(BlueprintReadOnly)
	bool bCanCritKill = false;

	UPROPERTY(BlueprintReadOnly)
	ESkillType SkillType = ESkillType::Melee;
};

UCLASS()
class PW_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay() override;

	//체력 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> healthWidgetComponent;
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UHealthWidget> healthWidget;

	//스킬 정보 예측 위젯 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> skillInfoWidgetComponent;

	//무기 메시 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComp;

	//스킬 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Skill")
	TObjectPtr<USkillComponent> skillComponent;

	//버프 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Buff")
	TObjectPtr<UBuffComponent> buffComponent;
	
	UPROPERTY(VisibleAnywhere, Category = "Ailment")
	TObjectPtr<UAilmentComponent> ailmentComponent;

	//패시브 스킬 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Skill")
	TObjectPtr<UPassiveSkillComponent> passiveSkillComponent;

	//카메라 관련
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class USpringArmComponent> springArmComponent;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class UCameraComponent> cameraComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraArmLength = 1400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraPitchAngle = -55.f;

	// 캐릭터 스탯
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stat")
	int32 maxHp = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 hp = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 atk = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 speed = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 skill = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 def = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float movingPoint = 9.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float currentMovingPoint = 9.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 mentality = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 stress = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 maxStress = 100;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 actionPoint = 2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 currentActionPoint = 2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 damageReduction = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 damageAmplification = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 penetration = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	float sight = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat")
	int32 battleResource = 10;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Combat")
	float accuracy = 0.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Combat")
	float evasion = 0.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Combat")
	float critical = 0.0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Growth")
	int32 hpGrowth = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Growth")
	int32 atkGrowth = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Growth")
	int32 speedGrowth = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Growth")
	int32 skillGrowth = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Growth")
	int32 defGrowth = 50;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stat|Growth")
	int32 mentalityGrowth = 5;
	
	//AttackRangeIndicator와 오버렙 시 미리 계산해 값을 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingDamage")
	float pendingDMG;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingDamage")
	float pendingAccuracy;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingDamage")
	float pendingCritical;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PendingDamage")
	ESkillType pendingSkillType = ESkillType::Melee;

	// 가장 최근 본인을 공격한 캐릭터 — AfterDamage Reactive 디스패치 시 공격자 식별용
	TWeakObjectPtr<ACharacterBase> lastAttacker = nullptr;

	// 본인 이동 상태 — 이번 턴에 이동했는가 (실제 이동 + 인디케이터 가상 토글 통합)
	// 턴 시작 시 false로 초기화, 첫 이동 시 true로 전이
	bool isMoved = false;

	//레벨 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 level = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float exp = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float maxExp = 100;

	//캐릭터의 기본 스킬, 파생 클래스에서 구현
	virtual void SetDefaultSkills();
	//캐릭터의 기본 패시브 스킬, 파생 클래스에서 구현
	virtual void SetDefaultPassives();
	//스탯에 따른 캐릭터의 기본 명중, 치명, 회피 계산
	void SetDefaultStats();
	//레벨업
	void LevelUp();
public:
	virtual void Tick(float DeltaTime) override;

	//턴 순서 계산
	int32 GetTurnOrder() const;
	//경험치 획득
	void GetEXP(bool bIsKill);
	
	//각종 스탯의 Getter
	float GetCurrentMovingPoint() const { return currentMovingPoint; }
	float GetMovingPoint() const { return movingPoint; }
	bool IsMoved() const { return isMoved; }
	// AI 이동(MoveTo 비동기) 시 이동 시작 시점에 PathLength 기반으로 사전 차감.
	// AllyCharacterBase는 Tick에서 실제 이동거리를 측정해 차감하나, AI는 콜백 기반이라 사전 차감으로 단순화.
	void ConsumeMovingPoint(float meters);
	int32 GetHp() const { return hp; }
	int32 GetMaxHp() const { return maxHp; }
	bool IsDead() const { return hp <= 0; }
	int32 GetCurrentActionPoint() const { return currentActionPoint; }
	void ReduceActionPoint(int32 amount);
	int32 GetBattleResource() const { return battleResource; }
	void ReduceBattleResource(int32 amount);
	int32 GetAtk() const { return atk; }
	float GetAccuracy() const { return accuracy; }
	float GetCritical() const { return critical; }
	int32 GetDamageAmplification() const { return damageAmplification; }
	int32 GetPenetration() const { return penetration; }
	int32 GetDef() const { return def; }
	float GetEvasion() const { return evasion; }
	int32 GetDamageReduction() const { return damageReduction; }
	
	//AttackRangeIndicator 오버렙 시 데미지 미리 계산
	//PickTeam: Buff 스킬에서 AllyOnly면 아군 대상이라 회피를 차감하지 않음
	void CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication, int Penetration, ESkillType SkillType, EPickTeam PickTeam);

	// 순수 데미지 예측 — 상태 미변경(const). AI 후보 평가에서 (스킬 × 대상 × 위치) 다회 호출 안전.
	// 내부에서 Attacker의 BeforeDamageCalc 패시브 디스패치를 포함하나 모두 로컬 카피로 처리.
	// AttackerLocation은 위치 의존 보너스 확장 여지를 위해 인자로 유지 (현재 본문 미사용).
	FDamageResult PreviewDamage(const UActiveSkillBase* Skill,
	                            const ACharacterBase* Attacker,
	                            const FVector& AttackerLocation) const;

	//미리 계산해둔 값으로 치명타, 회피 여부 계산 후 데미지 적용
	bool ReflectDamage();

	void SetLastAttacker(ACharacterBase* Attacker);
	TWeakObjectPtr<ACharacterBase> GetLastAttacker() const { return lastAttacker; }

	// 이동 상태 전이 — true/false 변경 시 BeforeMove 패시브 revert+apply 및 스킬 재계산
	// 같은 상태로 호출 시 no-op (Tick 폭주 안전)
	void OnMoveStateChanged(bool newIsMoved);
	
	virtual void InitTurn();
	virtual void EndTurn();

	//데미지 적용 후 HealthWidget 갱신, 음수일시 회복
	//bIsLethal=false 시 hp를 1까지만 깎고 사망 처리 안 함 (환경 데미지, 상태이상 등)
	void ReceiveDamage(int32 Amount, bool bIsLethal);

	// 사망 시 호출 — 파생 클래스에서 사전 처리 후 Super 호출
	virtual void HandleDeath();

	// 사망 시 GameMode 등 외부 시스템에 통보
	FOnCharacterDeath OnCharacterDeath;

	//캡슐의 NavMesh 등록 여부 토글
	void SetNavObstacleEnabled(bool bEnabled);

	//스킬 정보 예측 위젯 표시/숨김
	void ShowSkillInfo();
	void HideSkillInfo();
	void ClearPendingDamage();
	//AI 행동 평가용: 현재 스킬 흐름에서 대상에게 계산해둔 예측값 조회
	void GetPendingSkillValues(float& OutDamage, float& OutAccuracy, float& OutCritical, ESkillType& OutSkillType) const
	{
		OutDamage = pendingDMG;
		OutAccuracy = pendingAccuracy;
		OutCritical = pendingCritical;
		OutSkillType = pendingSkillType;
	}

	UStaticMeshComponent* GetWeaponMeshComponent() const;
	USkillComponent* GetSkillComponent() const { return skillComponent; }
	UBuffComponent* GetBuffComponent() const { return buffComponent; }
	UAilmentComponent* GetAilmentComponent() const { return ailmentComponent; }
	UPassiveSkillComponent* GetPassiveSkillComponent() const { return passiveSkillComponent; }

	//버프 적용/해제 시 스탯 델타 일괄 가감 — sign: +1=적용, -1=해제
	//파생 스탯(accuracy/evasion/critical)은 SetDefaultStats를 다시 부르지 않고 직접 가감
	void ApplyBuffDelta(const UBuffBase* buff, int32 sign);

	//패시브(Stat) 등록/해제 시 스탯 델타 일괄 가감 — sign: +1=등록, -1=해제
	//ApplyBuffDelta와 동일한 가역성 원칙 (음수 sign 허용, 0 클램프 금지)
	void ApplyPassiveStatDelta(const UPassiveSkillBase* passive, int32 sign);
};
