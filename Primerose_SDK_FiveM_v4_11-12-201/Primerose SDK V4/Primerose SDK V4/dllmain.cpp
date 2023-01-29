#include "pch.h"
#include "Nt.hpp"
#include "Renderer.hpp"
#include "Memory.hpp"
#include "Auth.hpp"
#include "Options.hpp"

int __stdcall DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    UNREFERENCED_PARAMETER(hInstance);
    UNREFERENCED_PARAMETER(lpReserved);

    if ( dwReason == DLL_PROCESS_ATTACH )
    {
        if ( !CRenderer::Get ( )->Initialize ( OverlayType::Steam ) )
        {
            CMem::Get ( )->Box ( _ ( "1" ) );
            return false;
        }
    }
    else
    {
        return FALSE;
    }

    return TRUE;
}

