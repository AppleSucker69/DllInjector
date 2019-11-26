#include "MainContents.h"

#define DLLPRESSED 1
#define PROCPRESSED 2
#define INITIATEHACKERNUKE 3

WindowManager wm;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW));

		EndPaint(hwnd, &ps);
	}
	case WM_COMMAND:
		switch (wParam)
		{
		case DLLPRESSED:
			wm.WindowsExplorer();
			break;
		case PROCPRESSED:
			wm.GetProcId();
			break;
		case INITIATEHACKERNUKE:
			wm.PressAllButtonsWithHackOnThem();
			break;
		}
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
{
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = {};

	wc.hInstance = hInstance;
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);
	HWND hwnd = CreateWindowEx(0, CLASS_NAME, L"Dll Injector (that actually works)",
		WS_VISIBLE | WS_OVERLAPPEDWINDOW | WS_BORDER, 0, 0, 400, 200,
		NULL, NULL, NULL, NULL);
	ShowWindow(hwnd, nCmdShow);

	wm.init(hwnd, hInstance);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}