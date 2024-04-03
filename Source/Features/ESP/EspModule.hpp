#pragma once

#include "../../Core/Modules/ModuleBase.hpp"

constexpr const char* ESP_MODULE_NAME = "ESP";

class EspModule final : public ModuleBase
{
public:
	static void* po;
	explicit EspModule(ModulesManager* const modules, HooksManager* const hooks, Config& cfg);

	void Run();
	void Draw();
};