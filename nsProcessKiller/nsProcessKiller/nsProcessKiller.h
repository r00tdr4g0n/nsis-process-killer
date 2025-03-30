#pragma once

#ifdef NSPROCESSKILLER_EXPORTS
#define NSPROCESSKILLER_API __declspec(dllexport)
#else
#define NSPROCESSKILLER_API __declspec(dllimport)
#endif

#ifndef NSISCALL
#define NSISCALL WINAPI
#endif

#define NSIS_MAX_LEN 1024

#define DLL_INIT() { \
	g_stringSize = a_stringSize; \
	g_stackTop = a_stackTop; \
	g_variables = a_variables; \
}

typedef struct _stack_t {
	struct _stack_t* next;
	TCHAR text[1];
} stack_t;

extern UINT g_stringSize;
extern stack_t** g_stackTop;
extern LPTSTR g_variables;

void DebugString(LPCTSTR a_str, ...);
int NSISCALL PopString(LPCTSTR a_str, int a_maxLen);
void NSISCALL PushString(LPCTSTR a_str);

extern "C" NSPROCESSKILLER_API void _Test(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop);
extern "C" NSPROCESSKILLER_API void _FindProcess(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop);
//extern "C" NSPROCESSKILLER_API void _FindProcessUsingDll(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop);
extern "C" NSPROCESSKILLER_API void _KillProcess(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop);