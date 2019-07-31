#include "BW Primitives Importer.h"

extern ClassDesc2* GetBW_Primitives_ImporterDesc();

HINSTANCE hInstance;
int controlsInit = FALSE;

BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID /*lpvReserved*/)
{
    if( fdwReason == DLL_PROCESS_ATTACH )
    {
        hInstance = hinstDLL;
        DisableThreadLibraryCalls(hInstance);
    }
    return(TRUE);
}

__declspec( dllexport ) const TCHAR* LibDescription()
{
    return GetString(IDS_LIBDESCRIPTION);
}

__declspec( dllexport ) int LibNumberClasses()
{
    return 1;
}

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
    switch(i) {
        case 0: return GetBW_Primitives_ImporterDesc();
        default: return 0;
    }
}

__declspec( dllexport ) ULONG LibVersion()
{
    return VERSION_3DSMAX;
}

__declspec( dllexport ) int LibInitialize(void)
{
    return TRUE; // TODO: Perform initialization here.
}

__declspec( dllexport ) int LibShutdown(void)
{
    return TRUE; // TODO: Perform un-initialization here.
}

TCHAR *GetString(int id)
{
    static TCHAR buf[256];

    if (hInstance)
        return LoadString(hInstance, id, buf, sizeof(buf)) ? buf : NULL;
    return NULL;
}
