#pragma once

enum class eMessage
{
	eStartGame,
	eQuitGame,
	eOpenSettings,
	ePopStack,
	ePopStackToMainMenu,
	eStartLevel1,
	eStartLevel2,
	eStartLevel3,
	eStartLevel4,
	eStartLevel5,
	ePauseGame,
	eOpenLevelSelect,
	eChangeMasterVolume,
	eChangeMusicVolume,
	eChangeSFXVolume,
	eStartCutscene,
	eLevelUp,
	eUnlockRMB,
	eUnlockAOE,
	eUnlockTeleport,
	eUnlockHook,
	eUnlockUltimate,
	eGeneralLevelUp,
	ePlayerDied,
	eRespawn,
	eUpdateResolution,
	eOpenCredits,

	e960x540 = 540,
	e1280x720 = 720,
	e1600x900 = 900,
	e1920x1080 = 1080,
	e2560x1440 = 1440,

	Count
};