// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "dllmain.tmh"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // https://monoist.atmarkit.co.jp/mn/articles/1310/10/news001_2.html
        WPP_INIT_TRACING(L"Dll1");
        TraceEvents(TRACE_LEVEL_INFORMATION, DMFT_INIT, "DLL Loaded");
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        WPP_CLEANUP();
        break;
    }
    return TRUE;
}

