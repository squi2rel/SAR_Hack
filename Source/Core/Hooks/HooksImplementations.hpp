#include "HooksDefinitions.hpp"
#include "HooksManager.hpp"
#include <spdlog/spdlog.h>

#include "../Modules/ModulesManager.hpp"
#include "../../Features/ESP/EspModule.hpp"
#include "../../Features/Aimbot/Aimbot.hpp"

#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HooksManager* gpHooksManager = nullptr;
GameContext* gpCtx = nullptr;

void InitHooksDependencies(HooksManager* pHooksManager, GameContext* ctx)
{
	gpHooksManager = pHooksManager;
	gpCtx = ctx;

	// Init D3D11 hook
	auto& context = gpHooksManager->GetGraphicsContext();
	context.hwnd = GetForegroundWindow();
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 2;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = context.hwnd;
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	if (D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		0,
		feature_levels,
		2,
		D3D11_SDK_VERSION,
		&sd,
		&context.pSwapchain,
		&context.pDevice,
		nullptr,
		nullptr) != S_OK
		)
	{
		spdlog::error("Failed to create dummy D3D11 device");
		return;
	}

	context.hwnd = nullptr;

	void** pVmt = *(void***)context.pSwapchain;

	HooksDefinitions::oSwapChainPresent = (HooksDefinitions::T_SwapChainPresent)(pVmt[HooksDefinitions::SWAPCHAIN_PRESENT_INDEX]);
	HooksDefinitions::oResizeBuffers = (HooksDefinitions::T_SwapChainResizeBuffers)(pVmt[HooksDefinitions::SWAPCHAIN_RESIZE_INDEX]);
	HooksDefinitions::oSetFullscreenState = (HooksDefinitions::T_SwapChainSetFullscreenState)(pVmt[HooksDefinitions::SWAPCHAIN_FULLSCREEN_INDEX]);

	if (context.pSwapchain)
	{
		context.pSwapchain->Release();
		context.pSwapchain = nullptr;
	}

	if (context.pDevice)
	{
		context.pDevice->Release();
		context.pDevice = nullptr;
	}
}

void __fastcall HooksDefinitions::HkLocalPlayerStart(void* pLocalPlayer)
{
	EspModule::po = pLocalPlayer;

	LocalPlayerScript* pointer = reinterpret_cast<LocalPlayerScript*>(pLocalPlayer);
	gpCtx->localPlayer = pointer;

	HooksDefinitions::oLocalPlayerStart(pLocalPlayer);

	gpCtx->LocalStart(pointer);
}
void __fastcall HooksDefinitions::HkNetworkPlayerStart(void* pNetworkPlayer, int16_t playerId, int8_t b1)
{
	NetworkPlayer* pointer = reinterpret_cast<NetworkPlayer*>(pNetworkPlayer);
	gpCtx->players.push_back(pointer);

	HooksDefinitions::oNetworkPlayerStart(pNetworkPlayer, playerId, b1);

	gpCtx->PlayerStart(pointer);
}
void __fastcall HooksDefinitions::HkNetworkPlayerDestroy(void* pNetworkPlayer)
{
	if (gpCtx->localPlayer && pNetworkPlayer == reinterpret_cast<void*>(gpCtx->localPlayer->player))
	{
		gpCtx->localPlayer = nullptr;
	}

	NetworkPlayer* pointer = reinterpret_cast<NetworkPlayer*>(pNetworkPlayer);

	gpCtx->players.remove(pointer);

	HooksDefinitions::oNetworkPlayerDestroy(pNetworkPlayer);

	gpCtx->PlayerDestroy(pointer);
}
HRESULT __stdcall HooksDefinitions::HkSwapChainPresent(IDXGISwapChain* pSwapChain, UINT syncInterval, UINT flags)
{
	auto& context = gpHooksManager->GetGraphicsContext();
	auto* const modules = gpHooksManager->GetModules();
	auto& cfg = modules->GetConfig();

	if (cfg.bUnloadCheat) return HooksDefinitions::oSwapChainPresent(pSwapChain, syncInterval, flags);

	context.Init(pSwapChain);
	context.InitImGui();

	if (context.IsImGuiInited())
	{
		cfg.bRenderFinished = false;
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			RECT rect;
			GetWindowRect(context.hwnd, &rect);
			const int width = rect.right - rect.left;
			const int height = rect.bottom - rect.top;

			ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
			ImGui::SetNextWindowSize(ImVec2((float)width, (float)height));
			ImGui::Begin("Overlay", nullptr, 
				ImGuiWindowFlags_NoBackground	| 
				ImGuiWindowFlags_NoMove			| 
				ImGuiWindowFlags_NoResize		| 
				ImGuiWindowFlags_NoTitleBar		| 
				ImGuiWindowFlags_NoInputs
			);
			{
				if (context.bMenuOpen)
				{
					ImGui::SetNextWindowSize({800.0f, 550.0f});
					if (ImGui::Begin("SuperAnimalHack", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
					{
						ImGui::Text("Global");
						ImGui::Checkbox("Bwoking dead mode", &cfg.bZombieMode);

						ImGui::Separator();

						ImGui::Text("No recoil");
						ImGui::Checkbox("NoRecoil", &cfg.bNoRecoil);

						ImGui::Separator();

						ImGui::Text("Speedhack");
						ImGui::Checkbox("Jump speedhack", &cfg.bSpeedhack);

						ImGui::Separator();

						ImGui::Text("Camera");
						ImGui::Checkbox("Zoom", &cfg.bZoomOverride);
						ImGui::TextColored(ImColor(1.0f, 0.0f, 0.0f, 1.0f), "May cause lower FPS!!!");
						ImGui::SliderFloat("ZoomValue", &cfg.fZoomOverrideValue, 67.5f, 400.0f);

						ImGui::Separator();

						ImGui::Text("ESP");
						ImGui::Checkbox("ESP", &cfg.bEsp);
						ImGui::Checkbox("Snaplines", &cfg.bSnaplines);
						ImGui::Checkbox("Show distance", &cfg.bEspShowDistance);
						ImGui::SliderFloat("Snaplines distance", &cfg.fMaxSnaplineRenderDistance, 0.0f, 5000.0f);
						ImGui::SliderFloat("Snaplines near distance", &cfg.fNearSnaplineRenderDistance, 0.0f, 5000.0f);

						ImGui::Separator();

						ImGui::Text("Aimbot");
						ImGui::Checkbox("Aimbot", &cfg.bAimbot);
						ImGui::Checkbox("Lock target", &cfg.bAimbotLock);
						ImGui::Checkbox("Auto shotgun", &cfg.bAimbotAutoSwitch);
						ImGui::Checkbox("Forecast movement", &cfg.bAimbotForecast);
						ImGui::Checkbox("Keep active", &cfg.bAutoAimbot);
						if (cfg.bAutoAimbot)
						{
							ImGui::Text("Or hold LSHIFT to deactive");
						}
						else
						{
							ImGui::Text("Or hold LSHIFT to active");
						}
						ImGui::SliderFloat("Max distance", &cfg.fAimbotMaxDistance, 10.0f, 200.0f);
						ImGui::SliderFloat("Mouse move percent", &cfg.fAimbotAlpha, 0.0f, 1.0f);
						ImGui::Combo("Mode", &cfg.iAimbotMode, AimbotModule::modes, IM_ARRAYSIZE(AimbotModule::modes));
					}
					ImGui::End();
				}

				auto* const esp = modules->GetModule<EspModule>(ESP_MODULE_NAME);

				esp->Draw();
			}
			ImGui::End();
		}
		ImGui::EndFrame();
		ImGui::Render();

		context.pContext->OMSetRenderTargets(1, &context.pRTV, nullptr);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		cfg.bRenderFinished = true;
	}

	return HooksDefinitions::oSwapChainPresent(pSwapChain, syncInterval, flags);
}
HRESULT __stdcall HooksDefinitions::HkResizeBuffers(IDXGISwapChain* pSwapChain, const UINT BufferCount, const UINT Width, const UINT Height, const DXGI_FORMAT NewFormat, const UINT SwapChainFlags)
{
	auto& ctx = gpHooksManager->GetGraphicsContext();
	ctx.ShutdownImGui();

	return oResizeBuffers(pSwapChain, BufferCount, Width, Height, NewFormat, SwapChainFlags);
}
HRESULT __stdcall HooksDefinitions::HkSetFullscreenState(IDXGISwapChain* pSwapChain, const BOOL FullScreen, IDXGIOutput* pTarget)
{
	auto& ctx = gpHooksManager->GetGraphicsContext();
	ctx.ShutdownImGui();

	return oSetFullscreenState(pSwapChain, FullScreen, pTarget);
}
LRESULT APIENTRY HooksDefinitions::HkWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);

	return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
}