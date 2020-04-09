#include <windows.h>
#include <stdio.h>
#include <corecrt_startup.h>

#if 0
int APIENTRY wWinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nShowCmd
	)
{
#else
int wmain()
{
	auto lpCmdLine = _get_wide_winmain_command_line();
	auto startTick = GetTickCount();
#endif
#if 0
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.nShow = SW_SHOWNORMAL;
	sei.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_NOASYNC | SEE_MASK_WAITFORINPUTIDLE;
	sei.lpFile = lpCmdLine;
	sei.lpVerb = L"open";
	//sei.lpParameters = lpCmdLine;
	if (ShellExecuteEx(&sei))
	{
		fprintf(stderr, "MeasureStartup: %u ms\n", GetTickCount() - startTick);

		CloseHandle(sei.hProcess);
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}
#else
	STARTUPINFO si = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION pi = {};
	if (CreateProcess(nullptr, lpCmdLine, nullptr, nullptr, false, 0, nullptr, nullptr, &si, &pi))
	{
		HWND result{};
		DWORD ret = WaitForInputIdle(pi.hProcess, INFINITE);
		switch (ret)
		{
		case 0:
			fprintf(stderr, "MeasureStartup: %u ms\n", GetTickCount() - startTick);
			EnumThreadWindows(pi.dwThreadId, [](HWND hWnd, LPARAM param) -> BOOL {
				if (GetWindow(hWnd, GW_OWNER) == nullptr) {
					*(reinterpret_cast<HWND*>(param)) = hWnd;
					return TRUE;
				}
				return FALSE;
			}, reinterpret_cast<LPARAM>(&result));
			if(result)
			{
				SetForegroundWindow(result);
			}
			break;
		case WAIT_TIMEOUT:
			break;
		default:
			fprintf(stderr, "MeasureStartup error.(this program has't Message Queue)\n");
			break;

		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
		return EXIT_SUCCESS;
	}
	else
	{
		return EXIT_FAILURE;
	}

#endif
}
