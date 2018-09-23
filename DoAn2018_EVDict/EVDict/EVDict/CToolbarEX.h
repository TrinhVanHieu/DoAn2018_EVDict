#pragma once

#define IDC_MAIN_TOOL 2001
#define ID_FORMAT_B 2004
#define ID_FORMAT_U 2005
#define ID_FORMAT_I 2006
#define ID_FORMAT_C 2007
#pragma comment (lib, "comctl32.lib") 
class CToolbarEX
{
public:
	CToolbarEX();
	
	~CToolbarEX();

	HWND CreateMainFrame(HWND hWnd, HINSTANCE hInst);
};
