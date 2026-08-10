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
class USkillBase;
class UTerrainComponent;
class ATerrainBase;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ACharacterBase*, DeadCharacter);
//hp/스트레스 변동 시 HUD 게이지 등 외부 통지
DECLARE_MULTICAST_DELEGATE(FOnVitalsChanged);

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

	//지형 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Terrain")
	TObjectPtr<UTerrainComponent> terrainComponent;

	//캐릭터 스탯
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 maxHp = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 hp = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 atk = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 speed = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 skill = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 def = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	float movingPoint = 9.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	float currentMovingPoint = 9.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 mentality = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 stress = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 maxStress = 100;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 actionPoint = 2;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stat")
	int32 currentActionPoint = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 damageReduction = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 damageAmplification = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 penetration = 0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	float sight = 15;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat")
	int32 battleResource = 10;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Combat")
	float accuracy = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Combat")
	float evasion = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Combat")
	float critical = 0.0;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Growth")
	int32 hpGrowth = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Growth")
	int32 atkGrowth = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Growth")
	int32 speedGrowth = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Growth")
	int32 skillGrowth = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Growth")
	int32 defGrowth = 50;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Stat|Growth")
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

	//실제 이동력 소모 여부, 턴 시작 시 리셋
	//isMoved는 인디케이터 미리보기·AI 후보 평가가 일시적으로 켰다 끄므로 "실제로 이동했는가"의 근거가 못 됨
	bool hasConsumedMovingPoint = false;

	//포복 자세, 행동력 1로 진입하는 토글 상태 (일어서기는 무료), 턴이 넘어가도 유지
	bool bIsProne = false;

	//서있는 자세 기준 이동 속도, BeginPlay에서 캐싱 후 포복 배율 적용의 기준값
	float baseWalkSpeed = 0.f;

	//아치 궤적 폴리라인 및 현재 인덱스, 점프·밀치기 공용
	TArray<FVector> arcPoints;
	int32 arcIndex = 0;
	//아치 궤적 추종 중 여부, IsAirborne의 근거
	bool bIsArcMoving = false;
	//외력에 의한 아치 이동 여부, true면 이동 패시브 훅·완료 통지 생략, 착지 시 낙하 피해
	bool bIsArcForced = false;

	//아치 궤적 추종 속도(cm/s)
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float jumpSpeed = 800.f;

	//착지 몽타주, 아치 이동(점프·밀치기) 완료 시 재생 — 미지정 시 생략
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	TObjectPtr<UAnimMontage> landMontage;

	//낙하 피해 면제 높이(cm), 순수 하강분이 이를 초과하면 피해
	UPROPERTY(EditDefaultsOnly, Category = "Movement|FallDamage")
	float fallDamageThreshold = 200.f;
	//면제 초과 1m당 낙하 피해
	UPROPERTY(EditDefaultsOnly, Category = "Movement|FallDamage")
	int32 fallDamagePerMeter = 1;

	//아치 궤적 한 프레임 진행, 완료 시 착지 처리
	void UpdateArcMovement(float DeltaTime);
	//아치 이동 자연 완료(비강제) 시 호출, 아군은 OnMovementCompleted 브로드캐스트로 확장
	virtual void NotifyArcMoveCompleted() {}

	//레벨 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 level = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	float exp = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float maxExp = 100;

	//maxStress 도달 시 발동, 스트레스 0 초기화 후 호출됨 (20% 긍정 버프 / 80% 부정 버프·상태이상, 추후 구현)
	void OnStressOverflow();
	//체력 비율 경계(50%/30%) 하향 통과 시 스트레스 부여
	void ApplyHpThresholdStress(int32 hpBefore);

	//캐릭터의 기본 스킬, 파생 클래스에서 구현
	virtual void SetDefaultSkills();
	//캐릭터의 기본 패시브 스킬, 파생 클래스에서 구현
	virtual void SetDefaultPassives();
	//파생 스탯(명중/회피/치명) 재계산, 기본 공식 + 버프/패시브 보너스 가산
	void SetDefaultStats();
	//레벨업
	void LevelUp();
	//레벨업 시 강화효과 처리, 파생 클래스에서 구현(아군=선택 큐, 적=자동 습득)
	virtual void GrantLevelUpUpgrade() {}
public:
	virtual void Tick(float DeltaTime) override;

	//턴 순서 계산
	int32 GetTurnOrder() const;
	//경험치 누적 및 레벨업, 획득량 계산은 호출자(BattleGameMode 등) 책임
	void GainExp(float amount);

	//강화효과 습득, 액티브/패시브 계열을 판별해 각 컴포넌트에 등록
	void AcquireUpgrade(TSubclassOf<USkillBase> skillClass);

	//목표 레벨까지 반복 레벨업, 레벨당 강화 훅 발동 (적 초기 스케일링용)
	void ForceLevelUpTo(int32 targetLevel);
	
	//각종 스탯의 Getter
	float GetCurrentMovingPoint() const { return currentMovingPoint; }
	float GetMovingPoint() const { return movingPoint; }
	bool IsMoved() const { return isMoved; }
	//실제 이동력을 소모했는가, 투기적 isMoved 토글을 되돌릴 목적지
	bool HasConsumedMovingPoint() const { return hasConsumedMovingPoint; }

	//서있는 자세 기준 시야 눈높이(cm), LOS 매직넘버의 단일 출처·지면 타겟 기본값
	static constexpr float StandingEyeHeightZ = 80.f;
	//포복 자세 시야 눈높이(cm), 낮을수록 낮은 엄폐물로도 은폐 (튜닝값)
	static constexpr float ProneEyeHeightZ = 35.f;

	bool IsProne() const { return bIsProne; }
	//포복 상태 명시 설정(멱등), 이동 속도 배율 반영
	void SetProne(bool bNewProne);
	//아치 궤적(점프·밀치기) 추종 중 여부
	bool IsAirborne() const { return bIsArcMoving; }
	//포물선 궤적을 3D로 따라 이동, bForced=외력: 이동 패시브 훅·완료 통지 생략, 착지 시 낙하 피해
	virtual void MoveAlongArc(const TArray<FVector>& ArcPoints, bool bForced = false);
	//원점→대상 수평 일직선 방향으로 밀쳐냄, 궤적 계산·포복 강제 해제 포함
	void ApplyKnockback(const FVector& origin, float force, float angleDeg);
	//LOS 라인트레이스 양 끝점에 쓰는 자세별 눈높이(cm)
	float GetEyeHeightZ() const { return bIsProne ? ProneEyeHeightZ : StandingEyeHeightZ; }
	//포복 시 이동력 소모 배율, 지형 배율과 별개로 곱함
	float GetStanceMoveCostMultiplier() const { return bIsProne ? 2.f : 1.f; }
	//포복 시 실제 이동 속도 배율, 이동력 소모 배율과 별개
	static constexpr float ProneMoveSpeedMultiplier = 0.5f;
	//포복 자세 토글 및 이동 속도 반영, 행동력 소모는 스킬 계층(ProneSkill)에서 처리
	void ToggleProne();
	//이동력 차감, 지형 배율은 호출자가 미리 반영해서 넘김
	void ConsumeMovingPoint(float meters);
	//이동력 증가(달리기 등), 상한 클램프 없음 — 기본 최대치를 초과해 얻을 수 있음
	void GainMovingPoint(float meters);
	//현재 체류 지형의 이동력 소모 배율, 지형 밖이면 1.0
	float GetTerrainMoveCostMultiplier() const;
	int32 GetLevel() const { return level; }
	int32 GetHp() const { return hp; }
	int32 GetMaxHp() const { return maxHp; }
	int32 GetStress() const { return stress; }
	int32 GetMaxStress() const { return maxStress; }
	bool IsDead() const { return hp <= 0; }
	int32 GetCurrentActionPoint() const { return currentActionPoint; }
	void ReduceActionPoint(int32 amount);
	int32 GetBattleResource() const { return battleResource; }
	void ReduceBattleResource(int32 amount);
	int32 GetAtk() const { return atk; }
	int32 GetSpeed() const { return speed; }
	int32 GetSkill() const { return skill; }
	float GetSight() const { return sight; }
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
	//적은 override로 비전투 피격 시 전투 전환
	virtual void ReceiveDamage(int32 Amount, bool bIsLethal);

	//정신력 기반 스트레스 경감률(%), 50 * (1 - e^(-0.06 * mentality)), 최대 50 수렴
	float GetStressMitigation() const;
	//스트레스 획득, 경감률 적용·반올림 후 가산, maxStress 도달 시 0 초기화 후 OnStressOverflow 발동
	void ReceiveStress(int32 Amount);
	//스트레스 감소, 경감률 미적용, 0 미만 클램프
	void RelieveStress(int32 Amount);

	//사망 시 호출, 파생 클래스에서 사전 처리 후 Super 호출
	virtual void HandleDeath();

	//사망 시 GameMode 등 외부 시스템에 통보
	FOnCharacterDeath OnCharacterDeath;

	//hp/스트레스 변동 시 통보, HUD 게이지 갱신용
	FOnVitalsChanged OnVitalsChanged;

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
	UTerrainComponent* GetTerrainComponent() const { return terrainComponent; }

	//버프 적용/해제 시 스탯 델타 일괄 가감 — sign: +1=적용, -1=해제
	//파생 스탯(accuracy/evasion/critical)은 BuffComponent 보너스에 저장 후 재계산으로 반영
	void ApplyBuffDelta(const UBuffBase* buff, int32 sign);

	//패시브(Stat) 등록/해제 시 스탯 델타 일괄 가감 — sign: +1=등록, -1=해제
	//ApplyBuffDelta와 동일한 가역성 원칙 (음수 sign 허용, 0 클램프 금지)
	void ApplyPassiveStatDelta(const UPassiveSkillBase* passive, int32 sign);

	//지형 진입/이탈 시 스탯 델타 일괄 가감 — sign: +1=진입, -1=이탈
	//ApplyBuffDelta와 동일한 가역성 원칙
	void ApplyTerrainStatDelta(const ATerrainBase* terrain, int32 sign);
};
