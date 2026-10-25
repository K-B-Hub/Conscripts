//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Object/Buff/BuffBase.h"
#include "RifleMoraleBuff.generated.h"

//사기 진작이 부여하는 버프, 2턴간 명중을 올린다
UCLASS()
class PW_API URifleMoraleBuff : public UBuffBase
{
	GENERATED_BODY()

public:
	URifleMoraleBuff();
};