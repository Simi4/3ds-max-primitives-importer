#include "bw_common.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


void InfoMsg(const TCHAR* msg)
{
	MessageBox(GetActiveWindow(), msg, _T("Information"), MB_ICONINFORMATION);
}

void ErrorMsg(const TCHAR* msg)
{
	MessageBox(GetActiveWindow(), msg, _T("Error"), MB_ICONERROR);
}
