//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/SpecialForces/Basic/SpecialForcesResupply.h"

USpecialForcesResupply::USpecialForcesResupply()
{
	skillName = NSLOCTEXT("Skill", "SpecialForcesResupply_Name", "장비 정비");
	skillDescription = NSLOCTEXT("Skill", "SpecialForcesResupply_Description", "탄창과 휴대 장비를 3개 보충한다.");

	reloadAmount = 3;
}
