#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>

class WindowManager
{
public:
	void init(const HWND& hwnd, const HINSTANCE& hinst);

	void PressAllButtonsWithHackOnThem();
	void WindowsExplorer();
	void GetProcId();
	static void GetThreadId();
private:
	static void error(const char* str1, const char* str2);
	void GetPath(const char* str);

	static LRESULT CALLBACK ProcessFinder(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE hInstance;

	HWND DllPath;
	HWND procList;

	char dll[MAX_PATH];
};