#include "stdafx.h"
#include "CToolbarEX.h"
#include "EVDict.h"
/********************************************************************/


void TaoToolbar(HWND );
/********************************************************************/

CToolbarEX::CToolbarEX()
{
	
}
CToolbarEX::~CToolbarEX()
{
}
/********************************************************************/

HWND CToolbarEX::CreateMainFrame(HWND hWnd, HINSTANCE hInst)
{
	int nImagelist[10];
	HIMAGELIST hImagelist;
	HBITMAP hbmp;
	const int NUMBERBUTTONS = 10;

	//Tao toolbar
	HWND hWndToolbar;
	hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0,
		0, 0, hWnd, (HMENU)IDC_MAIN_TOOL, GetModuleHandle(NULL), NULL);
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	//Toa list anh
	hImagelist = ImageList_Create(20, 20, ILC_MASK | ILC_COLOR32, 3, 0);

	//load tung anh vao list
	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_EToolbar));
	nImagelist[0] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_VToolbar));
	nImagelist[1] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_ButToolbar));
	nImagelist[2] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);


	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_TToolbar));
	nImagelist[3] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_XToolbar));
	nImagelist[4] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_BToolbar));
	nImagelist[5] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_UToolbar));
	nImagelist[6] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	/*hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_SToolbar));
	nImagelist[7] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);*/

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_IToolbar));
	nImagelist[7] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_CToolbar));
	nImagelist[8] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	hbmp = LoadBitmap(hInst, MAKEINTRESOURCE(IDB_FToolbar));
	nImagelist[9] = ImageList_Add(hImagelist, hbmp, (HBITMAP)NULL);
	DeleteObject(hbmp);

	SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)hImagelist);
	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

	//Tao va load anh vao button
	TBBUTTON tbb[NUMBERBUTTONS] =
	{
		{nImagelist[0], ID_DICTIONARY_ENG, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[1], ID_DICTIONARY_VIET, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[2], ID_MENU_UPDATE, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[3], ID_MENU_ADD, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[4], ID_MENU_DELETE, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[5], ID_FORMAT_B, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[6], ID_FORMAT_U, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		//{nImagelist[7], ID_FORMAT_S, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[7], ID_FORMAT_I, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[8], ID_FORMAT_C, TBSTATE_ENABLED, TBSTYLE_BUTTON},
		{nImagelist[9], ID_FORMAT_FONT, TBSTATE_ENABLED, TBSTYLE_BUTTON},
	};
	SendMessage(hWndToolbar, (UINT)TB_ADDBUTTONS, NUMBERBUTTONS, (LPARAM)&tbb);

	if (hWndToolbar) return hWndToolbar;
	else return NULL;
}