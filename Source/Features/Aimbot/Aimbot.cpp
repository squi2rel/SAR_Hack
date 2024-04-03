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
	"LowestHealth"
};

bool useVel = true;
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
	if (getTimeMillis() - lastTime < 1000 / 60) return;
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
		const NetworkPlayer* targetPlayer = nullptr;

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
		case 1: //LowestHealth
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

		if (targetPlayer == nullptr) return;

		const auto& targetPos = targetPlayer->pos;

		const auto& it = lastPos.find(targetPlayer->playerID);
		auto& last = it->second;

		const auto& distanceToTarget = Utils::Length(localPlayerPos, targetPos);
		const auto deltaX = (targetPos.x - last.x) * distanceToTarget / 10, deltaY = (targetPos.y - last.y) * distanceToTarget / 10;//TODO test this
		last.x = targetPos.x;
		last.y = targetPos.y;

		const auto& screenPos = hooks->WorldToScreenPoint2(ctx.localPlayer->camera, useVel ? Vector3(targetPos.x + deltaX, targetPos.y + deltaY, 0.0f) : Vector3(targetPos.x, targetPos.y, 0.0f));
		const Vector3& screenTargetPos = screenPos;

		RECT rect;
		GetClientRect(window, &rect);
		POINT windowPos = { rect.left, rect.top };
		ClientToScreen(window, &windowPos);

		const int& x = windowPos.x;
		const int& y = windowPos.y;
		const int width = rect.right;
		const int height = rect.bottom;

		if (screenTargetPos.x > 0 && screenTargetPos.x < width && screenTargetPos.y > 0 && screenTargetPos.y < height)
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
}