#include "EspModule.hpp"
#include "../../Core/Hooks/HooksManager.hpp"
#include "../../Core/Math/Utils.hpp"
#include <spdlog/spdlog.h>
#include <string>

#include <imgui.h>

constexpr double M_PI = 3.14159265358979323846;

void* EspModule::po = nullptr;

EspModule::EspModule(ModulesManager* const modules, HooksManager* const hooks, Config& cfg)
	: ModuleBase(ESP_MODULE_NAME, modules, hooks, cfg)
{}

void EspModule::Run()
{
}

void EspModule::Draw()
{
	if (cfg.bEsp)
	{
		Sleep(5); // Made to remove textures blicks
		const auto& ctx = hooks->GetGameContext();
		if (!ctx.localPlayer) return;
		
		const size_t playersCount = ctx.players.size();

		std::stringstream ioss;
		std::string s;
		ioss << std::hex << po;
		ioss >> s;

		ImGui::GetWindowDrawList()->AddText(
			{ 10, 10 },
			ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f)),
			s.c_str()
		);

		for (auto* target : ctx.players)
		{
			if (!target) continue;
			if (target == ctx.localPlayer->player) continue;
			if (target->playerIsDead || cfg.bZombieMode && target->isZombie == ctx.localPlayer->player->isZombie) continue;

			if (cfg.bSnaplines && playersCount > 0)
			{
				const auto& localPlayerPos = ctx.localPlayer->player->pos;
				const auto& targetPos = target->pos;

				const auto& distanceToTarget = Utils::Length(localPlayerPos, targetPos);

				const auto& screenLocalPlayerPos = hooks->WorldToScreenPoint2(ctx.localPlayer->camera, Vector3(localPlayerPos.x, localPlayerPos.y, 0.0f));
				const auto& screenTargetPos = hooks->WorldToScreenPoint2(ctx.localPlayer->camera, Vector3(targetPos.x, targetPos.y, 0.0f));

				const auto& maxDistance = cfg.fMaxSnaplineRenderDistance;
				const auto& nearDistance = cfg.fNearSnaplineRenderDistance;

				if (distanceToTarget > maxDistance) continue;

				ImU32 color = (distanceToTarget <= nearDistance)
					? ImGui::ColorConvertFloat4ToU32(ImVec4(0.0f, 1.0f, 0.0f, 1.0f))
					: ImGui::ColorConvertFloat4ToU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));

				ImGui::GetWindowDrawList()->AddLine(
					{ screenLocalPlayerPos.x, screenLocalPlayerPos.y },
					{ screenTargetPos.x, ImGui::GetIO().DisplaySize.y - screenTargetPos.y - 15.0f },
					color
				);

				if (!cfg.bEspShowDistance || distanceToTarget > nearDistance || distanceToTarget < 35) continue;

				const float angle = atan2f(screenTargetPos.x - screenLocalPlayerPos.x, screenTargetPos.y - screenLocalPlayerPos.y + 15);
				const auto& str = std::to_string((int)(distanceToTarget));

				const auto& font = ImGui::CalcTextSize(str.c_str());

				ImGui::GetWindowDrawList()->AddText(
					{ screenLocalPlayerPos.x + sinf(angle) * 200 - font.x / 2, screenLocalPlayerPos.y - cosf(angle) * 200 - font.y / 2 },
					color,
					str.c_str()
				);
			}
		}
	}
}
