#include "stdafx.h"
#include "CRicheditEX.h"
#include "EVDict.h"
#include <RichEdit.h>
CRicheditEX::CRicheditEX()
{
}


CRicheditEX::~CRicheditEX()
{
}
HWND CRicheditEX::CreateMainFrame(HWND hWnd)
{
	LoadLibrary(_T("msftedit.dll"));
	HWND	hRichEdit = CreateWindowEx(WS_EX_CLIENTEDGE | WS_EX_DLGMODALFRAME, MSFTEDIT_CLASS, NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		250, 5, 866, 440, hWnd, (HMENU)IDC_MAIN_EDIT, GetModuleHandle(NULL), NULL);

	return hRichEdit;
}
