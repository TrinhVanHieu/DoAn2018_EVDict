#include "stdafx.h"
#include "CStatusbarEX.h"
#include "EVDict.h"

CStatusbarEX::CStatusbarEX()
{
}


CStatusbarEX::~CStatusbarEX()
{
}
HWND CStatusbarEX::CreateMainFrame(HWND hWnd)
{
	HWND hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE |
		SBARS_SIZEGRIP, 0, 0, 0, 0, hWnd, (HMENU)IDC_MAIN_STATUS, GetModuleHandle(NULL), NULL);

	int statwidths[] = { 150, -1 };
	SendMessage(hStatus, SB_SETPARTS, sizeof(statwidths) / sizeof(int), (LPARAM)statwidths);
	SendMessage(hStatus, SB_SETTEXT, 0, (LPARAM)_T("Eng - Viet Dictionary"));
	return hStatus;
}