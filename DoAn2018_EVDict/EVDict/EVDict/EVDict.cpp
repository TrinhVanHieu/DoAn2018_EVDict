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

//dialog about
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//dialog Delete
INT_PTR CALLBACK    DialogErrorDelete(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogChooseDelete(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogDelete(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogDeleteYesNo(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogDeleteSuccess(HWND, UINT, WPARAM, LPARAM);

//dialog Update
INT_PTR CALLBACK    DialogErrorUpdate(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogUpdateYesNo(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogUpdateSuccess(HWND, UINT, WPARAM, LPARAM);

//dialog Add
INT_PTR CALLBACK    DialogAdd(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogErrorAdd(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogAddYesNo(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    DialogAddNewUpdate(HWND, UINT, WPARAM, LPARAM);
/****************************************************************/
#define ID_SEARCH 2015 //ID button

void Loaddata(HWND hwndListbox, ifstream &Filein);
void Loaddata2(HWND hwndListbox, ifstream &Filein);
void Resize(HWND hWnd);
void ShowMeanEV(HWND hwndRichedit, HWND hWnd, ifstream &filein);

void CreateWriteFileUTF_8(string s, TCHAR *swork); // viet du lieu TCHAR* ra file UTF- 8
TCHAR *ReadRichEdit(HWND hwndRichedit); //lay du lieu cua richedit -> TCHAR*


void AddNewWord(); //add new word
void UpdateStatus(); //update status
void AutoSoft();  //Auto soft


vector<int> listflag; //list ds flag tuong ung vs moi tu 
vector<TCHAR*> list; //list data de hien thi listbox
vector<string> liststr;//list data de strcmp

vector<TCHAR*> listcb;//list data de hien thi trong combobox
vector<string> listcbstr;//list data trong combobox

string Convertostring(TCHAR tcArr[]); //Tchar* -> string
TCHAR *Convertotchar(string strA); // string -> Tchar *

unsigned int nCheckdialog = 0;
TCHAR* newword = new TCHAR[50]; //add tu moi

HWND hwndToolbar;
HWND hwndCombobox;
HWND hwndListbox;
HWND hwndStatus;
HWND hwndRichedit;
HWND hwndButtons;
string EV = "Eng_Viet";
string VE = "Viet_Eng";
int flag = 1; // lua chon che do EV - VE
int chon = 0; // da click vao 1 item -> sau se xoa
unsigned int flagtk = 0; // flag tim thay item

int flagadd = 0; //khi add xong 1 tu
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
	/*static HWND hwndToolbar;
	static HWND hwndCombobox;
	static HWND hwndListbox;
	static HWND hwndStatus;
	static HWND hwndRichedit;
	static HWND hwndButtons;*/

	/*string EV = "Eng_Viet";
	string VE = "Viet_Eng";*/
	//static int chon = 0; // da click vao 1 item -> sau se xoa
	//static int flag = 1; // lua chon che do EV - VE
	static int lbItem = 0;//chi so cua phan tu trong listbox
	static TCHAR* tchars = new TCHAR[50]; //Doc du lieu nhap vao tu combobox
	static int ntest = 0; //chi so item, khi tim kiem List box LB_FINDSTRING

	static TCHAR *chuoi = new TCHAR[50]; // data lay tu list box
	static unsigned int count = 0; // so luong item trong combobox
	//static unsigned int flagtk = 0; // flag tim thay item

	
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

		hwndButtons = CreateWindowEx(WS_EX_CLIENTEDGE, _T("button"), _T(">>"),
			WS_VISIBLE | WS_CHILD, 170, 30, 29, 29, hWnd, (HMENU)ID_SEARCH, GetModuleHandle(NULL), NULL);
		/*************Load data*********/
		ifstream Filein("Data/" + EV + "/ListData/ListData.txt", ios_base::in);
		Loaddata(hwndListbox, Filein);
		//update status
		UpdateStatus();

		//Dang ky hot key
		
		RegisterHotKey(hWnd, ID_MENU_UPDATE, MOD_CONTROL, 0x50);	//ctrl + P
		RegisterHotKey(hWnd, ID_MENU_ADD, MOD_CONTROL, 0x41);		//ctrl + A
		RegisterHotKey(hWnd, ID_MENU_DELETE, MOD_CONTROL, 0x44);    //ctrl + D
		RegisterHotKey(hWnd, ID_MENU_EXIT, MOD_CONTROL, 0x58);		//ctrl + X

		RegisterHotKey(hWnd, ID_DICTIONARY_ENG, MOD_CONTROL, 0x45);		//ctrl + E
		RegisterHotKey(hWnd, ID_DICTIONARY_VIET, MOD_CONTROL, 0x56);	//ctrl + V
		RegisterHotKey(hWnd, ID_FORMAT_FONT, MOD_CONTROL, 0x46);		//ctrl + F
		RegisterHotKey(hWnd, ID_HELP_ABOUT, MOD_CONTROL, 0x48);			//ctrl + H

		RegisterHotKey(hWnd, ID_FORMAT_B, MOD_CONTROL, 0x42);		//ctrl + B
		RegisterHotKey(hWnd, ID_FORMAT_U, MOD_CONTROL, 0x55);		//ctrl + U
		RegisterHotKey(hWnd, ID_FORMAT_I, MOD_CONTROL, 0x49);		//ctrl + I
		RegisterHotKey(hWnd, ID_FORMAT_C, MOD_CONTROL, 0x43);		//ctrl + C

		//hien thi dialog thong tin nguoi lap trinh
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About); 
	}
		break;
	case WM_SIZE:
	{
		Resize(hWnd);
	}
		break;
	case WM_HOTKEY: //xu ly cac hot key
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_MENU_UPDATE:	//ctrl + p: Update
		{

			if (chon == 1)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogUpdateYesNo), hWnd, DialogUpdateYesNo);

				if (nCheckdialog == 1)
				{
					string a;
					if (flag == 1)
					{
						a = "Data/Eng_Viet/Data/";
					}
					else
					{
						a = "Data/Viet_Eng/Data/";
					}

					string b;

					if (flagadd == 1)  b = Convertostring(newword);
					else b = Convertostring(chuoi);

					flagadd = 0;
					string c = ".txt";
					string s = a + b + c;
					TCHAR *buff = ReadRichEdit(hwndRichedit);
					CreateWriteFileUTF_8(s, buff);
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogUpdateSuccess), hWnd, DialogUpdateSuccess);
				}

			}
			else
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogErrorUpdate), hWnd, DialogErrorUpdate);
			}
		}
			break;
		case ID_MENU_ADD:		//ctrl + A: Add
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogAdd), hWnd, DialogAdd);
			UpdateStatus();
		}
			break;
		case ID_MENU_DELETE:	//ctrl + D: Delete
		{

			if (chon == 1 || flagtk == 1)
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogChooseDelete), hWnd, DialogChooseDelete);
				if (nCheckdialog == 1)
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDelete), hWnd, DialogDelete); //yes no
					if (nCheckdialog == 1)
					{
						if (flagadd == 0) SendMessage(hwndListbox, LB_GETTEXT, lbItem, (LPARAM)chuoi); //chua them 1 phan tu nao
						else chuoi = newword;
						flagadd = 0;
						for (unsigned int i = 0; i < liststr.size(); i++)
						{
							if (liststr[i] == Convertostring(chuoi) || liststr[i] == Convertostring(tchars)) listflag[i] = 0;
						}

						size_t sizelist = list.size();//size of list

						/******Delete data in RichEdit********************/
						SetWindowText(hwndRichedit, NULL);

						/******Delete data in combobox********************/
						SetWindowText(hwndCombobox, NULL);

						/******Delete data in List********************/
						SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
						/******************Load data******************************/
						for (unsigned int i = 0; i < sizelist; i++)
						{
							if (listflag[i] == 1)
								SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
						}
						chon = 0;
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteSuccess), hWnd, DialogDeleteSuccess);
					}
				}
				else if (nCheckdialog == 2) //xoa vinh vien 1 tu
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteYesNo), hWnd, DialogDeleteYesNo);
					if (nCheckdialog == 1)
					{
						//mo file Listdat va xoa phan tu
						string a;
						if (flag == 1)
						{
							a = EV;
						}
						else
						{
							a = VE;
						}

						string buff = "Data/" + a + "/ListData/ListData.txt";

						ofstream Fileout(buff, ios_base::out);
						//xoa phan tu = cach in ra danh sach moi
						unsigned int nchecktest = -1;
						if (flagadd == 0) SendMessage(hwndListbox, LB_GETTEXT, lbItem, (LPARAM)chuoi); //chua them 1 phan tu nao
						else chuoi = newword;
						flagadd = 0;

						//tim vi tri ma phan tu can xoa
						size_t size = liststr.size();
						for (unsigned int i = 0; i < size; i++)
						{
							if (liststr[i] == Convertostring(chuoi) || liststr[i] == Convertostring(tchars)) nchecktest = i;
						}

						//xoa phan tu khoi cac vector
						list.erase(list.begin() + nchecktest);
						liststr.erase(liststr.begin() + nchecktest);
						listflag.erase(listflag.begin() + nchecktest);

						//kich thuoc moi sau khi xoa 1 phan tu
						size = liststr.size();
						//in lai ra file moi
						for (unsigned int i = 0; i < size; i++)
						{
							if (i < size - 1) Fileout << liststr[i] << "\n";
							else Fileout << liststr[i];
						}
						Fileout.close(); //close fileout

						/********************* Hien thi lai thong tin *******************/
						/******Delete data in RichEdit********************/
						SetWindowText(hwndRichedit, NULL);

						/******Delete data in combobox********************/
						SetWindowText(hwndCombobox, NULL);

						/******Delete data in List********************/
						SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
						/******************Load data******************************/
						for (unsigned int i = 0; i < size; i++)
						{
							if (listflag[i] == 1)
								SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
						}
						chon = 0;
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteSuccess), hWnd, DialogDeleteSuccess);
					}
				}
			}
			else
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogErrorDelete), hWnd, DialogErrorDelete);
			}
			UpdateStatus();
		}
			break;
		case ID_MENU_EXIT:		//ctrl + X: Exit
		{
			DestroyWindow(hWnd);
		}
			break;
		case ID_DICTIONARY_ENG:	//ctrl + E: Eng - Viet
		{
			chon = 0; //set k item nao dc chon
			flag = 1;

			SetWindowText(hwndRichedit, NULL);
			SetWindowText(hwndCombobox, NULL);
			//xoa data trong danh sach listbox
			list.clear();
			liststr.clear();
			listflag.clear();
			//xoa data trong danh sach luu combobox

			/**********************xoa item trong combobox*************************/
			SendMessage(hwndCombobox, CB_RESETCONTENT, 0, 0);
			/*********************Xoa item trong listbox**************************/
			SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
			/***************Load data************/
			ifstream Filein("Data/" + EV + "/ListData/ListData.txt", ios_base::in);
			Loaddata(hwndListbox, Filein);
			/*********************Set Status*********************/
			UpdateStatus();
			//SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)_T("Eng - Viet Dictionary"));
		}
			break;
		case ID_DICTIONARY_VIET://ctrl + V: Viet - Eng
		{
			chon = 0; //set k item nao dc chon
			flag = 0;
			SetWindowText(hwndRichedit, NULL);
			SetWindowText(hwndCombobox, NULL);
			//xoa data trong danh sach listbox
			list.clear();
			liststr.clear();
			listflag.clear();
			//xoa data trong danh sach luu combobox

			/**********************xoa item trong combobox*************************/
			SendMessage(hwndCombobox, CB_RESETCONTENT, 0, 0);
			/*********************Xoa item trong listbox**************************/
			SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
			/***************Load data************/
			ifstream Filein("Data/" + VE + "/ListData/ListData.txt", ios_base::in);

			Loaddata(hwndListbox, Filein);
			/*********************Set Status*********************/
			UpdateStatus();
			//SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)_T("Viet - Eng Dictionary"));
		}
			break;
		case ID_FORMAT_FONT:	//ctrl + F: Font
		{
			MessageBox(hWnd, _T("Font"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
		}
			break;
		case ID_FORMAT_B:		//ctrl + B: Bold
		{
			MessageBox(hWnd, _T("Bold"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
		}
			break;
		case ID_FORMAT_U:		//ctrl + U: Underline
		{
			MessageBox(hWnd, _T("Underline"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
		}
			break;
		case ID_FORMAT_I:		//ctrl + I: Italic
		{
			MessageBox(hWnd, _T("Italic"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
		}
			break;	
		case ID_FORMAT_C:		//ctrl + C: Color
		{
			MessageBox(hWnd, _T("Color"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
		}
			break;
		case ID_HELP_ABOUT:		//ctrl + H: help -> About
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		}
			break;
		}
	}
	break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
			case IDC_MAIN_COMBOBOX:
			{
				GetWindowText(hwndCombobox, tchars, 50);
				if (flagtk == 1) //da tim kiem dc 1 item
				{
					/*
						hien thi lai danh sach khi click lai combobox
						hien thi lai danh sach khi click lai combobox
						khong xoa lich su cua combobox
						hien thi lia = cach load lai file listdata
					*/
					SetWindowText(hwndRichedit, NULL);
					//xoa data trong danh sach listbox
					list.clear();
					liststr.clear();
				
					//xoa data trong danh sach luu combobox
					
					/*********************Xoa item trong listbox**************************/
					SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
					/***************Load data************/
					string a;
					if (flag == 1) a = EV;
					else a = VE;
					ifstream Filein("Data/" + a + "/ListData/ListData.txt", ios_base::in);
					Loaddata2(hwndListbox, Filein); //load lai data, nhung giu nguyen listflag, trang thai co da xoa
					Filein.close();
				}
				flagtk = 0;
			}
				break;
			
			case IDC_MAIN_LISTBOX:
			{
				switch (HIWORD(wParam))
				{
				case LBN_DBLCLK:
				{
					
					// lay vi tri item trong listbox
					lbItem = (int)SendMessage(hwndListbox, LB_GETCURSEL, 0, 0);
					// Get  data.
					//TCHAR *chuoi = new TCHAR[50]; // data lay tu list box
					SendMessage(hwndListbox, LB_GETTEXT, lbItem, (LPARAM)chuoi);
					//hien thi data item tu listbox len combobox
					SetWindowText(hwndCombobox, chuoi);

					//hien thi len richedit
					string a;
					if (flag == 1)
					{
						a = "Data/Eng_Viet/Data/";
					}
					else
					{
						a = "Data/Viet_Eng/Data/";
					}
					string b = Convertostring(chuoi);
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
				chon = 0; //set k item nao dc chon
				flag = 1;
				
				SetWindowText(hwndRichedit, NULL);
				SetWindowText(hwndCombobox, NULL);
				//xoa data trong danh sach listbox
				list.clear();
				liststr.clear();
				listflag.clear();
				//xoa data trong danh sach luu combobox

				/**********************xoa item trong combobox*************************/
				SendMessage(hwndCombobox, CB_RESETCONTENT, 0, 0);
				/*********************Xoa item trong listbox**************************/
				SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
				/***************Load data************/
				ifstream Filein("Data/" + EV + "/ListData/ListData.txt", ios_base::in);
				Loaddata(hwndListbox, Filein);
				/*********************Set Status*********************/
				UpdateStatus();
				//SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)_T("Eng - Viet Dictionary"));
			}
			break;
			case ID_DICTIONARY_VIET:
			{
				chon = 0; //set k item nao dc chon
				flag = 0;
				SetWindowText(hwndRichedit, NULL);
				SetWindowText(hwndCombobox, NULL);
				//xoa data trong danh sach listbox
				list.clear();
				liststr.clear();
				listflag.clear();
				//xoa data trong danh sach luu combobox

				/**********************xoa item trong combobox*************************/
				SendMessage(hwndCombobox, CB_RESETCONTENT, 0, 0);
				/*********************Xoa item trong listbox**************************/
				SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
				/***************Load data************/
				ifstream Filein("Data/" + VE + "/ListData/ListData.txt", ios_base::in);
				
				Loaddata(hwndListbox, Filein);
				/*********************Set Status*********************/
				UpdateStatus();
				//SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)_T("Viet - Eng Dictionary"));
			}
				break;
			case ID_MENU_UPDATE:
			{
				if (chon == 1)
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogUpdateYesNo), hWnd, DialogUpdateYesNo);

					if (nCheckdialog == 1)
					{
						string a;
						if (flag == 1)
						{
							a = "Data/Eng_Viet/Data/";
						}
						else
						{
							a = "Data/Viet_Eng/Data/";
						}

						string b;

						if (flagadd == 1)  b = Convertostring(newword);
						else b = Convertostring(chuoi);

						flagadd = 0;
						string c = ".txt";
						string s = a + b + c;
						TCHAR *buff = ReadRichEdit(hwndRichedit);
						CreateWriteFileUTF_8(s, buff);
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogUpdateSuccess), hWnd, DialogUpdateSuccess);
						
					}
					
				}
				else
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogErrorUpdate), hWnd, DialogErrorUpdate);
				}
			}
				break;
			case ID_MENU_ADD:
			{
				DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogAdd), hWnd, DialogAdd);
				UpdateStatus();
			}
				break;
			case ID_SEARCH:
			{
				ntest = SendMessage(hwndListbox, LB_FINDSTRINGEXACT, 1, (LPARAM)tchars);//lay vi tri khi tim thay
				unsigned int checktt = 0; //kiem tra chuoi tim kiem co trung vs data cu k
				if (ntest >= 0)
				{
					SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
					SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)tchars);
					SendMessage(hwndListbox, LB_SELECTSTRING, 1, (LPARAM)tchars);
					flagtk = 1;
					lbItem = ntest;
					/************************* Combobox **********************/
					count = SendMessage(hwndCombobox, CB_GETCOUNT, 0, 0); // so item trong combobox cu
					for (unsigned int i = 0; i < count; i++)
					{
						if (listcbstr[i] == liststr[ntest])
						{
							checktt++;
						}
					}
					if (checktt == 0)
					{
						listcbstr.push_back(liststr[ntest]);
						listcb.push_back(list[ntest]);
						SendMessage(hwndCombobox, CB_ADDSTRING, 0, (LPARAM)list[ntest]);
					}
					
					/************************* Richedit **********************/
					string a;
					if (flag == 1)
					{
						a = "Data/Eng_Viet/Data/";
					}
					else
					{
						a = "Data/Viet_Eng/Data/";
					}
					//string b = liststr[lbItem];
					string b = Convertostring(tchars);
					string c = ".txt";
					string buff = a + b + c;

					ifstream Filein(buff, ios_base::in);
					ShowMeanEV(hwndRichedit, hWnd, Filein);
					Filein.close();
					chon = 1;//co the xoa
				}
				else
				{
					flagtk = 0;
					SetWindowText(hwndRichedit, NULL);
					
					//SetWindowText(hwndCombobox, NULL);
					MessageBox(NULL, _T("No word is found"), _T("Infor"), MB_ICONINFORMATION | MB_OK);
				}
				
			}
				break;
			case ID_MENU_DELETE:
			{
				
				if (chon == 1 || flagtk == 1)
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogChooseDelete), hWnd, DialogChooseDelete);
					if (nCheckdialog == 1)
					{
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDelete), hWnd, DialogDelete); //yes no
						if (nCheckdialog == 1)
						{
							if (flagadd == 0) SendMessage(hwndListbox, LB_GETTEXT, lbItem, (LPARAM)chuoi); //chua them 1 phan tu nao
							else chuoi = newword;
							flagadd = 0;
							for (unsigned int i = 0; i < liststr.size(); i++)
							{
								if (liststr[i] == Convertostring(chuoi) || liststr[i] == Convertostring(tchars)) listflag[i] = 0;
							}

							size_t sizelist = list.size();//size of list

							/******Delete data in RichEdit********************/
							SetWindowText(hwndRichedit, NULL);

							/******Delete data in combobox********************/
							SetWindowText(hwndCombobox, NULL);

							/******Delete data in List********************/
							SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
							/******************Load data******************************/
							for (unsigned int i = 0; i < sizelist; i++)
							{
								if (listflag[i] == 1)
									SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
							}
							chon = 0;
							DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteSuccess), hWnd, DialogDeleteSuccess);
						}
					}
					else if (nCheckdialog == 2) //xoa vinh vien 1 tu
					{
						DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteYesNo), hWnd, DialogDeleteYesNo);
						if (nCheckdialog == 1)
						{
							//mo file Listdat va xoa phan tu
							string a;
							if (flag == 1)
							{
								a = EV;
							}
							else
							{
								a = VE;
							}
							
							string buff = "Data/" + a + "/ListData/ListData.txt";

							ofstream Fileout(buff, ios_base::out);
							//xoa phan tu = cach in ra danh sach moi
							unsigned int nchecktest = -1;
							if (flagadd == 0) SendMessage(hwndListbox, LB_GETTEXT, lbItem, (LPARAM)chuoi); //chua them 1 phan tu nao
							else chuoi = newword;
							flagadd = 0;

							//tim vi tri ma phan tu can xoa
							size_t size = liststr.size();
							for (unsigned int i = 0; i < size; i++)
							{
								if (liststr[i] == Convertostring(chuoi) || liststr[i] == Convertostring(tchars)) nchecktest = i;
							}

							//xoa phan tu khoi cac vector
							list.erase(list.begin() + nchecktest);
							liststr.erase(liststr.begin() + nchecktest);
							listflag.erase(listflag.begin() + nchecktest);

							//kich thuoc moi sau khi xoa 1 phan tu
							size = liststr.size();
							//in lai ra file moi
							for (unsigned int i = 0; i < size; i++)
							{
								if (i < size - 1) Fileout << liststr[i] << "\n";
								else Fileout << liststr[i];
							}
							Fileout.close(); //close fileout
							
							/********************* Hien thi lai thong tin *******************/
							/******Delete data in RichEdit********************/
							SetWindowText(hwndRichedit, NULL);

							/******Delete data in combobox********************/
							SetWindowText(hwndCombobox, NULL);

							/******Delete data in List********************/
							SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
							/******************Load data******************************/
							for (unsigned int i = 0; i < size; i++)
							{
								if (listflag[i] == 1)
									SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
							}
							chon = 0;
							DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogDeleteSuccess), hWnd, DialogDeleteSuccess);
						}
					}
				}
				else
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogErrorDelete), hWnd, DialogErrorDelete);
				}
				UpdateStatus();
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

// Message handler for Dialog about.
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
// Message handler for Dialog delete
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
INT_PTR CALLBACK DialogChooseDelete(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == ID_Destroy)
		{
			if (LOWORD(wParam) == IDOK) nCheckdialog = 1;
			else if (LOWORD(wParam) == IDCANCEL) nCheckdialog = 0;
			else nCheckdialog = 2;
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
INT_PTR CALLBACK DialogDeleteYesNo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (LOWORD(wParam) == IDOK) nCheckdialog = 1;
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

//Message handler for dialog update
INT_PTR CALLBACK DialogErrorUpdate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
INT_PTR CALLBACK DialogUpdateYesNo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (LOWORD(wParam) == IDOK) nCheckdialog = 1;
			else nCheckdialog = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK DialogUpdateSuccess(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
//Message handler for dialog add
INT_PTR CALLBACK DialogErrorAdd(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
INT_PTR CALLBACK DialogAdd(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	{
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			switch (LOWORD(wParam))
			{
			case IDOK:
			{
				 GetDlgItemText(hDlg, IDC_EditWord, newword, 50);
				 string newwordstr = Convertostring(newword);
				 if (newwordstr == "") //tu vua nhap rong
				 {
					 MessageBox(NULL, _T("No charactor, Please try again!!!"), _T("Error"), MB_ICONWARNING | MB_OK);
				 }
				 else // tu vua nhap khong rong
				 {
					 size_t size = list.size();
					 unsigned int check = 0;
					 unsigned int num = -1;
					 for (unsigned int i = 0; i < size; i++)
					 {
						 if (liststr[i] == newwordstr) //tu da ton tai
						 {
							 check = 0;
							 num = i;
							 break;
						 }
						 else //tu moi
						 {
							 check++;
						 }
					 }
					 if (check == 0)
					 {
						 if (listflag[num] == 1) //trung tu hien co trong list, va chua bi xoa, listflag = 1
							 DialogBox(hInst, MAKEINTRESOURCE(IDD_DialogAddNewUpdate), hDlg, DialogAddNewUpdate);
						 else //trung tu vua xoa tam
						 {
							 MessageBox(NULL, _T("This word has just been removed, we have recovered"), _T("Infor"), MB_ICONWARNING | MB_OK);
							 listflag[num] = 1;

							 //Hien thi lai listbox
							 size_t sizelist = list.size();//size of list

								/******Delete data in RichEdit********************/
							 SetWindowText(hwndRichedit, NULL);

							 /******Delete data in combobox********************/
							 SetWindowText(hwndCombobox, NULL);

							 /******Delete data in List********************/
							 SendMessage(hwndListbox, LB_RESETCONTENT, 0, 0);
							 /******************Load data******************************/
							 for (unsigned int i = 0; i < sizelist; i++)
							 {
								 if (listflag[i] == 1)
									 SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
							 }


							 //hien thi len richedit
							 string a;
							 if (flag == 1)
							 {
								 a = "Data/Eng_Viet/Data/";
							 }
							 else
							 {
								 a = "Data/Viet_Eng/Data/";
							 }
							 string b = Convertostring(newword);
							 string c = ".txt";
							 string buff = a + b + c;

							 ifstream Filein(buff, ios_base::in);
							 ShowMeanEV(hwndRichedit, hDlg, Filein);
							 Filein.close();
							 //lua chon item
							 SendMessage(hwndListbox, LB_SELECTSTRING, 1, (LPARAM)newword);

							 //hien thi combobox
							 SetWindowText(hwndCombobox, newword);
							 chon = 1;
							 flagadd = 1;
						 }
					 }
					 else //khong trung tu da co
					 {
						 AddNewWord();
					 }
				 }
				
				
			}
			break;
			case IDCANCEL:
			{
				
			}
			break;
			}
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

	}
		
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK DialogAddYesNo(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
			if (LOWORD(wParam) == IDOK) nCheckdialog = 1;
			else nCheckdialog = 0;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK DialogAddNewUpdate(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
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
	{
		switch (LOWORD(wParam))
		{
		case IDYES:
		{
			
			//hien thi len richedit
			string a;
			if (flag == 1)
			{
				a = "Data/Eng_Viet/Data/";
			}
			else
			{
				a = "Data/Viet_Eng/Data/";
			}
			string b = Convertostring(newword);
			string c = ".txt";
			string buff = a + b + c;

			ifstream Filein(buff, ios_base::in);
			ShowMeanEV(hwndRichedit, hDlg, Filein);
			Filein.close();
			//lua chon item
			SendMessage(hwndListbox, LB_SELECTSTRING, 1, (LPARAM)newword);

			//hien thi combobox
			SetWindowText(hwndCombobox, newword);
			chon = 1;
			flagadd = 1;
		}
			break;
		case IDCANCEL:
		{
			
		}
			break;
		}
		EndDialog(hDlg, LOWORD(wParam));
		return (INT_PTR)TRUE;
		break;
	}
	}
	return (INT_PTR)FALSE;
}
/**************************** other function ****************************************/
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
	

	SetWindowPos(hListBox, NULL, 0, 55, 200, nEditHeight - 19, SWP_NOZORDER);
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
		listflag.push_back(1);
	}
	Filein.close();
	
	size_t sizeliststr = liststr.size();
	for (unsigned int i = 0; i < sizeliststr; i++)
	{
		TCHAR *buff = Convertotchar(liststr[i]);
		list.push_back(buff);
	}
	//Sap xep theo bang chu cai
	AutoSoft();

	//Hien thi item len listbox
	size_t sizeList = list.size();
	for (unsigned int i = 0; i < sizeList; i++)
	{
		if (listflag[i] == 1)
		SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)list[i]);
	}
}
void Loaddata2(HWND hwndListbox, ifstream &Filein)
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
		//listflag.push_back(1);
	}
	Filein.close();

	
	//Hien thi item len listbox
	size_t sizeliststr = liststr.size();
	for (unsigned int i = 0; i < sizeliststr; i++)
	{
		TCHAR *buff = Convertotchar(liststr[i]);
		list.push_back(buff);
	}
	//Sap xep theo bang chu cai
	AutoSoft();
	size_t sizeList = list.size();
	for (unsigned int i = 0; i < sizeList; i++)
	{
		if (listflag[i] == 1)
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
void CreateWriteFileUTF_8(string s, TCHAR *swork)
{
	TCHAR *sfile = Convertotchar(s);
	DWORD dwBytesWritten;

	SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HANDLE hFile = CreateFile(sfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	unsigned char smarker[3];
	smarker[0] = 0xEF;
	smarker[1] = 0xBB;
	smarker[2] = 0xBF;
	
	WriteFile(hFile, smarker, 3, &dwBytesWritten, NULL);

	size_t llen = _tcslen(swork);
	unsigned int lengt = (llen + 1) * sizeof(TCHAR);
	char *utf8 = (char *)malloc(lengt); //(llen + 1) * sizeof(TCHAR));

	int lBytesWritten = WideCharToMultiByte(CP_UTF8, 0, swork, llen, utf8, lengt, NULL, NULL);

	// lBytesWritten includes the null

	int err = GetLastError();

	WriteFile(hFile, utf8, lBytesWritten, &dwBytesWritten, NULL);

	free(utf8);

	SetEndOfFile(hFile);

	CloseHandle(hFile);
}
TCHAR *ReadRichEdit(HWND hwndRichedit)
{
	//lay kich thuoc data cua richedit
	_gettextlengthex vd1;
	vd1.codepage = GTL_DEFAULT;
	vd1.flags = 1200; //Unicode
	//vd1.flags = CP_ACP; ANSI
	_gettextlengthex *vd2 = &vd1;

	unsigned int b = SendMessage(hwndRichedit, EM_GETTEXTLENGTHEX, (WPARAM)vd2, 0);
	b = b + 2;
	//lay data tu richedit ->>buff
	_gettextex vd;
	vd.cb = b; //kich thuoc data
	vd.flags = GT_DEFAULT;
	vd.codepage = 1200; //Unicode

	//vd.codepage = CP_ACP; //ANSI
	vd.lpDefaultChar = 0;
	vd.lpUsedDefChar = 0;

	_gettextex *point = &vd;

	TCHAR *buff = new TCHAR[b];

	SendMessage(hwndRichedit, EM_GETTEXTEX, (WPARAM)point, (LPARAM)buff);
	return buff;
}
void AddNewWord()
{
	string newwordstr = Convertostring(newword);
	string a;
	if (flag == 1)
	{
		a = EV;
	}
	else
	{
		a = VE;
	}

	//xoa rich edit
	SetWindowText(hwndRichedit, NULL);
	//tao moi 1 file
	ofstream Fileout1("Data/" + a + "/Data/" + newwordstr + ".txt", ios_base::out);
	Fileout1.close();

	//them tu vua nhap vao list
	list.push_back(newword);
	liststr.push_back(newwordstr);
	listflag.push_back(1);
	MessageBox(NULL, _T("Add mean into Richedit, then click Update to Success"), _T("infor"), MB_ICONINFORMATION | MB_OK);
	
	//add new word into file and display listbox
	ofstream Fileout("Data/" + a + "/ListData/ListData.txt", ios_base::app);
	Fileout << "\n" + newwordstr;
	Fileout.close();

	

	
	//them vao listbox
	SendMessage(hwndListbox, LB_ADDSTRING, 0, (LPARAM)newword);
	//them vao combobox
	SetWindowText(hwndCombobox, newword);

	//chon item vua roi
	SendMessage(hwndListbox, LB_SELECTSTRING, 1, (LPARAM)newword);
	chon = 1;
	flagadd = 1;
	//flagtk = 1;
}
void UpdateStatus()
{
	unsigned int count = -1;
	string a;
	if (flag == 1) a = EV;
	else a = VE;
	count = SendMessage(hwndListbox, LB_GETCOUNT, 0, 0);
	string b = to_string(count);
	string buff = a + " Dictionary (" + b + ")";
	TCHAR *tchar = Convertotchar(buff);
	SendMessage(hwndStatus, SB_SETTEXT, 0, (LPARAM)tchar);
}
void AutoSoft()
{
	size_t size = liststr.size();
	string buff;
	TCHAR* Buff;
	int flag;
	for (int i = 0; i < size - 1; i++)
	{
		for (int j = i + 1; j < size; j++)
		{
			if (liststr[i] > liststr[j])
			{
				//sap xep chuoi string
				buff = liststr[i];
				liststr[i] = liststr[j];
				liststr[j] = buff;

				//sap xep chuoi TCHAR*
				Buff = list[i];
				list[i] = list[j];
				list[j] = Buff;
				//sap xep flag tu
				flag = listflag[i];
				listflag[i] = listflag[j];
				listflag[j] = flag;
			}
		}
	}
}
