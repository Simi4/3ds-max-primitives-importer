#pragma once
#include <tchar.h>
#include <string>
#include <vector>
#include <map>

void InfoMsg(const TCHAR* msg);
void ErrorMsg(const TCHAR* msg);

#define INFO_MSG(X) InfoMsg(_T(X))
#define ERROR_MSG(X) ErrorMsg(_T(X))
