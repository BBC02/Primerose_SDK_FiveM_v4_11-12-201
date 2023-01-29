#include "pch.h"
#include "Renderer.hpp"
#include "Memory.hpp"
#include "GUI.hpp"
#include "Cheat.hpp"
#include "Options.hpp"

static auto Renderer = CRenderer::Get ( );
extern LRESULT ImGui_ImplWin32_WndProcHandler ( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

Present OPresent = nullptr;
HWND    OWindow = nullptr;
WNDPROC OWindowEx = nullptr;
ID3D11Device* Device = nullptr;
ID3D11DeviceContext* Context = nullptr;
ID3D11RenderTargetView* RenderTargetView = nullptr;
ImFont* PFont = nullptr;

static void InitializeImGui ( ) 
{
    ImGui::CreateContext ( );
    auto io = &ImGui::GetIO ( );

    io->ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    io->IniFilename = nullptr;
    io->LogFilename = nullptr;

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;

    ImFontConfig rubik;
    rubik.FontDataOwnedByAtlas = false;
    rubik.GlyphRanges = io->Fonts->GetGlyphRangesCyrillic ( );

    io->Fonts->AddFontFromMemoryTTF ( const_cast< uint8_t* >( Rubik ) , sizeof ( Rubik ) , 21.0f , &rubik );
    io->Fonts->AddFontFromMemoryCompressedTTF ( font_awesome_data , font_awesome_size , 58.5f , &icons_config , icons_ranges );

    PFont = io->Fonts->AddFontFromFileTTF ( _ ( "C:\\Windows\\Fonts\\cour.ttf" ) , 18.5f );

    io->Fonts->AddFontDefault ( );

    CO->Dialog.SetTitle ( _("Primerose - Choose a file" ) );
    CO->Dialog.SetTypeFilters ( { ".lua" } );
}

inline LRESULT __stdcall WindowHandler ( const HWND hWnd , UINT uMsg , WPARAM wParam , LPARAM lParam )
{
    if ( uMsg == WM_KEYUP && wParam == Renderer->OpenValue )
        Renderer->Open ^= 1;

    if ( Renderer->Open )
    {
        ImGui_ImplWin32_WndProcHandler ( hWnd , uMsg , wParam , lParam );
        return true;
    }

    auto fnCallWindowProcA = reinterpret_cast< decltype( &CallWindowProcA ) >( CMem::Get ( )->GetExpAddress ( _ ( "user32.dll" ) , _ ( "CallWindowProcA" ) ) );
    return fnCallWindowProcA ( OWindowEx , hWnd , uMsg , wParam , lParam );
}

inline bool PointerSwap ( IDXGISwapChain* pSwapChain )
{
    if ( SUCCEEDED ( pSwapChain->GetDevice ( __uuidof( ID3D11Device ) , ( void** ) &Device ) ) )
    {
        Device->GetImmediateContext ( &Context );

        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc ( &sd );

        OWindow = sd.OutputWindow;

        ID3D11Texture2D* pBackBuffer;

        pSwapChain->GetBuffer ( 0 , __uuidof( ID3D11Texture2D ) , ( LPVOID* ) &pBackBuffer );
        Device->CreateRenderTargetView ( pBackBuffer , NULL , &RenderTargetView );
        pBackBuffer->Release ( );

        OWindowEx = ( WNDPROC ) SetWindowLongPtr ( OWindow , GWLP_WNDPROC , ( LONG_PTR )WindowHandler );

        ImGui::CreateContext ( );

        InitializeImGui ( );

        ImGui_ImplWin32_Init ( OWindow );
        ImGui_ImplDX11_Init ( Device , Context );
        
        Renderer->InitSuccess = true;
    }

    return false;
}

inline HRESULT __stdcall PresentHook ( IDXGISwapChain* pSwapChain , UINT SyncInterval , UINT Flags )
{
    if ( !Renderer->InitSuccess )
    {
        if ( !PointerSwap ( pSwapChain ) )
            return OPresent ( pSwapChain , SyncInterval , Flags );
    }

    if ( Device || Context )
    {
        ID3D11Texture2D* targetTexture = 0;

        if ( !RenderTargetView )
        {
            if ( SUCCEEDED ( pSwapChain->GetBuffer ( 0 , __uuidof( ID3D11Texture2D ) , reinterpret_cast< LPVOID* >( &targetTexture ) ) ) )
            {
                Device->CreateRenderTargetView ( targetTexture , 0 , &RenderTargetView );
                targetTexture->Release ( );
            }
        }
    }

    ImGui_ImplDX11_NewFrame ( );
    ImGui_ImplWin32_NewFrame ( );
    ImGui::NewFrame ( );

    ImGui::GetIO ( ).MouseDrawCursor = Renderer->Open;
    ImGui::GetIO ( ).WantCaptureMouse = Renderer->Open;

    CO->Dialog.Display ( );

    Cheat::Get ( )->Execute ( );
    
    if ( Renderer->Open )
    {
        CGUI::Get ( )->Theme ( );
        CGUI::Get ( )->Render ( );
    }

    ImGui::PushStyleVar ( ImGuiStyleVar_WindowPadding , ImVec2 ( 0 , 0 ) );
    CO->Dialog.Display ( );
    ImGui::PopStyleVar ( );

    if ( CO->Dialog.HasSelected ( ) )
    {
        std::ifstream file ( ( CO->Dialog.GetPwd ( ) / CO->Dialog.GetSelected ( ) ).string ( ).c_str ( ) );
        std::string buffer , output;

        while ( std::getline ( file , buffer ) )
            output += buffer;

        Cheat::Get ( )->ExecuteCode ( output );

        buffer.clear ( );
        output.clear ( );

        CO->Dialog.ClearSelected ( );
    }

    ImGui::EndFrame ( );

    Context->OMSetRenderTargets ( 1 , &RenderTargetView , 0 );
    ImGui::Render ( );

    if ( RenderTargetView )
    {
        RenderTargetView->Release ( );
        RenderTargetView = nullptr;
    }

    ImGui_ImplDX11_RenderDrawData ( ImGui::GetDrawData ( ) );
    return OPresent(pSwapChain , SyncInterval , Flags );
}

bool CRenderer::Initialize ( OverlayType Type )
{
	switch ( Type )
	{
	case OverlayType::Steam:
		m_Hook.Module =  ( "GameOverlayRenderer64.dll" );
		break;
	case OverlayType::Discord:
		m_Hook.Module = _ ( "DiscordHook64.dll" );
		break;
	}

	auto Module = CMem::Get ( )->Module ( _("GameOverlayRenderer64.dll") );

    if ( !Module )
    {
        CMem::Get ( )->Box ( _ ( "Error creating overlay #0" ) );
        return false;
    }

    uintptr_t Present = 0; 
    
    while (!Present)
        Present = CMem::Get ( )->Pattern ( _ ( "GameOverlayRenderer64.dll" ) , ( "48 89 6C 24 18 48 89 74 24 20 41 56 48 83 EC 20 41 8B E8" ) );

    if ( !Present )
    {
        CMem::Get ( )->Box ( _ ( "Error creating overlay #1" ) );
        return false;
    }

    static auto hkAddress = CMem::Get ( )->Pattern (  _( "GameOverlayRenderer64.dll" ) ,  ( "48 ? ? ? ? 57 48 83 EC 30 33 C0" ) );
    auto hook = ( ( ( __int64 ( __fastcall* )( __int64 , __int64 , __int64* , __int64 ) )( hkAddress ) ) );

    hook ( ( __int64 ) Present , ( __int64 ) PresentHook , ( __int64* ) &OPresent , ( __int64 ) 1 );

	return true;
}
