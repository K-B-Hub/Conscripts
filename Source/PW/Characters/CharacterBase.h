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

//데미지 사전 계산 결과, PreviewDamage가 반환하는 RNG 적용 전 결정론적 값
USTRUCT(BlueprintType)
struct FDamageResult
{
	GENERATED_BODY()

	//명중 확률
	UPROPERTY(BlueprintReadOnly)
	float HitChance = 0.f;

	//치명 확률
	UPROPERTY(BlueprintReadOnly)
	float CritChance = 0.f;

	//명중·비치명 시 피해
	UPROPERTY(BlueprintReadOnly)
	int32 NormalDamage = 0;

	//명중·치명 시 피해
	UPROPERTY(BlueprintReadOnly)
	int32 CritDamage = 0;

	//일반 명중으로 확정 처치 가능한가
	UPROPERTY(BlueprintReadOnly)
	bool bCanKill = false;

	//치명타 발동 시에만 처치 가능한가
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

	//캐릭터 스탯
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

	//가장 최근 본인을 공격한 캐릭터, AfterDamage Reactive 디스패치 시 공격자 식별용
	TWeakObjectPtr<ACharacterBase> lastAttacker = nullptr;

	//본인 이동 상태, 턴 시작 시 false로 초기화, 첫 이동 시 true로 전이
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
	//AI 이동 시 PathLength 기반으로 이동력 사전 차감
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
	
	//아군 여부, AAllyCharacterBase에서 true로 override
	virtual bool IsAlly() const { return false; }

	//AttackRangeIndicator 오버렙 시 데미지 미리 계산
	//Attacker: 실제 시전자, this와 IsAlly()가 같으면 아군 대상 취급(회피 차감 없이 반드시 명중)
	void CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication, int Penetration, ESkillType SkillType, const ACharacterBase* Attacker);

	//순수 데미지 예측, 상태 미변경(const)이라 AI 후보 평가에서 다회 호출 안전
	FDamageResult PreviewDamage(const UActiveSkillBase* Skill,
	                            const ACharacterBase* Attacker,
	                            const FVector& AttackerLocation) const;

	//미리 계산해둔 값으로 치명타, 회피 여부 계산 후 데미지 적용
	bool ReflectDamage();

	void SetLastAttacker(ACharacterBase* Attacker);
	TWeakObjectPtr<ACharacterBase> GetLastAttacker() const { return lastAttacker; }

	//이동 상태 전이, 변경 시 BeforeMove 패시브 revert+apply 및 스킬 재계산
	void OnMoveStateChanged(bool newIsMoved);
	
	virtual void InitTurn();
	virtual void EndTurn();

	//데미지 적용 후 HealthWidget 갱신, 음수일시 회복
	//bIsLethal=false 시 hp를 1까지만 깎고 사망 처리 안 함 (환경 데미지, 상태이상 등)
	void ReceiveDamage(int32 Amount, bool bIsLethal);

	//사망 시 호출, 파생 클래스에서 사전 처리 후 Super 호출
	virtual void HandleDeath();

	//사망 시 GameMode 등 외부 시스템에 통보
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
