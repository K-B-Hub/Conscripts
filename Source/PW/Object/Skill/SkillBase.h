//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SkillBase.generated.h"

class ACharacterBase;
class UBuffBase;
class UAilmentBase;
//모든 스킬(액티브, 패시브)의 공통 기반 데이터
//Blueprintable이어야 파생 BP를 만들 수 있다, 아이콘 등 에셋 지정이 BP에서 이뤄진다
UCLASS(Abstract, Blueprintable, BlueprintType)
class PW_API USkillBase : public UObject
{
	GENERATED_BODY()

public:
	//스킬 이름 (UI 표시용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	FText skillName;
	//스킬 설명 (UI 툴팁용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	FText skillDescription;
	//스킬 아이콘 (UI 버튼에 표시), 상세창·툴팁·강화 카드는 이것만 쓴다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	TObjectPtr<UTexture2D> skillIcon;
	//전투 스킬 버튼의 호버·클릭 아이콘, 미지정이면 skillIcon을 그대로 쓴다
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	TObjectPtr<UTexture2D> skillIconHovered;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Identity")
	TObjectPtr<UTexture2D> skillIconPressed;
	//스킬 소유 캐릭터
	UPROPERTY()
	TWeakObjectPtr<ACharacterBase> owner;
	//스킬 적중 시 대상에게 부여될 버프 클래스 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Buff")
	TArray<TSubclassOf<UBuffBase>> buffs;
	//스킬 적중 시 대상에게 부여될 상태이상 클래스 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill|Ailment")
	TArray<TSubclassOf<UAilmentBase>> ailments;
	//소유자 설정
	void SetOwner(ACharacterBase* InOwner);

	ACharacterBase* GetOwner() const;
};
