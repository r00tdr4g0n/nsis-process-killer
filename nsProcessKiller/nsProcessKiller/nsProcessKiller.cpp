#include "pch.h"
#include "nsProcessKiller.h"

UINT g_stringSize;
stack_t** g_stackTop;
LPTSTR g_variables;

TCHAR g_debug[NSIS_MAX_LEN];
TCHAR g_buf[NSIS_MAX_LEN];

void DebugString(LPCTSTR a_str, ...)
{
	va_list ap;
	va_start(ap, a_str);
	TCHAR label[] = _T("[nsProcessKiller] ");
	ZeroMemory(g_debug, sizeof(g_debug));
	_stprintf_s(g_debug, label);
	_vstprintf_s(g_debug + _tcslen(label), _countof(g_debug) - _tcslen(label), a_str, ap);
	OutputDebugString(g_debug);
	va_end(ap);
}

int NSISCALL PopString(LPCTSTR a_str, int a_maxLen)
{
	stack_t* s = NULL;

	if (!g_stackTop || !*g_stackTop) return 1;
	
	s = *g_stackTop;
	
	if (a_str) {
		_tcscpy_s((LPTSTR)a_str, a_maxLen, (s->text));
		s->next = *g_stackTop;
		*g_stackTop = s;
	}

	return 0;
}

void NSISCALL PushString(LPCTSTR a_str)
{
	stack_t* s = NULL;
	
	if (!g_stackTop) return;
	
	s = (stack_t*)GlobalAlloc(GPTR, sizeof(stack_t) + g_stringSize * sizeof(TCHAR));
	
	if (s) {
		_tcscpy_s(s->text, g_stringSize, a_str);
		s->next = *g_stackTop;
		*g_stackTop = s;
	}
}

void _Test(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop)
{
	DLL_INIT();
	PopString(g_buf, NSIS_MAX_LEN);
	::MessageBox(NULL, g_buf, L"nsProcessKeller", MB_OK);
	PushString(_T("string from nsProcessKiller.dll"));
}

bool GetProcessName(DWORD a_pid, LPTSTR a_buf, DWORD a_bufSize)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, a_pid);
	bool bResult = false;

	if (hProcess) {
		::GetModuleBaseName(hProcess, NULL, a_buf, a_bufSize);
		CloseHandle(hProcess);
		bResult = true;
	}
	else {
		bResult = false;
	}

	return bResult;
}

void _FindProcess(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop)
{
	DLL_INIT();
	PopString(g_buf, NSIS_MAX_LEN);

	DebugString(_T("Target process is %s"), g_buf);

	DWORD procList[1024] = { 0, };
	DWORD procListSize = 0;
	DWORD procCount = 0;
	TCHAR name[MAX_PATH] = { 0, };

	if (::EnumProcesses(procList, sizeof(procList), &procListSize)) {
		procCount = procListSize / sizeof(DWORD);

		for (DWORD i = 0; i < procCount; i++) {
			ZeroMemory(name, sizeof(name));
			if (GetProcessName(procList[i], name, sizeof(name))) {
				if (!_tcsicmp(name, g_buf)) {
					TCHAR processId[16] = { 0, };
					_itot_s(procList[i], processId, 10);
					PushString(processId);
					DebugString(_T("PID is %d"), procList[i]);
					break;
				}
			}
			else {
				continue;
			}
		}

		PushString(_T("false"));
	}
	else {
		DebugString(_T("Failed to EnumProcesses"));
		PushString(_T("false"));
	}
}

bool WaitForCloseProcess(DWORD a_pid, DWORD a_timeout)
{
	HANDLE hProcess = OpenProcess(SYNCHRONIZE, FALSE, a_pid);
	bool bResult = false;

	if (!hProcess) {
		DebugString(_T("Failed to open process"));
		return false;
	}

	DWORD result = ::WaitForSingleObject(hProcess, a_timeout);

	if (result == WAIT_OBJECT_0) {
		DebugString(_T("Terminated process (pid : %d)"), a_pid);
		bResult = true;
	}
	else if (result == WAIT_TIMEOUT) {
		DebugString(_T("Running process (pid : %d)"), a_pid);
		bResult = false;
	}
	else if (result == WAIT_FAILED) {
		DebugString(_T("Failed to WaitForSingleObject (error : %d)"), GetLastError());
		bResult = false;
	}
	else {
		bResult = false;
	}

	if (!bResult) ::TerminateProcess(hProcess, 0);

	CloseHandle(hProcess);

	return bResult;
}

void _KillProcess(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop)
{
	DLL_INIT();
	PopString(g_buf, NSIS_MAX_LEN);

	DWORD pid = _tcstol(g_buf, NULL, 10);
	DWORD wndPid = 0;

	if (!pid || pid == LONG_MIN || pid == LONG_MAX) {
		DebugString(_T("PID is not valid"));
		return;
	}

	HWND hWnd = ::FindWindow(NULL, NULL);

	while(hWnd) {
		wndPid = 0;
		GetWindowThreadProcessId(hWnd, &wndPid);
		if (wndPid == pid) {
			::PostMessage(hWnd, WM_QUIT, 0, 0);
			
			if (WaitForCloseProcess(pid, 5000)) {
				DebugString(_T("Success to close process"));
			}
			else {
				DebugString(_T("Failed to close process"));
			}
		}
		hWnd = ::GetNextWindow(hWnd, GW_HWNDNEXT);
	}
}

bool IsUsingModule(DWORD a_pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, a_pid);
	HMODULE moduleList[1024] = { 0, };
	DWORD moduleListSize = 0;
	DWORD moduleCount = 0;
	TCHAR name[MAX_PATH] = { 0, };

	bool bResult = false;

	if (!hProcess) {
		DebugString(_T("Failed to open process(pid : %d)"), a_pid);
		return false;
	}

	if (::EnumProcessModules(hProcess, moduleList, sizeof(moduleList), &moduleListSize)) {
		moduleCount = moduleListSize / sizeof(HMODULE);
		for (DWORD i = 0; i < moduleCount; i++) {
			ZeroMemory(name, sizeof(name));
			if (::GetModuleBaseName(hProcess, moduleList[i], name, MAX_PATH)) {
				if (!_tcsicmp(name, g_buf)) {
					bResult = true;
					break;
				}
			}
		}
	}
	else {
		DebugString(_T("Failed to EnumProcessModules(pid : %d)"), a_pid);
		bResult = false;
	}

	CloseHandle(hProcess);

	return bResult;
}

void _FindProcessUsingDll(HWND a_parentHwnd, int a_stringSize, TCHAR* a_variables, stack_t** a_stackTop)
{
	DLL_INIT();
	PopString(g_buf, NSIS_MAX_LEN);

	DebugString(_T("Target module is %s"), g_buf);

	DWORD procList[1024] = { 0, };
	DWORD procListSize = 0;
	DWORD procCount = 0;

	if (::EnumProcesses(procList, sizeof(procList), &procListSize)) {
		procCount = procListSize / sizeof(DWORD);

		DWORD i = 0;
		for (i = 0; i < procCount; i++) {
			if (IsUsingModule(procList[i])) {
				TCHAR processId[16] = { 0, };
				_itot_s(procList[i], processId, 10);
				PushString(processId);
				DebugString(_T("PID is %d"), procList[i]);
				break;
			}
		}

		if (i >= procCount) PushString(_T("false"));
	}
	else {
		DebugString(_T("Failed to EnumProcesses"));
		PushString(_T("false"));
	}
}