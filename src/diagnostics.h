#pragma once

#include <windows.h>

void LogStartupEvent(const wchar_t* message);
void LogLastError(const wchar_t* stage, DWORD error);
void ShowFatalStartupError(const wchar_t* stage, DWORD error);
void ShowTrayIconStartupWarning(DWORD error);
