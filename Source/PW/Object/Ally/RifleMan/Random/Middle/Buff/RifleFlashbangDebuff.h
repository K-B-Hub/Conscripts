//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "RifleFlashbangDebuff.generated.h"

//섬광탄이 부여하는 디버프, 음수 스탯 버프로 모델링한다
UCLASS()
class PW_API URifleFlashbangDebuff : public UBuffBase
{
	GENERATED_BODY()

public:
	URifleFlashbangDebuff();
};