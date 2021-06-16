///*
//	Freelancer project:
//	
//
//	Developed by Secured Globe, Inc.
//	https://www.securedglobe.net
//
//	January 2021
//
//*/
#include "stdafx.h"
#include "utils.h"
#include "GetGoogleSheets.h"
#include "SG_GetGoogleSheets.h"

HANDLE hConsole;


#define ENDPOINT  Texas
#define LOG_COLOR_WHITE 7
#define LOG_COLOR_GREEN 10
#define LOG_COLOR_YELLOW 14 
#define LOG_COLOR_MAGENTA 13
#define LOG_COLOR_CIAN 11
#define BUFSIZE MAX_PATH
inline void setcolor(int textcol, int backcol)
{
	if ((textcol % 16) == (backcol % 16))textcol++;
	textcol %= 16; backcol %= 16;
	unsigned short wAttributes = ((unsigned)backcol << 4) | (unsigned)textcol;
	SetConsoleTextAttribute(hConsole, wAttributes);
}
void refresh()
{
	HWND hwnd = FindWindowEx(NULL, NULL, L"CabinetWClass", NULL);
	while (hwnd != NULL)
	{
		PostMessage(hwnd, WM_COMMAND, 41504, 0);
		hwnd = FindWindowEx(NULL, hwnd, L"CabinetWClass", NULL);
	}
}

void DisplayMainMenu()
{
	bool sys_exists = false, inf_exists = false, drv_exists = false;
	bool file_exists = false;
	int ch = -1;

	while (-1)
	{
		system("cls");
		refresh();
		setcolor(LOG_COLOR_YELLOW, 0);
		wprintf(L"Load Google Sheets tool - by Secured Globe, Inc.\n");
		setcolor(LOG_COLOR_CIAN, 0);
		wprintf(L"(c)2021 Secured Globe, Inc \n1501 Broadway Ave.Suite 1200, New York 10036, NY\nPhone: (646) 480 - 0506 Email : info@securedglobe.net Skype : 'securedglobe'\n");
		setcolor(LOG_COLOR_GREEN, 0);
		wprintf(L"                                                                               \n\n\n");
		setcolor(LOG_COLOR_WHITE, 0);

		setcolor(LOG_COLOR_MAGENTA, 0);
#ifdef _M_X64
		wprintf(L"You are running a x64 version\n\n");
#else
		wprintf(L"You are running a x86 version\n\n");
#endif
		setcolor(LOG_COLOR_GREEN, 0);
		wprintf(L"\n\t\tO P T I O N S :\n");
		wprintf(L"[1]\t\tLoad a Google Sheets document\n");
		wprintf(L"[q]\t\tQuit\t\n");
		setcolor(LOG_COLOR_WHITE, 0);
		string outputFile{ "" };

		ch = _getche();
		wprintf(L"%c", 8);
		if (ch == L'1')
		{
			HandleGoogleSheets("https://docs.google.com/spreadsheets/d/1EuB04RZI1uOogKXhIoNgWsmnAmS6NL0zC0WstRXUiQw/edit?usp=sharing");
			// 
		}
		if (ch == L'q')
		{
			break;
		}
		system("pause");
		ch = _getche();
	}
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPWSTR    lpCmdLine,
	int       nCmdShow)
{
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	LPWSTR *szArglist{ NULL };
	int nArgs;
	int i;

	szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if (szArglist == NULL)
	{
		wprintf(L"CommandLineToArgvW failed\n");
		return 0;
	}
	else 
	for (i = 0; i < nArgs; i++) 
		wprintf(L"%d: %ws\n", i, szArglist[i]);


	TCHAR szExeFileName[MAX_PATH];
	GetModuleFileName(NULL, szExeFileName, MAX_PATH);
	PathStripPath(szExeFileName);


	HANDLE hMutex;
	hMutex = CreateMutex(
		NULL,					// default security descriptor
		FALSE,                  // mutex not owned
		MUTEX_NAME);			// mutext name

	if (hMutex == NULL)
		WriteLogFile(ERR_MUTEXCREATIONERROR, GetLastError());
	else
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		WriteLogFile(L"Software already running\n");
		exit(0);
	}

	WriteLogFile(L"Welcome to %s\n%d args were sent\n\n", szExeFileName, nArgs - 1);

	DisplayMainMenu();
	return 0;
}