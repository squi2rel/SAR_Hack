#pragma once
#include <nlohmann/json.hpp>

struct Config
{
	bool bRenderFinished = false;
	bool bEspOnTeammates = false;
	bool bZoomOverride = false;
	bool bEspOnEnemies = false;
	bool bUnloadCheat = false;
	bool bSpeedhack = false;
	bool bSnaplines = false;
	bool bHealthBar = false;
	bool bArmorBar = false;
	bool bNoRecoil = false;
	bool bBoxes = false;
	bool bBhop = false;
	bool bEsp = false;
	bool bEspShowDistance = true;
	bool bZombieMode = false;
	bool bAimbot = false;
	bool bAutoAimbot = false;
	bool bAimbotLock = true;
	bool bAimbotAutoSwitch = false;
	bool bAimbotForecast = true;

	int iAimbotMode = 0;

	const float fZoomDefaultValue = 67.5f;

	float fZoomOverrideValue = fZoomDefaultValue;
	float fNearSnaplineRenderDistance = 200.0f;
	float fMaxSnaplineRenderDistance = 500.0f;
	float fAimbotMaxDistance = 120.0f;
	float fAimbotAlpha = 0.03f;

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
		Config,
		fNearSnaplineRenderDistance,
		fMaxSnaplineRenderDistance,
		fZoomOverrideValue,
		bEspOnTeammates,
		bEspOnEnemies,
		bZoomOverride,
		bSnaplines,
		bHealthBar,
		bSpeedhack,
		bArmorBar,
		bNoRecoil,
		bBoxes,
		bBhop,
		bEsp,
		bAimbot
	);
};
