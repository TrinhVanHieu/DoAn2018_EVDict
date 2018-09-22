// EVDict.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "EVDict.h"
/***********************************************************/
#include "CToolbarEX.h"
#include "CComboboxEX.h"
#include "CListboxEX.h"
#include "CStatusbarEX.h"
#include "CRicheditEX.h"
#include "Richedit.h"
#include <fstream>
#include <string>




/**********************************************************/
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogErrorDelete(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogDelete(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogDeleteSuccess(HWND, UINT, WPARAM, LPARAM);

/****************************************************************/

void Loaddata(HWND hwndListbox, ifstream &Filein);

void Resize(HWND hWnd);
void ShowMeanEV(HWND hwndRichedit, HWND hWnd, ifstream &filein);


vector<TCHAR*> list;
vector<string> liststr;
string Convertostring(TCHAR tcArr[]); //Tchar* -> string
TCHAR *Convertotchar(string strA); // string -> Tchar *

unsigned int nCheckdialog = 0;
/***************************Hien thi file tren RichEdit****************************/
//+ func ShowMeanEV
#define StreamType_File 1
#define StreamType_Buffer 2
#define StreamType_NA 0

int StreamType = StreamType_NA; //SetStreamType(StreamType_File);
void SetStreamType(int st)
{
	//1 = file, 2 = buffer
	//StreamType_File,  StreamType_Buffer
	StreamType = st;
}
int GetStreamType(void)
{
	return StreamType;
}

static DWORD CALLBACK EditStreamCallback(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	if (StreamType == StreamType_NA)
		return 0;
	else if (StreamType == StreamType_File)	//were we passed a file object?
	{
		fstream *fpntr = (fstream*)dwCookie;	//cast it as a file object


		fpntr->read((char *)pbBuff, (unsigned int)cb);	//read data
		*pcb = (long)fpntr->gcount();	//tells windows when to stop calling the callback function
	}
	else if (StreamType == StreamType_Buffer)	//were we passed a string object?
	{
		string *writes = (string *)dwCookie;	//cast as string

		if (writes->size() < cb)	//check if we are allowed to write rest or not
		{//since the size of the string is less then what were allowed
			*pcb = (long)writes->size();	//set size to string size
			memcpy(pbBuff, (void *)writes->data(), *pcb);	//write rest
			writes->erase();	//erase whole string
		}
		else
		{	//string is too big to write
			*pcb = cb;	//set size to allowed transfer size
			memcpy(pbBuff, (void *)writes->data(), *pcb);	//write allowed amount
			writes->erase(0, cb);	//erase only what we wrote
		}
	}
	return 0;
}
/********************************WinMain**************************************/
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EVDICT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EVDICT));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EVDICT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hwndToolbar;
	static HWND hwndCombobox;
	static HWND hwndListbox;
	static HWND hwndStatus;
	static HWND hwndRichedit;
	
	string EV = "Eng_Viet";
	string VE = "Viet_Eng";
	static int chon = 0;
	static int flag = 1;
	static int lbItem;//chi so cua phan tu trong listbox
    switch (message)
    {
	case WM_CREATE:
	{

		CToolbarEX Toolbar;
		CComboboxEX Combobox;
		CListboxEX Listbox;
		CStatusbarEX Status;
		CRicheditEX Richedit;

		hwndToolbar = Toolbar.CreateMainFrame(hWnd, hInst);
		hwndCombobox = Combobox.CreateMainFrame(hWnd);
		hwndListbox = Listbox.CreateMainFrame(hWnd);
		hwndStatus = Status.CreateMainFrame(hWnd);
		hwndRichedit = Richedit.CreateMainFrame(hWnd);

		
		/*************Load data*********/
		ifstream Filein("Data/" + EV + "/ListData/ListData.txt", ios_base::in);
		Loaddata(hwndListbox, Filein);
	}
		break;
	case WM_SIZE:
	{
		Resize(hWnd);
	}
		break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_MAIN_LISTBOX:
			{
				switch (HIWORD(wParam))
				{
				case LBN_DBLCLK:
				{

					// Get selected index.
					lbItem = (int)SendMessage(hwndListbox, LB_GETCURSEL, 0, 0);
					// Get item data.
					//int i = (int)SendMessage(hwndListbox, LB_GETITEMDATA, lbItem, 0);
					string a;
					if (flag == 1)
					{
						a = "Data/Eng_Viet/Data/";
					}
					else
					{
						a = "Data/Viet_Eng/Data/";
					}
					string b = liststr[lbItem];
					string c = ".txt";
					string buff = a + b + c;
					

					ifstream Filein(buff, ios_base::in);
					ShowMeanEV(hwndRichedit, hWnd, Filein);
					Filein.close();
					chon = 1;
				}
					break;
				}
			}
				break;
			
			case ID_DICTIONARY_ENG:
			{
				flag = 1;
				SetWindowText(hwndRichedit, NULL);
				list.clear();
				liststr.clear();
				SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
				/***************Load data************/
				ifstream Filein("Data/" + EV + "/ListData/ListData.txt", ios_base::in);
				Loaddata(hwndListbox, Filein);
				/*********************Set Status*********************/
				SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)_T("Eng - Viet Dictionary"));
			}
			break;
			case ID_DICTIONARY_VIET:
			{
				flag = 0;
				SetWindowText(hwndRichedit, NULL);
				list.clear();
				liststr.clear();
				SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
				/***************Load data************/
				ifstream Filein("Data/" + VE + "/ListData/ListData.txt", ios_base::in);
				
				Loaddata(hwndListbox, Filein);
				/*********************Set Status*********************/
				SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)_T("Viet - Eng Dictionary"));
			}
				break;
			case ID_MENU_UPDATE:
			{
				/******Delete data in List********************/
				
				MessageBox(hWnd, _T("Update"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
			}
				break;
			case ID_MENU_ADD:
			{
				/*ofstream Fileout("Fileout.txt", ios_base::out);
				TCHAR buff[] = _T("hello anh chị em");
				string s = "o ó ò õ ọ";
				
				if (!Fileout)
				{
					MessageBox(NULL, _T("Khong mo dc file "), _T("Error"), MB_ICONWARNING | MB_OK);
				}
				Fileout << s;
				Fileout.close();*/
				//MessageBox(hWnd, _T("Add"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
			}
				break;
			case ID_MENU_DELETE:
			{
				if (chon == 1)
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDelete), hWnd, DialogDelete);
					if (nCheckdialog == 1)
					{
						list.erase(list.begin() + lbItem); //delete item in list
						liststr.erase(liststr.begin() + lbItem); //delete item in liststr

						size_t sizelist = list.size();//size of list

						/******Delete data in RichEdit********************/
						SetWindowText(hwndRichedit, NULL);

						/******Delete data in List********************/
						SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
						/**********************Update new file**********************/
						ofstream Fileout;
						if (flag == 1) Fileout.open("Data/" + EV + "/ListData/ListData.txt", ios_base::out);
						else Fileout.open("Data/" + VE + "/ListData/ListData.txt", ios_base::out);
						for (int i = 0; i < sizelist; i++)
						{
							if (i < sizelist - 1) Fileout  << liststr[i] << "\n";
							else Fileout << liststr[i];
						}
						Fileout.close();

						/******************Load data******************************/
						for (int i = 0; i < sizelist; i++)
						{
							SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
						}

						chon = 0;
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteSuccess), hWnd, DialogDeleteSuccess);
					}
				}
				else
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogErrorUpdate), hWnd, DialogErrorDelete);
				}
			}
				break;
			case ID_FORMAT_FONT:
			{
				MessageBox(hWnd, _T("Font"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
			}
				break;
			case ID_FORMAT_B:
			{
				MessageBox(hWnd, _T("Bold"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
			}
				break;
			case ID_FORMAT_U:
			{
				MessageBox(hWnd, _T("Underline"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
			}
				break;
			case ID_FORMAT_I:
			{
				MessageBox(hWnd, _T("Italic"), _T("Infor"), MB_ICONINFORMATION | MB_OK); 
			}
				break;
			case ID_FORMAT_C:
			{
				MessageBox(hWnd, _T("Color"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
			}
				break;
            case ID_HELP_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case ID_MENU_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: Add any drawing code that uses hdc here...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hDlg, &ps);
	}
		break;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
INT_PTR CALLBACK DialogErrorDelete(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hDlg, &ps);
	}
	break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK DialogDelete(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hDlg, &ps);
	}
	break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			if(LOWORD(wParam) == IDOK) nCheckdialog = 1;
			else nCheckdialog = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK DialogDeleteSuccess(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_PAINT:
	{

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hDlg, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hDlg, &ps);
	}
	break;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
/********************************************************************/
void Resize(HWND hWnd)
{
	HWND hTool;
	RECT rcTool;
	int nToolHeight;

	HWND hStatus;
	RECT rcStatus;
	int nStatusHeight;

	//HWND hCombobox;
	

	HWND hListBox;
	

	HWND hRichEdit;
	RECT rcRichEdit;
	int nEditHeight;

	hTool = GetDlgItem(hWnd, IDC_MAIN_TOOL);
	SendMessage(hTool, TB_AUTOSIZE, 0, 0);
	GetWindowRect(hTool, &rcTool);
	nToolHeight = rcTool.bottom - rcTool.top;

	hStatus = GetDlgItem(hWnd, IDC_MAIN_STATUS);
	SendMessage(hStatus, WM_SIZE, 0, 0);
	GetWindowRect(hStatus, &rcStatus);
	nStatusHeight = rcStatus.bottom - rcStatus.top;

	//hCombobox = GetDlgItem(hWnd, IDC_MAIN_COMBOBOX);



	hListBox = GetDlgItem(hWnd, IDC_MAIN_LISTBOX);
	


	hRichEdit = GetDlgItem(hWnd, IDC_MAIN_EDIT);
	GetClientRect(hWnd, &rcRichEdit);
	nEditHeight = rcRichEdit.bottom - nToolHeight - nStatusHeight;
	

	SetWindowPos(hListBox, NULL, 0, 55, 200, nEditHeight - 20, SWP_NOZORDER);
	SetWindowPos(hRichEdit, NULL, 200, nToolHeight, rcRichEdit.right - 200, nEditHeight, SWP_NOZORDER);
}

void ShowMeanEV(HWND hwndRichedit, HWND hWnd, ifstream &filein)
{
	EDITSTREAM editstream;
	if (filein.fail())
	{
		filein.close();
		MessageBox(hWnd, _T("Error opening file"), _T("Error"), MB_OK);
		return;
	}
	else
	{
		//ShowMeanEV(fileio, editstream, hWnd, hwndRichedit);
		SetStreamType(StreamType_File);
		editstream.pfnCallback = EditStreamCallback;	//tell it the callback function
		editstream.dwCookie = (unsigned long)&filein;	//pass the file object through cookie
		SendMessage(hwndRichedit, EM_STREAMIN, SF_TEXT, (LPARAM)&editstream); //tell it to start stream in
		//filein.close();
	}
}
void Loaddata(HWND hwndListbox, ifstream &Filein)
{
	if (!Filein)
	{
		MessageBox(NULL, _T("Khong mo dc file"), _T("Error"), MB_ICONWARNING | MB_OK);
	}
	//Filein.seekg(3);
	while (!Filein.eof())
	{
		string buff;
		getline(Filein, buff);
		liststr.push_back(buff);
	}
	Filein.close();
	
	size_t sizeliststr = liststr.size();
	for (int i = 0; i < sizeliststr; i++)
	{
		TCHAR *buff = Convertotchar(liststr[i]);
		list.push_back(buff);
	}
	
	size_t sizeList = list.size();
	for (unsigned int i = 0; i < sizeList; i++)
	{
		SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
	}
}
string Convertostring(TCHAR tcArr[])
{
	wstring arrw(tcArr); // Tchar* -> wstring
	string arrs(arrw.begin(), arrw.end()); 
	return arrs;
}
TCHAR *Convertotchar(string strA)
{
	TCHAR *buff = new TCHAR[strA.size() + 1];
	buff[strA.size()] = 0;
	copy(strA.begin(), strA.end(), buff);
	return buff;
}
