//Fill out your copyright notice in the Description page of Project Settings.

#include "Object/Ally/Sniper/Basic/SniperReload.h"

USniperReload::USniperReload()
{
	skillName = NSLOCTEXT("Skill", "SniperReload_Name", "재장전");
	skillDescription = NSLOCTEXT("Skill", "SniperReload_Description", "탄환을 4개 보충한다.");

	reloadAmount = 4;
}
