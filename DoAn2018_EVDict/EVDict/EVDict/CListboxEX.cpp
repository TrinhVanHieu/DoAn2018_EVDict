#include "stdafx.h"
#include "CListboxEX.h"
#include "EVDict.h"

CListboxEX::CListboxEX()
{
}


CListboxEX::~CListboxEX()
{
}
HWND CListboxEX::CreateMainFrame(HWND hWnd)
{
	HWND hListbox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("listbox"), _T(""), WS_CHILD | WS_VISIBLE | 
		WS_VSCROLL |  LBS_NOTIFY, 0, 55, 200, 100, hWnd, (HMENU)IDC_MAIN_LISTBOX, GetModuleHandle(NULL), NULL);
	return hListbox;
}