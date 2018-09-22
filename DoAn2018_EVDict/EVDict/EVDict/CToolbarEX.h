#pragma once

#define IDC_MAIN_TOOL 1001
#define ID_FORMAT_B 1004
#define ID_FORMAT_U 1005
#define ID_FORMAT_I 1006
#define ID_FORMAT_C 1007
#define ID_FORMAT_S 1008
#pragma comment (lib, "comctl32.lib") 
class CToolbarEX
{
public:
	CToolbarEX();
	
	~CToolbarEX();

	HWND CreateMainFrame(HWND hWnd, HINSTANCE hInst);
};
