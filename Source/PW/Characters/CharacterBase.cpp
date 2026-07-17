// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "NavigationSystem.h"
#include "ActorComponent/AilmentComponent.h"
#include "ActorComponent/PassiveSkillComponent.h"
#include "Object/Skill/PassiveSkillBase.h"
#include "Object/Skill/ActiveSkillBase.h"
#include "Widget/HealthWidget.h"
#include "Widget/SkillInfoWidget.h"
#include "ActorComponent/SkillComponent.h"
#include "ActorComponent/BuffComponent.h"
#include "Object/Buff/BuffBase.h"
#include "GameMode/BattleGameMode.h"

//데미지 산식의 단일 출처, CalculateDamage와 PreviewDamage가 공유
static void ComputeDamageNumbers(const ACharacterBase* Target,
	float Damage, float Accuracy, float Critical, int32 DamageAmplfication, int32 Penetration,
	ESkillType SkillType, bool bAllyTarget,
	float& OutDmg, float& OutAccuracy, float& OutCritical)
{
	if (!Target)
	{
		OutDmg = 0.f; OutAccuracy = 0.f; OutCritical = 0.f;
		return;
	}

	if (SkillType == ESkillType::Heal)
	{
		//음수 데미지로 회복 처리, 치명타 미적용
		OutDmg = -(Damage * (1 + DamageAmplfication / 100.f));
		//아군 대상이면 회피 무시하고 반드시 명중
		OutAccuracy = bAllyTarget ? 100.f : FMath::Clamp(Accuracy - Target->GetEvasion(), 0.f, 100.f);
		OutCritical = 0.f;
		return;
	}
	if (SkillType == ESkillType::Buff)
	{
		OutDmg = 0.f;
		//아군 대상이면 회피 무시하고 반드시 명중
		OutAccuracy = bAllyTarget ? 100.f : FMath::Clamp(Accuracy - Target->GetEvasion(), 0.f, 100.f);
		OutCritical = 0.f;
		return;
	}
	if (SkillType == ESkillType::Ailment)
	{
		OutDmg = 0.f;
		OutAccuracy = FMath::Clamp(Accuracy - Target->GetEvasion(), 0.f, 100.f);
		OutCritical = 0.f;
		return;
	}

	OutDmg = (Damage * (1 + DamageAmplfication / 100.f) - Target->GetDef() * (1 - Penetration / 100.f))
	         * (1 - Target->GetDamageReduction() / 100.f);
	if (OutDmg <= 0.f) OutDmg = 0.f;
	OutAccuracy = FMath::Clamp(Accuracy - Target->GetEvasion(), 0.f, 100.f);
	OutCritical = Critical;
}

ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	healthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthWidget"));
	healthWidgetComponent->SetupAttachment(RootComponent);
	healthWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 120.f)); //캡슐 상단 위
	healthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	healthWidgetComponent->SetDrawSize(FVector2D(150.f, 20.f));

	skillInfoWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SkillInfoWidget"));
	skillInfoWidgetComponent->SetupAttachment(RootComponent);
	skillInfoWidgetComponent->SetRelativeLocation(FVector(0.f, 0.f, 90.f)); //캐릭터 중앙 부근을 앵커로
	skillInfoWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	//TODO: Pivot을 조정해 앵커 기준 오른편 아래로 위젯이 펼쳐지게 한다 (기본 0.5,0.5 = 중앙)
	skillInfoWidgetComponent->SetPivot(FVector2D(1, -1));
	skillInfoWidgetComponent->SetDrawSize(FVector2D(200.f, 60.f));
	skillInfoWidgetComponent->SetVisibility(false);

	WeaponMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComp->SetupAttachment(GetMesh(), FName("WeaponSocket_R"));
	WeaponMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	skillComponent = CreateDefaultSubobject<USkillComponent>(TEXT("SkillComponent"));
	if (skillComponent)
	{
		skillComponent->SetOwner(this);
	}

	buffComponent = CreateDefaultSubobject<UBuffComponent>(TEXT("BuffComponent"));
	ailmentComponent = CreateDefaultSubobject<UAilmentComponent>(TEXT("AilmentComponent"));
	passiveSkillComponent = CreateDefaultSubobject<UPassiveSkillComponent>(TEXT("PassiveSkillComponent"));

	GetCapsuleComponent()->SetCanEverAffectNavigation(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);

	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	//컨트롤러는 카메라 폰만 빙의, 빙의 없이도 AddMovementInput 이동 허용
	GetCharacterMovement()->bRunPhysicsWithNoController = true;
}

void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->InitHealth(maxHp, hp);
		healthWidget = HealthWidget;
	}
	SetDefaultStats();
	SetDefaultSkills();
	//패시브 등록, 파생 스탯 보너스는 컴포넌트에 저장되어 재계산에도 보존
	SetDefaultPassives();
}

void ACharacterBase::SetDefaultSkills()
{
	//파생 클래스에서 구현
}

void ACharacterBase::SetDefaultPassives()
{
	//파생 클래스에서 구현
}

void ACharacterBase::SetDefaultStats()
{
	//기본 공식으로 계산 후 버프/패시브 보너스 가산, 언제 재계산해도 보너스 보존
	accuracy = skill * 1.2;
	evasion = speed * 1.2;
	critical = skill * 0.5;
	if (buffComponent)
	{
		accuracy += buffComponent->GetAccuracyBonus();
		evasion += buffComponent->GetEvasionBonus();
		critical += buffComponent->GetCriticalBonus();
	}
	if (passiveSkillComponent)
	{
		accuracy += passiveSkillComponent->GetAccuracyBonus();
		evasion += passiveSkillComponent->GetEvasionBonus();
		critical += passiveSkillComponent->GetCriticalBonus();
	}
	if (skillComponent)
	{
		skillComponent->CalcSkillStats();
	}
}

void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

int32 ACharacterBase::GetTurnOrder() const
{
	return speed + FMath::RandRange(0, speed);
}

void ACharacterBase::GetEXP(bool bIsKill)
{
	if (bIsKill)
	{
		exp += 55;			//추후 경험치 공식 적용 필요
	}
	else
	{
		exp += 20;
	}
	if (exp >= maxExp)
	{
		while (exp >= maxExp)
		{
			LevelUp();
			exp -= 100;
		}
	}
}

void ACharacterBase::LevelUp()
{
	level++;

	if (FMath::RandRange(1, 100) <= hpGrowth)
	{
		maxHp++; hp++;
		if (IsValid(healthWidget))
			healthWidget->InitHealth(maxHp, hp);
	}
	if (FMath::RandRange(1, 100) <= atkGrowth)       { atk++; }
	if (FMath::RandRange(1, 100) <= speedGrowth)     { speed++; }
	if (FMath::RandRange(1, 100) <= skillGrowth)     { skill++; }
	if (FMath::RandRange(1, 100) <= defGrowth)       { def++; }
	if (FMath::RandRange(1, 100) <= mentalityGrowth) { mentality++; }

	//파생 스탯 재계산
	SetDefaultStats();

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 레벨업 → Lv.%d"), *GetName(), level);
}

void ACharacterBase::ReduceActionPoint(int32 amount)
{
	currentActionPoint = FMath::Clamp(currentActionPoint - amount, 0, actionPoint);
}

void ACharacterBase::ReduceBattleResource(int32 amount)
{
	battleResource = FMath::Clamp(battleResource - amount, 0, battleResource);
}

void ACharacterBase::ConsumeMovingPoint(float meters)
{
	currentMovingPoint = FMath::Max(0.f, currentMovingPoint - meters);
}

void ACharacterBase::CalculateDamage(float Damage, float Accuracy, float Critical, int32 DamageAmplfication,
                                     int Penetration, ESkillType SkillType, const ACharacterBase* Attacker)
{
	//공용 헬퍼 호출, 결과를 pending* 멤버에 기입 (인디케이터 위젯·ReflectDamage 경로)
	pendingSkillType = SkillType;
	const bool bAllyTarget = Attacker && (Attacker->IsAlly() == IsAlly());
	ComputeDamageNumbers(this, Damage, Accuracy, Critical, DamageAmplfication, Penetration,
		SkillType, bAllyTarget, pendingDMG, pendingAccuracy, pendingCritical);
}

FDamageResult ACharacterBase::PreviewDamage(const UActiveSkillBase* Skill,
                                            const ACharacterBase* Attacker,
                                            const FVector& AttackerLocation) const
{
	FDamageResult result;
	if (!Skill || !Attacker) return result;

	result.SkillType = Skill->skillType;

	//스킬 calc 값 로컬 카피, 외부 상태 미변경 보장
	float dmg  = Skill->calcDamage;
	int32 amp  = Skill->calcDamageAmplfication;
	int32 pen  = Skill->calcPenetration;
	float acc  = Skill->calcAccuracy;
	float crit = Skill->calcCritical;

	//캐스터 측 BeforeDamageCalc 패시브 보너스 합산
	if (UPassiveSkillComponent* PSC = Attacker->GetPassiveSkillComponent())
	{
		PSC->DispatchBeforeDamageCalc(const_cast<ACharacterBase*>(this),
			Skill->skillType, Skill->damageType, dmg, amp, pen, acc, crit);
	}

	const bool bAllyTarget = Attacker->IsAlly() == IsAlly();
	float outDmg = 0.f, outAcc = 0.f, outCrit = 0.f;
	ComputeDamageNumbers(this, dmg, acc, crit, amp, pen, Skill->skillType, bAllyTarget,
		outDmg, outAcc, outCrit);

	result.HitChance   = outAcc;
	result.CritChance  = outCrit;
	result.NormalDamage = FMath::RoundToInt(outDmg);
	result.CritDamage   = FMath::RoundToInt(outDmg * 2.f);   //ReflectDamage 산식과 일치
	//두 단계 처치 가능성, AI는 확정 처치와 운빨 처치를 다르게 가중
	result.bCanKill     = (result.NormalDamage >= hp);
	result.bCanCritKill = !result.bCanKill && (result.CritDamage >= hp);

	return result;
}

bool ACharacterBase::ReflectDamage()
{
	if (pendingSkillType == ESkillType::Buff || pendingSkillType == ESkillType::Ailment || pendingSkillType == ESkillType::Heal)
	{
		float hitRoll = FMath::FRandRange(0.f, 100.f);
		if (hitRoll > pendingAccuracy)
		{
			UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 버프 or 상태이상 회피"), *GetName());
			//버프/상태이상이 빗나가도 공격자가 스트레스 1~3 획득
			if (ACharacterBase* attacker = lastAttacker.Get())
			{
				attacker->ReceiveStress(FMath::RandRange(1, 3));
			}
			return false;
		}
		//치명타 굴림 없이 음수 데미지(pendingDMG)로 즉시 회복
		if (pendingSkillType == ESkillType::Heal)
		{
			ReceiveDamage(FMath::RoundToInt(pendingDMG), true);
		}
		return true;
	}

	float hitRoll = FMath::FRandRange(0.f, 100.f);
	if (hitRoll > pendingAccuracy)
	{
		UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 공격 회피"), *GetName());
		//회피자는 스트레스 3~5 감소
		RelieveStress(FMath::RandRange(3, 5));
		//공격이 빗나가면 공격자가 스트레스 1~3 획득
		if (ACharacterBase* attacker = lastAttacker.Get())
		{
			attacker->ReceiveStress(FMath::RandRange(1, 3));
		}
		return false;
	}
	float critRoll = FMath::FRandRange(0.f, 100.f);
	const bool bCrit = critRoll <= pendingCritical;
	int32 finalDamage = bCrit
		? FMath::RoundToInt(pendingDMG * 2.f)
		: FMath::RoundToInt(pendingDMG);

	//치명타 피격 스트레스 5~10, 사망(Destroy) 전에 적용되도록 데미지보다 먼저 부여
	if (bCrit)
	{
		ReceiveStress(FMath::RandRange(5, 10));
		//치명타를 맞춘 공격자는 스트레스 4~8 감소
		if (ACharacterBase* attacker = lastAttacker.Get())
		{
			attacker->RelieveStress(FMath::RandRange(4, 8));
		}
	}

	ReceiveDamage(finalDamage, true);

	return true;
}

void ACharacterBase::SetLastAttacker(ACharacterBase* Attacker)
{
	if (IsValid(Attacker))
	{
		lastAttacker = Attacker;
	}
}

void ACharacterBase::OnMoveStateChanged(bool newIsMoved)
{
	if (isMoved == newIsMoved) return;

	//이전 상태 보너스 revert, isMoved 갱신, 새 상태 보너스 apply 순서로 처리
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchBeforeMove(isMoved, -1);
	}
	isMoved = newIsMoved;
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchBeforeMove(isMoved, +1);
	}
	if (skillComponent)
	{
		skillComponent->CalcSkillStats();
	}
}

void ACharacterBase::InitTurn()
{
	currentActionPoint = actionPoint;
	currentMovingPoint = movingPoint;

	//이전 턴 이동 상태를 false로 전이
	OnMoveStateChanged(false);

	if (buffComponent)
	{
		buffComponent->OnTurnStart();
	}
	if (ailmentComponent)
	{
		ailmentComponent->OnTurnStart();
	}

	//턴 시작 Conditional 패시브, AP/이동력 리셋 이후 확정 상태 기준으로 평가
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchConditional(EConditionalType::TurnStart);
	}
}

void ACharacterBase::EndTurn()
{
	//턴 종료 Conditional 패시브, 버프/상태이상 차감 전 상태에서 평가
	if (passiveSkillComponent)
	{
		passiveSkillComponent->DispatchConditional(EConditionalType::TurnEnd);
	}

	if (buffComponent)
	{
		buffComponent->OnTurnEnd();
	}
	if (ailmentComponent)
	{
		ailmentComponent->OnTurnEnd();
	}
}

void ACharacterBase::ApplyBuffDelta(const UBuffBase* buff, int32 sign)
{
	if (!buff || (sign != 1 && sign != -1)) return;

	//최대 체력 변경, 현재 체력은 새 max에 클램프
	maxHp += buff->hp * sign;
	if (maxHp <= 0) maxHp = 1;
	hp = FMath::Clamp(hp, 1, maxHp);
	if (healthWidget)
	{
		healthWidget->InitHealth(maxHp, hp);
	}

	//기본 능력치
	atk += buff->atk * sign;
	speed += buff->speed * sign;
	skill += buff->skill * sign;
	def += buff->def * sign;
	movingPoint += buff->movingPoint * sign;
	currentMovingPoint += buff->movingPoint * sign;
	currentMovingPoint = FMath::Clamp(currentMovingPoint, 0, movingPoint);
	mentality += buff->mentality * sign;

	//행동력 최대치 변경, 현재치는 새 max에 클램프
	actionPoint += buff->actionPoint * sign;
	currentActionPoint += buff->actionPoint * sign;
	currentActionPoint = FMath::Clamp(currentActionPoint, 0, actionPoint);

	//피해 보정
	damageReduction += buff->damageReduction * sign;
	damageAmplification += buff->damageAmplification * sign;
	penetration += buff->penetration * sign;
	sight += buff->sight * sign;

	//전투 파생 스탯은 직접 가감하지 않고 BuffComponent 보너스에 저장, 재계산 시 가산
	if (buffComponent)
	{
		buffComponent->AddCombatStatBonus(buff->accuracy * sign, buff->evasion * sign, buff->critical * sign);
	}

	//파생 스탯 재계산(보너스 포함), 사망 상태에서는 owner가 stale weak ptr이라 스킵
	if (!IsDead())
	{
		SetDefaultStats();
	}
}

void ACharacterBase::ApplyPassiveStatDelta(const UPassiveSkillBase* passive, int32 sign)
{
	if (!passive || (sign != 1 && sign != -1)) return;

	UE_LOG(LogTemp, Log, TEXT("[Passive] %s 패시브 '%s' 적용 시작 (sign=%d)"),
		*GetName(),
		*passive->GetClass()->GetName(),
		sign);

	//적용 전 스냅샷
	const int32 beforeMaxHp = maxHp;
	const int32 beforeHp = hp;
	const int32 beforeAtk = atk;
	const int32 beforeSpeed = speed;
	const int32 beforeSkill = skill;
	const int32 beforeDef = def;
	const float beforeMovingPoint = movingPoint;
	const int32 beforeMentality = mentality;
	const int32 beforeActionPoint = actionPoint;
	const int32 beforeDamageReduction = damageReduction;
	const int32 beforeDamageAmplification = damageAmplification;
	const int32 beforePenetration = penetration;
	const float beforeSight = sight;
	const float beforeAccuracy = accuracy;
	const float beforeEvasion = evasion;
	const float beforeCritical = critical;

	//최대 HP 증가량만큼 현재 HP도 함께 증가, 패시브는 영구이므로 등록 시점이 곧 성장
	const int32 hpDelta = passive->hp * sign;
	maxHp += hpDelta;
	if (maxHp <= 0) maxHp = 1;
	hp += hpDelta;
	hp = FMath::Clamp(hp, 1, maxHp);
	if (healthWidget)
	{
		healthWidget->InitHealth(maxHp, hp);
	}

	//기본 능력치
	atk += passive->atk * sign;
	speed += passive->speed * sign;
	skill += passive->skill * sign;
	def += passive->def * sign;
	movingPoint += passive->movingPoint * sign;
	currentMovingPoint += passive->movingPoint * sign;
	currentMovingPoint = FMath::Clamp(currentMovingPoint, 0, movingPoint);
	mentality += passive->mentality * sign;

	//행동력 최대치 변경, 현재치도 같이 갱신 후 클램프
	actionPoint += passive->actionPoint * sign;
	currentActionPoint += passive->actionPoint * sign;
	currentActionPoint = FMath::Clamp(currentActionPoint, 0, actionPoint);

	//피해 보정
	damageReduction += passive->damageReduction * sign;
	damageAmplification += passive->damageAmplification * sign;
	penetration += passive->penetration * sign;
	sight += passive->sight * sign;

	//전투 파생 스탯은 직접 가감하지 않고 PassiveSkillComponent 보너스에 저장, 재계산 시 가산
	if (passiveSkillComponent)
	{
		passiveSkillComponent->AddCombatStatBonus(passive->accuracy * sign, passive->evasion * sign, passive->critical * sign);
	}

	//파생 스탯 재계산(보너스 포함), 사망 상태에서는 owner가 stale weak ptr이라 스킵
	if (!IsDead())
	{
		SetDefaultStats();
	}

	//변경된 스탯만 로그 출력
	if (beforeMaxHp != maxHp || beforeHp != hp)
		UE_LOG(LogTemp, Log, TEXT("  HP: %d/%d -> %d/%d"), beforeHp, beforeMaxHp, hp, maxHp);
	if (beforeAtk != atk)
		UE_LOG(LogTemp, Log, TEXT("  atk: %d -> %d"), beforeAtk, atk);
	if (beforeSpeed != speed)
		UE_LOG(LogTemp, Log, TEXT("  speed: %d -> %d"), beforeSpeed, speed);
	if (beforeSkill != skill)
		UE_LOG(LogTemp, Log, TEXT("  skill: %d -> %d"), beforeSkill, skill);
	if (beforeDef != def)
		UE_LOG(LogTemp, Log, TEXT("  def: %d -> %d"), beforeDef, def);
	if (!FMath::IsNearlyEqual(beforeMovingPoint, movingPoint))
		UE_LOG(LogTemp, Log, TEXT("  movingPoint: %.2f -> %.2f"), beforeMovingPoint, movingPoint);
	if (beforeMentality != mentality)
		UE_LOG(LogTemp, Log, TEXT("  mentality: %d -> %d"), beforeMentality, mentality);
	if (beforeActionPoint != actionPoint)
		UE_LOG(LogTemp, Log, TEXT("  actionPoint: %d -> %d"), beforeActionPoint, actionPoint);
	if (beforeDamageReduction != damageReduction)
		UE_LOG(LogTemp, Log, TEXT("  damageReduction: %d -> %d"), beforeDamageReduction, damageReduction);
	if (beforeDamageAmplification != damageAmplification)
		UE_LOG(LogTemp, Log, TEXT("  damageAmplification: %d -> %d"), beforeDamageAmplification, damageAmplification);
	if (beforePenetration != penetration)
		UE_LOG(LogTemp, Log, TEXT("  penetration: %d -> %d"), beforePenetration, penetration);
	if (!FMath::IsNearlyEqual(beforeSight, sight))
		UE_LOG(LogTemp, Log, TEXT("  sight: %.2f -> %.2f"), beforeSight, sight);
	if (!FMath::IsNearlyEqual(beforeAccuracy, accuracy))
		UE_LOG(LogTemp, Log, TEXT("  accuracy: %.2f -> %.2f"), beforeAccuracy, accuracy);
	if (!FMath::IsNearlyEqual(beforeEvasion, evasion))
		UE_LOG(LogTemp, Log, TEXT("  evasion: %.2f -> %.2f"), beforeEvasion, evasion);
	if (!FMath::IsNearlyEqual(beforeCritical, critical))
		UE_LOG(LogTemp, Log, TEXT("  critical: %.2f -> %.2f"), beforeCritical, critical);
}

void ACharacterBase::ReceiveDamage(int32 Amount, bool bIsLethal)
{
	if (IsDead()) return;

	const int32 hpBefore = hp;
	const int32 minHp = bIsLethal ? 0 : 1;
	hp = FMath::Clamp(hp - Amount, minHp, maxHp);

	if (UHealthWidget* HealthWidget = Cast<UHealthWidget>(healthWidgetComponent->GetWidget()))
	{
		HealthWidget->ApplyDamage(Amount);
	}

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 데미지 %d 수신 → 잔여 체력 %d / %d"), *GetName(), Amount, hp, maxHp);

	//HUD 게이지 갱신 통지
	if (hp != hpBefore)
	{
		OnVitalsChanged.Broadcast();
	}

	//체력 비율 경계 하향 통과 스트레스, 사망 시 ReceiveStress 내부 가드로 무시됨
	if (hp < hpBefore)
	{
		ApplyHpThresholdStress(hpBefore);
	}
	//회복 시 스트레스 감소, 100% 도달 시 10~15, 그 외 5~10
	else if (hp > hpBefore)
	{
		RelieveStress(hp == maxHp ? FMath::RandRange(10, 15) : FMath::RandRange(5, 10));
	}

	//Damaged Conditional, hp 변화가 있었을 때 발동(회복 포함), 사망 처리 전
	if (hp != hpBefore && passiveSkillComponent)
	{
		passiveSkillComponent->DispatchConditional(EConditionalType::Damaged);
	}

	//공격자 측 AfterDamage 패시브, 사망 처리 전에 발동시켜 AfterSlay와 순서 보장
	//스킬 적중에 의한 치명 피해일 때만 발동, 회복이나 비치명 데미지는 제외
	if (bIsLethal && Amount > 0)
	{
		if (ACharacterBase* attacker = lastAttacker.Get())
		{
			if (UPassiveSkillComponent* attackerPSC = attacker->GetPassiveSkillComponent())
			{
				attackerPSC->DispatchAfterDamage(this);
			}
		}
	}

	if (hp <= 0)
	{
		HandleDeath();
	}
}

float ACharacterBase::GetStressMitigation() const
{
	//정신력 스트레스 경감 곡선, 정신력 0=0%, 35≈50%, 최대 50% 수렴
	return 50.f * (1.f - FMath::Exp(-0.06f * mentality));
}

void ACharacterBase::ReceiveStress(int32 Amount)
{
	//스트레스는 플레이어(아군) 캐릭터만 받음
	if (!IsAlly() || IsDead() || Amount <= 0) return;

	//경감률 적용 후 반올림 (예: 정신력 5, 12% 경감 → 10 * 0.88 = 8.8 → 9)
	const int32 finalAmount = FMath::RoundToInt(Amount * (1.f - GetStressMitigation() / 100.f));
	if (finalAmount <= 0) return;

	stress += finalAmount;

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 스트레스 %d 수신(원본 %d, 경감 %.1f%%) → %d / %d"),
		*GetName(), finalAmount, Amount, GetStressMitigation(), stress, maxStress);

	//maxStress 도달 시 0으로 초기화 후 스트레스 이벤트 발동
	const bool bOverflow = stress >= maxStress;
	if (bOverflow)
	{
		stress = 0;
	}

	//HUD 게이지 갱신 통지
	OnVitalsChanged.Broadcast();

	if (bOverflow)
	{
		OnStressOverflow();
	}
}

void ACharacterBase::RelieveStress(int32 Amount)
{
	//스트레스는 플레이어(아군) 캐릭터만 보유
	if (!IsAlly() || IsDead() || Amount <= 0) return;

	const int32 stressBefore = stress;
	stress = FMath::Max(0, stress - Amount);
	if (stress == stressBefore) return;

	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 스트레스 %d 감소 → %d / %d"),
		*GetName(), Amount, stress, maxStress);

	//HUD 게이지 갱신 통지
	OnVitalsChanged.Broadcast();
}

void ACharacterBase::OnStressOverflow()
{
	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 스트레스 한계 도달, 스트레스 이벤트 발동"), *GetName());

	//게임모드의 이벤트 풀에서 20% 긍정 / 80% 부정 이벤트 적용
	if (ABattleGameMode* gameMode = GetWorld()->GetAuthGameMode<ABattleGameMode>())
	{
		gameMode->ApplyStressEvent(this);
	}
}

void ACharacterBase::ApplyHpThresholdStress(int32 hpBefore)
{
	//경계 하향 통과 판정, 두 경계 동시 통과 시 깊은 쪽(30%)만 발동
	if (hpBefore * 10 > maxHp * 3 && hp * 10 <= maxHp * 3)
	{
		ReceiveStress(FMath::RandRange(20, 30));
	}
	else if (hpBefore * 2 > maxHp && hp * 2 <= maxHp)
	{
		ReceiveStress(FMath::RandRange(7, 13));
	}
}

void ACharacterBase::HandleDeath()
{
	UE_LOG(LogTemp, Log, TEXT("[CharacterBase] %s 사망"), *GetName());

	//공격자 측 AfterSlay 패시브 디스패치, 처치 지점 위치 전달
	if (ACharacterBase* attacker = lastAttacker.Get())
	{
		if (UPassiveSkillComponent* passivecomponent = attacker->GetPassiveSkillComponent())
		{
			passivecomponent->DispatchAfterSlay(GetActorLocation());
		}
	}

	OnCharacterDeath.Broadcast(this);

	//NavMesh 장애물 해제
	SetNavObstacleEnabled(false);

	//충돌 비활성화, Destroy 전 오버랩 정리 유도
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Destroy();
}

void ACharacterBase::ShowSkillInfo()
{
	skillInfoWidgetComponent->SetVisibility(true);
	if (USkillInfoWidget* InfoWidget = Cast<USkillInfoWidget>(skillInfoWidgetComponent->GetWidget()))
	{
		InfoWidget->UpdateInfo(pendingDMG, pendingAccuracy, pendingCritical, pendingSkillType);
	}
}

void ACharacterBase::HideSkillInfo()
{
	skillInfoWidgetComponent->SetVisibility(false);
}

void ACharacterBase::ClearPendingDamage()
{
	pendingDMG = 0.f;
	pendingAccuracy = 0.f;
	pendingCritical = 0.f;
	pendingSkillType = ESkillType::Melee;
}

void ACharacterBase::SetNavObstacleEnabled(bool bEnabled)
{
	GetCapsuleComponent()->SetCanEverAffectNavigation(bEnabled);

	if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
	{
		NavSys->UpdateActorInNavOctree(*this);
		//월드 시작 직후 등 옥트리 갱신이 타일 재빌드로 이어지지 않는 경우 대비, 캡슐 영역 명시적 dirty
		NavSys->AddDirtyArea(GetCapsuleComponent()->Bounds.GetBox().ExpandBy(100.f), ENavigationDirtyFlag::All);
	}
}

UStaticMeshComponent* ACharacterBase::GetWeaponMeshComponent() const
{
	return WeaponMeshComp;
}
