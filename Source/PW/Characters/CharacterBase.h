// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class USkillComponent;

UCLASS()
class PW_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 체력 위젯 컴포넌트 (캐릭터 머리 위에 표시)
	// 에디터 BP의 Class Defaults에서 Widget Class를 할당해 사용
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> healthWidgetComponent;

	// 스킬 전투 예측 위젯 컴포넌트 (체력 위젯 위에 표시)
	// AttackRangeIndicator 오버랩 시 활성화
	UPROPERTY(VisibleAnywhere, Category = "UI")
	TObjectPtr<UWidgetComponent> skillInfoWidgetComponent;

	// 무기 메시 컴포넌트 (오른손 소켓에 자동 부착)
	// 에디터 BP의 Class Defaults에서 Static Mesh를 할당해 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UStaticMeshComponent> WeaponMeshComp;

	// 스킬 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Skill")
	TObjectPtr<USkillComponent> skillComponent;

	// 카메라 관련 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class USpringArmComponent> springArmComponent;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	TObjectPtr<class UCameraComponent> cameraComponent;
	// 카메라 암 길이
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera")
	float cameraArmLength = 1400.f;
	// 카메라 내려다보는 각도
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

	// 레벨 관련
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 level = 1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float exp = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	float maxExp = 100;

	virtual void SetDefaultSkills();
	void SetDefaultStats();
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetTurnOrder() const;
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
	
	void CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication, int Penetration);
	void ReflectDamage();

	void InitTurn();
	virtual void EndTurn();

	// 데미지 수신 — hp를 감소시키고 HealthWidget 갱신 (양수: 데미지, 음수: 회복)
	void ReceiveDamage(int32 Amount);

	// 캡슐의 NavMesh 등록 여부 토글 후 NavOctree 즉시 갱신
	void SetNavObstacleEnabled(bool bEnabled);

	// 스킬 전투 예측 위젯 표시/숨김
	void ShowSkillInfo();
	void HideSkillInfo();
	void ClearPendingDamage();

	UStaticMeshComponent* GetWeaponMeshComponent() const;
	USkillComponent* GetSkillComponent() const { return skillComponent; }
};