#include "stdafx.h"
#include "CComboboxEX.h"
#include "EVDict.h"

CComboboxEX::CComboboxEX()
{
}


CComboboxEX::~CComboboxEX()
{
}
HWND CComboboxEX::CreateMainFrame(HWND hWnd)
{
	HWND hCombobox = CreateWindowEx(WS_EX_CLIENTEDGE, _T("Combobox"), _T(""), WS_CHILD | WS_VISIBLE |
		WS_VSCROLL | CBS_DROPDOWN, 0, 30, 170, 100, hWnd, (HMENU)IDC_MAIN_COMBOBOX, GetModuleHandle(NULL), NULL);
	return hCombobox;
}