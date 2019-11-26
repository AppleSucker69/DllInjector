#include "MainContents.h"

HWND main;
HWND targetId;

int prevScroll;
DWORD idBuffer;

bool threadNotOpened = true;

LRESULT WindowManager::ProcessFinder(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		EnableWindow(main, 0);
		break;
	case WM_CLOSE:
		EnableWindow(main, 1);
		threadNotOpened = true;
		break;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_THUMBTRACK:
			ScrollWindow(hwnd, 0, (prevScroll - HIWORD(wParam)), NULL, NULL);
			prevScroll = HIWORD(wParam);
			SetScrollPos(hwnd, SB_VERT, prevScroll, TRUE);
		}
		break;
	case WM_COMMAND:
		if (threadNotOpened)
			GetThreadId();
		else
			EnableWindow(main, 1);

		idBuffer = (DWORD)wParam;
		SetWindowTextA(targetId, std::to_string(idBuffer).c_str());
		DestroyWindow(hwnd);
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowManager::error(const char* str2, const char* str1)
{
	MessageBoxA(main, str1, str2, MB_ICONERROR);
	DestroyWindow(main);
}

void WindowManager::GetPath(const char* str)
{
	for (int i = 0; i < MAX_PATH; i++)
		dll[i] = str[i];
}

void WindowManager::init(const HWND& hwnd, const HINSTANCE& hinst)
{
	main = hwnd;
	hInstance = hinst;

	DllPath = CreateWindowA("Edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL,
		0, 20, 150, 20, main, NULL, NULL, NULL);
	targetId = CreateWindowA("Edit", "", WS_VISIBLE | WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | WS_DISABLED,
		200, 20, 150, 20, main, NULL, NULL, NULL);

	CreateWindowA("Button", "..", WS_VISIBLE | WS_CHILD,
		150, 20, 20, 20, main, HMENU(1), NULL, NULL);
	CreateWindowA("Button", "..", WS_VISIBLE | WS_CHILD,
		350, 20, 20, 20, main, HMENU(2), NULL, NULL);
	CreateWindowA("Button", "Extract Deep Web Hacker Nuke", WS_VISIBLE | WS_CHILD,
		0, 60, 385, 100, main, HMENU(3), NULL, NULL);
	CreateWindowA("Static", "The Dll You Want To Inject", WS_VISIBLE | WS_CHILD,
		0, 0, 180, 15, main, NULL, NULL, NULL);
	CreateWindowA("Static", "The Target Id", WS_VISIBLE | WS_CHILD,
		200, 0, 180, 15, main, NULL, NULL, NULL);
}

void WindowManager::WindowsExplorer()
{
	OPENFILENAMEA ofn;
	CHAR file_name[MAX_PATH];
	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.lpstrFile = file_name;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.hwndOwner = main;
	ofn.lpstrFilter = "Dlls\0*.DLL\0";
	ofn.nFilterIndex = 1;

	GetOpenFileNameA(&ofn);
	GetPath(ofn.lpstrFile);
	SetWindowTextA(DllPath, dll);
}

void WindowManager::GetProcId()
{
	HANDLE procSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	PROCESSENTRY32 procEntry;
	procEntry.dwSize = sizeof(PROCESSENTRY32);

	if (procSnap == INVALID_HANDLE_VALUE)
		error("Couldn't snapshot processes", "Snapshotting Processes");
	else
	{
		WNDCLASSW wc = {};
		wc.lpfnWndProc = ProcessFinder;
		wc.lpszClassName = L"IdFinder";
		wc.hInstance = hInstance;
		RegisterClass(&wc);

		procList = CreateWindow(L"IdFinder", L"Process Id Finder", WS_VISIBLE | WS_SYSMENU | WS_BORDER | WS_VSCROLL,
			0, 0, 200, 500, main, NULL, NULL, NULL);

		int index = 0;

		Process32First(procSnap, &procEntry);
		while (Process32Next(procSnap, &procEntry))
		{	CreateWindow(L"Button", procEntry.szExeFile, WS_VISIBLE | WS_CHILD,
				0, index, 200, 20, procList, HMENU(procEntry.th32ProcessID), NULL, NULL); index += 20;	}
		SetScrollRange(procList, SB_VERT, 0, index-460, FALSE);
		CloseHandle(procSnap);
	}
}

void WindowManager::GetThreadId()
{
	threadNotOpened = false;

	THREADENTRY32 threadEntry;
	threadEntry.dwSize = sizeof(THREADENTRY32);

	HANDLE threadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, NULL);
	if (threadSnap == INVALID_HANDLE_VALUE)
		error("Couldn't snapshot processes", "Snapshotting Processes");
	else {

		Thread32First(threadSnap, &threadEntry);

		HWND threadList = CreateWindow(L"IdFinder", L"Thread Id Finder", WS_VISIBLE | WS_BORDER | WS_SYSMENU | WS_VSCROLL,
			0, 0, 400, 200, NULL, NULL, NULL, NULL);
		int index = 0;
		while (Thread32Next(threadSnap, &threadEntry))
			if (threadEntry.th32OwnerProcessID == idBuffer && threadEntry.th32ThreadID != NULL)
				{CreateWindow(L"Button", std::to_wstring(threadEntry.th32ThreadID).c_str(), WS_VISIBLE | WS_CHILD,
				0, index, 400, 20, threadList, (HMENU)threadEntry.th32ThreadID, NULL, NULL); index += 20;}

		if (index == 0) 
		{DestroyWindow(threadList);
		threadNotOpened = true;
		EnableWindow(main, 1);}
		else
			SetScrollRange(threadList, SB_VERT, 0, index - 160, FALSE);
		CloseHandle(threadSnap);
	}
}

void WindowManager::PressAllButtonsWithHackOnThem()
{
	if (targetId == NULL)
		error("Getting Processe's Id", "Failed initializing variable with processes id. Or you haven't entered one.");
	else 
	{
		HANDLE openProc = OpenProcess(PROCESS_ALL_ACCESS, NULL, idBuffer);
		if (!openProc)
			error("Opening Process", "Failed getting handle to process or process isn't open");
		else 
		{
			void* allocatedMem = VirtualAllocEx(openProc, nullptr, MAX_PATH,
				MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
			if (!allocatedMem)
				error("Allocating Memory", "Couldn't allocate in target programs memory");
			else 
			{
				if (!WriteProcessMemory(openProc, allocatedMem, dll, MAX_PATH, nullptr))
					error("Writing Programs Memory", "Failed to write programs memory with the dll or you didn't enter one.");
				else 
				{
					HANDLE rThread = CreateRemoteThread(openProc, nullptr, NULL,
						PTHREAD_START_ROUTINE(LoadLibraryA), allocatedMem, NULL, nullptr);
					if (!rThread)
						error("Creating Thread Inside Process", "Failed creating thread in target program");
					else
					{CloseHandle(openProc); VirtualFreeEx(openProc, allocatedMem, NULL, MEM_RELEASE);
					MessageBoxA(0, "Successfully Injected", "Injected dll into the process", MB_ICONEXCLAMATION);}
				}
			}
		}
	}
}