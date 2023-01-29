#ifndef PCH_H
#define PCH_H

#include "framework.h"

# include <Windows.h>
# include <TlHelp32.h>
# include <vector>
# include <string>
# include <filesystem>
# include <fstream>
# include <map>
# include <unordered_map>
# include <sddl.h>
# include <optional>
# include <random>

# include <d3d11.h>

# include <nlohmann/json.hpp>
# include <cpr/cpr.h>
# include <fmt/format.h>

# include "Encryption.hpp"

# include <imgui.h>
# include <imgui_internal.h>
# include <imgui_impl_dx11.h>
# include <imgui_impl_win32.h>
# include <ImGuiPP.hpp>
# include <Options.hpp>

# include <Fonts.hpp>

# pragma comment(lib, "D3DX11.lib")
# pragma warning(disable: 4005)

# include <Resource.h>
# include <ResourceCache.h>
# include <ResourceManager.h>
# include <ResourceMetaDataComponent.h>

# define Log(Arg, ...) printf_s(Arg, __VA_ARGS__)

typedef HRESULT(__stdcall* Present) (IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
typedef LRESULT(__stdcall* WNDPROC)(HWND, UINT, WPARAM, LPARAM);

using json = nlohmann::json;
namespace fs = std::filesystem;

#define CALL_ONCE(Function) { static bool _OnceCalled = false;  if (!_OnceCalled) { Function(); _OnceCalled = true; } }

#endif 
