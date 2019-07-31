#pragma once
#include <string>
#include <string_view>
#include <tchar.h>

void InfoMsg(const TCHAR* msg);
void ErrorMsg(const TCHAR* msg);

#define INFO_MSG(X) InfoMsg(_T(X))
#define ERROR_MSG(X) ErrorMsg(_T(X))
