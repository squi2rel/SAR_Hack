#include "Aimbot.hpp"
#include "../../Core/Hooks/HooksManager.hpp"
#include "../../Core/Math/Utils.hpp"
#include <spdlog/spdlog.h>
#include <winuser.h>
#include <windows.h>

#include <imgui.h>

AimbotModule::AimbotModule(ModulesManager* const modules, HooksManager* const hooks, Config& cfg)
	: ModuleBase(AIMBOT_MODULE_NAME, modules, hooks, cfg)
{}

const char* AimbotModule::modes[] = {
	"ClosestDistance",
	"ClosestDistanceToMouse",
	"LowestHealth"
};

const static int16_t shotgunID1 = 0x11a, shotgunID2 = 0x11b;

NetworkPlayer* lockedOn = nullptr;

bool setWeapon = false;
std::map<short, Vector2> lastPos;
long long lastTime = 0;

static unsigned long long getTimeMillis() {
	using namespace std::chrono;
	return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void PlayerStart(NetworkPlayer* player) {
	lastPos.emplace(player->playerID, Vector2(player->pos.x, player->pos.y));
}

void PlayerDestroy(NetworkPlayer* player) {
	lastPos.erase(player->playerID);
	if (player == lockedOn) lockedOn = nullptr;
}

void LocalPlayerStart(LocalPlayerScript* player) {
	lastTime = getTimeMillis();
}

void AimbotModule::Init()
{
	hooks->GetGameContext().OnPlayerChanged(PlayerStart, PlayerDestroy);
	hooks->GetGameContext().OnLocalStart(LocalPlayerStart);
}

void AimbotModule::Run()
{
	if (getTimeMillis() - lastTime < 1000 / 25) return;
	lastTime = getTimeMillis();
	if (cfg.bAimbot && (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0x8000 == cfg.bAutoAimbot)
	{
		Sleep(5); // Made to remove textures blicks

		const HWND& window = hooks->GetGraphicsContext().hwnd;
		if (GetForegroundWindow() != window) return;

		const auto& ctx = hooks->GetGameContext();
		const size_t playersCount = ctx.players.size();

		if (!ctx.localPlayer || playersCount == 0 || ctx.localPlayer->player->playerIsDead) return;

		const auto& localPlayerPos = ctx.localPlayer->player->pos;
		const auto& screenLocalPlayerPos = hooks->WorldToScreenPoint2(ctx.localPlayer->camera, Vector3(localPlayerPos.x, localPlayerPos.y, 0.0f));

		float nearest = INFINITY;
		NetworkPlayer* targetPlayer = nullptr;

		RECT rect;
		GetClientRect(window, &rect);
		POINT windowPos = { rect.left, rect.top };
		ClientToScreen(window, &windowPos);
		const int& x = windowPos.x;
		const int& y = windowPos.y;
		POINT mouse;
		GetCursorPos(&mouse);
		const int screenX = (int) mouse.x - x, screenY = (int) ImGui::GetIO().DisplaySize.y - (int) mouse.y + y;

		if (lockedOn != nullptr && lockedOn->playerIsDead) return;
		if (cfg.bAimbotLock && lockedOn != nullptr)
		{
			targetPlayer = Utils::Length(localPlayerPos, lockedOn->pos) > cfg.fAimbotMaxDistance ? nullptr : lockedOn;
		}
		else
		{
			switch (cfg.iAimbotMode)
			{
			case 0: //ClosestDistance
				for (auto* target : ctx.players)
				{
					if (!target) continue;
					if (target == ctx.localPlayer->player) continue;
					if (target->playerIsDead || cfg.bZombieMode && target->isZombie == ctx.localPlayer->player->isZombie) continue;

					const auto& targetPos = target->pos;
					const auto& distanceToTarget = Utils::Length(localPlayerPos, targetPos);

					if (distanceToTarget > cfg.fAimbotMaxDistance) continue;
					if (distanceToTarget < nearest) {
						nearest = distanceToTarget;
						targetPlayer = target;
					}
				}
				break;
			case 1: //ClosestDistanceToMouse
				for (auto* target : ctx.players)
				{
					if (!target) continue;
					if (target == ctx.localPlayer->player) continue;
					if (target->playerIsDead || cfg.bZombieMode && target->isZombie == ctx.localPlayer->player->isZombie) continue;

					const auto& targetPos = target->pos;
					Vector3 v = hooks->WorldToScreenPoint2(ctx.localPlayer->camera, Vector3(targetPos.x, targetPos.y, 0.0f));
					const auto& distanceToTarget = Utils::Length(Vector2((float) screenX, (float) screenY), Vector2(v.x, v.y));

					if (Utils::Length(localPlayerPos, targetPos) > cfg.fAimbotMaxDistance) continue;
					if (distanceToTarget < nearest) {
						nearest = distanceToTarget;
						targetPlayer = target;
					}
				}
				break;
			case 2: //LowestHealth
				for (auto* target : ctx.players)
				{
					if (!target) continue;
					if (target == ctx.localPlayer->player) continue;
					if (target->playerIsDead || cfg.bZombieMode && target->isZombie == ctx.localPlayer->player->isZombie) continue;

					const auto& targetPos = target->pos;
					if (Utils::Length(localPlayerPos, targetPos) > cfg.fAimbotMaxDistance) continue;

					const auto& targetHealth = target->playerHP;
					if (targetHealth < nearest) {
						nearest = targetHealth;
						targetPlayer = target;
					}
				}
				break;
			}
		}

		lockedOn = targetPlayer;
		if (targetPlayer == nullptr) return;

		const auto& targetPos = targetPlayer->pos;

		const auto& it = lastPos.find(targetPlayer->playerID);
		auto& last = it->second;

		const auto& distanceToTarget = Utils::Length(localPlayerPos, targetPos);
		const auto deltaX = (targetPos.x - last.x) * distanceToTarget / 10, deltaY = (targetPos.y - last.y) * distanceToTarget / 10;
		last.x = targetPos.x;
		last.y = targetPos.y;

		if (cfg.bAimbotAutoSwitch)
		{
			const auto& arr = ctx.localPlayer->player->equipmentIDs->equipments;
			if (distanceToTarget < 40 && !setWeapon)
			{
				if (arr[0] == shotgunID1 || arr[0] == shotgunID2)
				{
					keybd_event('1', 0, 0, 0);
					keybd_event('1', 0, KEYEVENTF_KEYUP, 0);
					setWeapon = true;
				}
				else if (arr[1] == shotgunID1 || arr[1] == shotgunID2)
				{
					keybd_event('2', 0, 0, 0);
					keybd_event('2', 0, KEYEVENTF_KEYUP, 0);
					setWeapon = true;
				}
			}
			else if (distanceToTarget > 50 && setWeapon) {
				if (arr[0] == shotgunID1 || arr[0] == shotgunID2)
				{
					keybd_event('2', 0, 0, 0);
					keybd_event('2', 0, KEYEVENTF_KEYUP, 0);
				}
				else
				{
					keybd_event('1', 0, 0, 0);
					keybd_event('1', 0, KEYEVENTF_KEYUP, 0);
				}
				setWeapon = false;
			}
		}

		const auto& screenPos = hooks->WorldToScreenPoint2(ctx.localPlayer->camera, cfg.bAimbotForecast ? Vector3(targetPos.x + deltaX, targetPos.y + deltaY, 0.0f) : Vector3(targetPos.x, targetPos.y, 0.0f));
		const Vector3& screenTargetPos = screenPos;
		const int width = rect.right;
		const int height = rect.bottom;

		if (screenTargetPos.x > 10 && screenTargetPos.x < width - 10 && screenTargetPos.y > 10 && screenTargetPos.y < height - 10)
		{ //lerp set
			POINT mouse;
			GetCursorPos(&mouse);

			const float tx = x + screenTargetPos.x, ty = y + height - screenTargetPos.y - 15;
			const float& alpha = cfg.fAimbotAlpha;
			const float invAlpha = 1.0f - alpha;
			if (Utils::Length({ tx, ty }, { (float)mouse.x, (float)mouse.y }) < 50) {
				SetCursorPos((int)tx, (int)ty);
			}
			else
			{
				SetCursorPos((int)(mouse.x * invAlpha + tx * alpha), (int)(mouse.y * invAlpha + ty * alpha));
			}
		}
		else
		{ //round
			const float angle = atan2f(screenTargetPos.x - screenLocalPlayerPos.x, screenTargetPos.y - screenLocalPlayerPos.y + 15);
			SetCursorPos(x + width / 2 + (int)(sin(angle) * 200), y + height / 2 - (int)(cos(angle) * 200));
		}
	}
	else
	{
		lockedOn = nullptr;
		setWeapon = false;
	}
}