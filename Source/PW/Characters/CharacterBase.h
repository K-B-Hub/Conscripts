// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UHealthWidget;
class UStaticMeshComponent;
class UWidgetComponent;
class USkillComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ACharacterBase*, DeadCharacter);

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

	//레벨 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 level = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float exp = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float maxExp = 100;

	//캐릭터의 기본 스킬, 파생 클래스에서 구현
	virtual void SetDefaultSkills();
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
	int32 GetHp() const { return hp; }
	int32 GetMaxHp() const { return maxHp; }
	int32 GetCurrentActionPoint() const { return currentActionPoint; }
	void ReduceActionPoint(int32 amount);
	int32 GetBattleResource() const { return battleResource; }
	void ReduceBattleResource(int32 amount);
	int32 GetAtk() const { return atk; }
	float GetAccuracy() const { return accuracy; }
	float GetCritical() const { return critical; }
	int32 GetDamageAmplification() const { return damageAmplification; }
	int32 GetPenetration() const { return penetration; }
	
	//AttackRangeIndicator 오버렙 시 데미지 미리 계산
	void CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication, int Penetration);
	//미리 계산해둔 값으로 치명타, 회피 여부 계산 후 데미지 적용
	void ReflectDamage();
	
	void InitTurn();
	virtual void EndTurn();

	//데미지 적용 후 HealthWidget 갱신, 음수일시 회복
	void ReceiveDamage(int32 Amount);

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

	UStaticMeshComponent* GetWeaponMeshComponent() const;
	USkillComponent* GetSkillComponent() const { return skillComponent; }
};