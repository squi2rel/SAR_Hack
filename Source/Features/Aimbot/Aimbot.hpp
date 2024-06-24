#pragma once
#include "../../Core/Modules/ModuleBase.hpp"

constexpr const char* AIMBOT_MODULE_NAME = "AIMBOT";

class AimbotModule final : public ModuleBase
{
public:
	static const char* modes[3];
	explicit AimbotModule(ModulesManager* const modules, HooksManager* const hooks, Config& cfg);

	void Init();
	void Run();
};