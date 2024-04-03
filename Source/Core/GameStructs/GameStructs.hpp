#pragma once

#include "../Math/Vectors.hpp"
#include <list>
#include <functional>

// Generated using ReClass.NET

enum struct EPlayerActions // GEnum3
{
	None,
	Healing,
	HealingCupgrade,
	Zipline
};
enum struct EPlatformType // GEnum26
{
	Anonymous,
	Steam,
	Xbox,
	PSN,
	Switch,
	FB
};
enum struct EWalkMode // GEnum34
{
	Slow,
	Normal,
	Roll,
	RollCreep,
	Vehicle,
	Downed,
	BananaStun,
	ParachuteFreefallSplat
};

// Created with ReClass.NET 1.2 by KN4CK3R

class LocalPlayerScript
{
public:
	char pad_0000[24]; //0x0000
	void* camera; //0x0018
	float horizInput; //0x0020
	float vertInput; //0x0024
	Vector2 velocity; //0x0028
	float N00000432; //0x0030
	char pad_0034[4]; //0x0034
	void* gameLevelType; //0x0038
	void* mainGameScript; //0x0040
	class NetworkPlayer* player; //0x0048
	void* playerReloadingBGQuad; //0x0050
	void* playerReloadingQuad; //0x0058
	void* nearInteractionObj; //0x0060
	void* quickChatMousingOverObject; //0x0068
	void* mousingOverFish; //0x0070
	float interactTextYOffset; //0x0078
	bool changedWeaponOrAmmoOrShotOrReloadedNeedsUIRefresh; //0x007C
	bool currentInteractionIsForCrafting; //0x007D
	char pad_007E[2]; //0x007E
	float inWeaponSwitchUntilTime; //0x0080
	bool pickedUpWeaponNeedsPlaySound; //0x0084
	bool pickedUpAmmoNeedsPlaySound; //0x0085
	bool pickedUpArmorNeedsPlaySound; //0x0086
	bool pickedUpOtherNeedsPlaySound; //0x0087
	bool cantLootNeedsPlaySound; //0x0088
	bool forgeWeaponNeedsPlaySound; //0x0089
	char pad_008A[6]; //0x008A
	void* pickedUpAttachmentNeedsPlaySound; //0x0090
	void* enteredOrExitedVehicleNeedsPlaySound; //0x0098
	int16_t lastMoleCrateIndexOpened; //0x00A0
	char pad_00A2[6]; //0x00A2
	void* parachuteLandingObj; //0x00A8
	void* parachuteLineObj; //0x00B0
	float parachuteStartTime; //0x00B8
	float localLandingTime; //0x00BC
	float parachuteDiveSwapDisabledUntil; //0x00C0
	bool canFlightEject; //0x00C4
	char pad_00C5[3]; //0x00C5
	Vector2 lastServerPosition; //0x00C8
	Vector2 lastServerPositionForBananaStun; //0x00D0
	bool bananaStunDoneInterp; //0x00D8
	char pad_00D9[3]; //0x00D9
	float mouseDistanceFromGun; //0x00DC
}; //Size: 0x00E0

class NetworkPlayer
{
public:
	char pad_0000[112]; //0x0000
	class GameCamera* gameCamera; //0x0070
	char pad_0078[64]; //0x0078
	int16_t playerID; //0x00B8
	char pad_00BA[46]; //0x00BA
	int32_t accountLevelIndex; //0x00E8
	bool isAdminStartingGhost; //0x00EC
	char pad_00ED[59]; //0x00ED
	float currentGunAngleToMouse; //0x0128
	bool isPressingMoveKeys; //0x012C
	char pad_012D[3]; //0x012D
	float playerHP; //0x0130
	float playerHPMax; //0x0134
	bool playerIsDead; //0x0138
	char pad_0139[15]; //0x0139
	int32_t numKills; //0x0148
	int16_t killedByPlayerID; //0x014C
	char pad_014E[2]; //0x014E
	float lastPingTimeInSeconds; //0x0150
	char pad_0154[100]; //0x0154
	float healingJuice; //0x01B8
	bool healingCurrently; //0x01BC
	char pad_01BD[3]; //0x01BD
	float healingDrinkStartTime; //0x01C0
	char pad_01C4[4]; //0x01C4
	int8_t ductTapes; //0x01C8
	bool ductTapingCurrently; //0x01C9
	char pad_01CA[22]; //0x01CA
	int8_t currArmorLvl; //0x01E0
	char pad_01E1[3]; //0x01E1
	int32_t currArmorAmount; //0x01E4
	int32_t grenadeCount; //0x01E8
	void* lastAttackTime; //0x01EC
	char pad_01F4[16]; //0x01F4
	float lastAttackTimeMelee; //0x0204
	bool attacking; //0x0208
	bool currentAttackIsMelee; //0x0209
	bool currentAttackIsAuto; //0x020A
	char pad_020B[181]; //0x020B
	Vector2 pos; //0x02C4
	char pad_02CC[20]; //0x02CC
	bool needTrackFootsteps; //0x02E0
	bool isParachuting; //0x02E1
	char pad_02E2[4]; //0x02E2
	float currParachuteHeight; //0x02E6
	bool isParachuteDiving; //0x02EA
	char pad_02EB[21]; //0x02EB
	bool inFlight; //0x0300
	char pad_0301[69]; //0x0301
	bool isZombie; //0x0346
	char pad_0347[69]; //0x0347
	float rollStartTime; //0x038C
	float rollEndTime; //0x0390
	char pad_0394[32]; //0x0394
	bool didHitBunnyHopRoll; //0x03B4
	char pad_03B5[3]; //0x03B5
	float bunnyHopCurrentBonus; //0x03B8
	char pad_03BC[76]; //0x03BC
	void* bone_1; //0x0408
	void* bone_2; //0x0410
	void* bone_3; //0x0418
	void* bone_4; //0x0420
	void* bone_5; //0x0428
	void* bone_6; //0x0430
	void* bone_7; //0x0438
	void* bone_8; //0x0440
	char pad_0448[24]; //0x0448
	void* skeletalBounds; //0x0460
	char pad_0468[52]; //0x0468
	bool teammateLeft; //0x049C
	bool isWithinVisionBounds; //0x049D
	char pad_049E[74]; //0x049E
}; //Size: 0x04E8

class GameCamera
{
public:
	char pad_0000[24]; //0x0000
	Vector2 camTarget; //0x0018
	float camAngleRadians; //0x0020
	float parachutingOffset; //0x0024
	char pad_0028[4]; //0x0028
	bool inFlight; //0x002C
	bool isIn32v32; //0x002D
	char pad_002E[2]; //0x002E
	float mainOrthoSize; //0x0030
	float parachuteOrthoSize; //0x0034
	float flightOrthoSize; //0x0038
	float ghostMaxOrthoSize; //0x003C
	char pad_0040[12]; //0x0040
	bool isAdminGhost; //0x004C
	char pad_004D[7]; //0x004D
}; //Size: 0x0054

class Bone
{
public:
	char pad_0000[16]; //0x0000
	class BoneData* data; //0x0010
	char pad_0018[8]; //0x0018
	class Bone* parentBone; //0x0020
	char pad_0028[8]; //0x0028
	float x; //0x0030
	float y; //0x0034
	float rotation; //0x0038
	float scaleX; //0x003C
	float scaleY; //0x0040
	char pad_0044[8]; //0x0044
	float ax; //0x004C
	float ay; //0x0050
	float arotation; //0x0054
	float ascaleX; //0x0058
	float ascaleY; //0x005C
	char pad_0060[16]; //0x0060
	float worldX; //0x0070
	char pad_0074[8]; //0x0074
	float worldY; //0x007C
}; //Size: 0x0080

class BoneData
{
public:
	char pad_0000[16]; //0x0000
	int32_t index; //0x0010
	char pad_0014[4]; //0x0014
	wchar_t* name; //0x0018
	class BoneData* parent; //0x0020
	char pad_0028[4]; //0x0028
	float x; //0x002C
	float y; //0x0030
	float rotation; //0x0034
	float scaleX; //0x0038
	float scaleY; //0x003C
}; //Size: 0x0040

class SkeletalBounds
{
public:
	char pad_0000[24]; //0x0000
	float minX; //0x0018
	float minY; //0x001C
	float maxX; //0x0020
	float maxY; //0x0024
}; //Size: 0x0028


constexpr const int LOCAL_PLAYER_INDEX = 0;
class GameContext
{
private:
	using PlayerCallback = std::function<void(NetworkPlayer*)>;
	using LocalCallback = std::function<void(LocalPlayerScript*)>;
	mutable std::list<PlayerCallback> playerStart;
	mutable std::list<PlayerCallback> playerDestroy;
	mutable std::list<LocalCallback> localStart;
public:
	explicit GameContext()
		: localPlayer(nullptr)
	{}

	LocalPlayerScript* localPlayer = nullptr;

	std::list<NetworkPlayer*> players;

	void PlayerStart(NetworkPlayer* p)
	{
		for (auto& c : playerStart) {
			c(p);
		}
	}

	void PlayerDestroy(NetworkPlayer* p)
	{
		for (auto& c : playerDestroy) {
			c(p);
		}
	}

	void OnPlayerChanged(PlayerCallback start, PlayerCallback destroy) const
	{
		playerStart.push_back(start);
		playerDestroy.push_back(destroy);
	}

	void LocalStart(LocalPlayerScript* p)
	{
		for (auto& c : localStart) {
			c(p);
		}
	}

	void OnLocalStart(LocalCallback start) const
	{
		localStart.push_back(start);
	}
};