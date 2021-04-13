#include <Windows.h>
#include <iostream>
#include <afxres.h>
#include<string.h>
#include <string>
using namespace std;

MSG message;
HWND EnableCamera;
HWND ShotPicture;
HWND hwnd;
HWND result;
HWND format;
HWND video_source;
HWND ShotVideo;
HWND zmniejsz;
HWND zwieksz;
int ostr = 0;

static const int WM_CAP = WM_USER;
static const int WM_CAP_DRIVER_CONNECT = WM_CAP + 10;
static const int WM_CAP_DRIVER_DISCONNECT = WM_CAP + 11;
static const int WM_CAP_SET_PREVIEW = WM_CAP + 50;
static const int WM_CAP_SET_PREVIEWRATE = WM_CAP + 52;
static const int WM_CAP_SET_SCALE = WM_CAP + 53;
static const int WM_CAP_FILE_SET_CAPTURE_FILE = WM_CAP + 20;
static const int WM_CAP_SEQUENCE = WM_CAP + 62;
static const int WM_CAP_STOP = WM_CAP + 68;
static const int WM_CAP_FILE_SAVEAS = WM_CAP + 23;
static const int WM_CAP_FILE_SAVEDIB = WM_CAP + 25;
static const int WM_CAP_DLG_VIDEOSOURCE = WM_CAP + 42;
static const int WM_CAP_DLG_VIDEOFORMAT = WM_CAP + 41;

typedef HWND(__stdcall *capCreateCaptureWindowA)(PCHAR lpszWindowName, long dwStyle, int x, int y, int nWidth, int nHeight, HWND ParentWin, int nId);
typedef BOOL(__stdcall *capGetDriverDescriptionA)(WORD wDriverIndex, LPTSTR lpszName, int cbName, LPTSTR lpszVer, int cbVer);

static capCreateCaptureWindowA _capCreateCaptureWindowA = NULL;
static capGetDriverDescriptionA _capGetDriverDescriptionA = NULL;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


bool capInit()
{
	HINSTANCE hAviCap = LoadLibrary((LPCTSTR)L"avicap32.dll");

	_capCreateCaptureWindowA = (capCreateCaptureWindowA)GetProcAddress(hAviCap, "capCreateCaptureWindowA");
	_capGetDriverDescriptionA = (capGetDriverDescriptionA)GetProcAddress(hAviCap, "capGetDriverDescriptionA");

	return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPSTR NazwaKlasy = "Okienko";
	//Wypełnienie struktury
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = (LPCWSTR)NazwaKlasy;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, (LPCWSTR)L"Odmowa !!", (LPCWSTR)L"...",
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}
	//Tworzenie okna
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, (LPCWSTR)NazwaKlasy, (LPCWSTR)L"Kamerka", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

	//Tworzenie przycisków
	ShotPicture = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Zrob zdjecie", WS_CHILD | WS_VISIBLE,
		10, 10, 100, 35, hwnd, NULL, hInstance, NULL);

	ShotVideo = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Nagraj film", WS_CHILD | WS_VISIBLE,
		10, 60, 100, 35, hwnd, NULL, hInstance, NULL);

	format = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Rozdzielczosc", WS_CHILD | WS_VISIBLE,
		10, 110, 100, 35, hwnd, NULL, hInstance, NULL);

	video_source = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"Ustawienia", WS_CHILD | WS_VISIBLE,
		10, 160, 100, 35, hwnd, NULL, hInstance, NULL);

	zwieksz = CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"+", WS_CHILD | WS_VISIBLE,
		10, 210, 100, 35, hwnd, NULL, hInstance, NULL);

	zmniejsz= CreateWindowEx(0, (LPCWSTR)L"BUTTON", (LPCWSTR)L"-", WS_CHILD | WS_VISIBLE,
		10, 260, 100, 35, hwnd, NULL, hInstance, NULL);


	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	capInit();
	result = _capCreateCaptureWindowA(NULL, WS_VISIBLE | WS_CHILD, 120,10,640, 480,	hwnd, 1);

	if (::SendMessage(result, WM_CAP_DRIVER_CONNECT, (WPARAM)0, 0L)) //łączenie z urzadzeniem
	{
		::SendMessage(result, WM_CAP_SET_SCALE, true, 0);		//wlaczenie skalowanie
		::SendMessage(result, WM_CAP_SET_PREVIEWRATE, 0x3c, 0);	//prędkość odświeżania 30hz
		::SendMessage(result, WM_CAP_SET_PREVIEW, true, 0);	//wlaczenie trybu podglądu
	}

	//Odczytywanie komunikatow z przyciskow
	while (GetMessage(&message, NULL, 0, 0))
	{
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return message.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		if ((HWND)lParam == video_source)
		{
			::SendMessage(result, WM_CAP_DLG_VIDEOSOURCE, 0, 0);
		}
		if ((HWND)lParam == format)
		{
			::SendMessage(result, WM_CAP_DLG_VIDEOFORMAT, 0, 0);
		}
		if ((HWND)lParam == ShotPicture)
		{
			::SendMessage(result, WM_CAP_FILE_SAVEDIB, 0, (LPARAM)"1.jpg");
		}

		if ((HWND)lParam == ShotVideo)
		{
			bool recording_now = false;
			if (recording_now)
			{
				::SendMessage(result, WM_CAP_STOP, 0, (LPARAM)"f1.avi");
				recording_now = false;
			}
			else
			{
				::SendMessage(result, WM_CAP_FILE_SET_CAPTURE_FILE, 0, (LPARAM)"f1.avi");
				::SendMessage(result, WM_CAP_SEQUENCE, 0, 0);
				recording_now = true;
			}
		}
		if ((HWND)lParam == zmniejsz)
		{
			if (ostr > 0)
			{
				ostr -= 5;
				string c = std::to_string(ostr);
				MessageBoxA(NULL, c.c_str(), "Zmiana zakresu", MB_ICONINFORMATION);
			}
		}
		if ((HWND)lParam == zwieksz)
		{
			if (ostr < 100)
			{
				ostr += 5;
				string c = std::to_string(ostr);
				MessageBoxA(NULL, c.c_str() , "Zmiana zakresu", MB_ICONINFORMATION);
			}
		}
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}