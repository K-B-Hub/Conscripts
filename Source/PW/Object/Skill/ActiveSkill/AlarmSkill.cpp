//Fill out your copyright notice in the Description page of Project Settings.


#include "Object/Skill/ActiveSkill/AlarmSkill.h"
#include "Object/Buff/AlarmBuff.h"
#include "Characters/CharacterBase.h"
#include "GameMode/BattleGameMode.h"

UAlarmSkill::UAlarmSkill()
{
	skillName = NSLOCTEXT("Skill", "Alarm_Name", "알람");
	skillDescription = NSLOCTEXT("Skill", "Alarm_Description", "알람을 준비한다. 만료 전에 처치하지 못하면 모든 적이 전투에 합류한다");

	skillType = ESkillType::Buff;
	damageType = EDamageType::Normal;
	selectMode = ESelectMode::Self;
	pickTeam = EPickTeam::AllyOnly;
	areaTarget = EAreaTarget::None;
	areaForm = EAreaForm::Circle;

	pickRange = 0.f;
	pickCount = 1;

	battleResourceCost = 0;
	actionPointCost = 1;

	damageRatio = 0.f;
	bonusAccuracy = 0.f;
	bonusCritical = 0.f;
	baseDamage = 0;
	bonusPenetration = 0;
	bonusDamageAmplication = 0;

	static ConstructorHelpers::FObjectFinder<UAnimMontage> MontageAsset(TEXT("/Game/Animation/Animations/Rifleman/AnimMontage/AM_Rifle_Fire_Montage"));
	if (MontageAsset.Succeeded())
	{
		skillMontage = MontageAsset.Object;
	}

	buffs.Add(UAlarmBuff::StaticClass());
}

bool UAlarmSkill::CanExecute() const
{
	if (!Super::CanExecute()) return false;

	//전역 1회 제한, 알람이 실제 발동된 뒤에만 사용 불가 (무산 시 재사용 가능)
	ACharacterBase* ownerPtr = GetOwner();
	UWorld* world = ownerPtr ? ownerPtr->GetWorld() : nullptr;
	ABattleGameMode* gm = world ? world->GetAuthGameMode<ABattleGameMode>() : nullptr;
	return gm && !gm->IsAlarmUsed();
}
