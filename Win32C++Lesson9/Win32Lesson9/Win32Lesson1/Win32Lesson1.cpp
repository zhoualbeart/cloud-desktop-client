// Win32Lesson1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Win32Lesson1.h"
#include "qrcode.h"
#include "const.h"
#include "json_data_parse.h"
#include <io.h>
#include <stdio.h>
#include <curl.h>
#include <stat.h>
#include <timeb.h>


#define DEFAULT_APPCC_HOST   "https://appcc.cloudak47.com"
#define CONF_FILE            "config.json"
#define BMP_FILE			 "qr.bmp"

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

static char *read_json_data_from_file(char const *filename);
static void strrpl(char* pDstOut, char* pSrcIn, const char* pSrcRpl, const char* pDstRpl);

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	if (NULL == lpCmdLine || 0 == strlen(lpCmdLine)) // check the arg is null or not
		return FALSE;

	if (0 != _access(lpCmdLine, 0))   // check is the file exist
		return FALSE;


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32LESSON1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	char str_final[MAX_PATH] = {0};
	strrpl(str_final, lpCmdLine, "\\", "\\\\");

	 char TempFilePath[MAX_PATH];
     if(!GetTempPath(sizeof(TempFilePath),TempFilePath)) {
		 return FALSE;
     }


	/* generate qrcode bmp file */
	/*if (0 != genernate_qrcode(lpCmdLine, "b.bmp"))
	{
		printf("error to gernate qrcode file\n");
		return FALSE;
	}*/

	/*
	 * https://appcc.pispower.com/s?T=12&t=1497008691868&d=urlencode(JSON) 
	 * http[s]://appcc.cloudak47.cn/s?T=12&t=1497008691868&d=
	 * {“username”:”xxx”, “domain”:”od.com”, “filepath”:”C:\demo.txt”}
	 */

	char qrcode_content[10240] = {0};
	char temp2[1024] = {0};
	char temp3[1024] = {0};
	char *temp4;

	/* 1.get url from config */
	char appcc_host[MAX_PATH] = {0};
	char *json_data = NULL;
	char config_file[MAX_PATH] = {0};
	/* get exec dir */

	char   sPath[MAX_PATH] = {0};   
	GetModuleFileName(NULL,sPath,MAX_PATH);
	*strrchr(sPath, 0x5C) = 0;
	sprintf(config_file, "%s\\%s", sPath, CONF_FILE);

	char qr_bmp_file[MAX_PATH] = {0};
	sprintf(qr_bmp_file, "%s%s", TempFilePath, BMP_FILE);

	json_data = read_json_data_from_file(config_file);

	if (JSON_DATA_INVALIDE == get_appcc_host(json_data, appcc_host))
	{
		strcpy(appcc_host, DEFAULT_APPCC_HOST);
	}

	/* 2.get time from appcc */
	char strtime[MAX_PATH] = {0};
	struct timeb rawtime;
	memset(&rawtime, 0, sizeof(rawtime));
	ftime(&rawtime);
	//sprintf(strtime, "%hu", rawtime.millitm);
	sprintf(strtime, "%ld000", rawtime.time);


	/* 3.get username */
	DWORD ret = MAX_PATH;
	char username[MAX_PATH]={0};
	if (!GetUserName(username, &ret))
	{
		fprintf(stderr, "Error: error number:%sd", GetLastError());
	}
	/* 4.get domain */
	/* cmd: wmic computersystem get domain */
	
	char *temp = "/s?T=12&t=";
	sprintf(temp2, "{\"username\":\"%s\", \"domain\":\"\", \"filepath\":\"%s\"}",username, str_final);
	temp4 = curl_escape(temp2, strlen(temp2));
	sprintf(temp3, "%s%s%s&d=%s", appcc_host, temp, strtime, temp4);

	if (0 != genernate_qrcode(temp3, qr_bmp_file))
	{
		printf("error to gernate qrcode file\n");
		return FALSE;
	}

	/* cleanup */

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32LESSON1));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
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
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	//wcex.style			= WS_CAPTION;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	//wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32LESSON1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON1));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   //hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
   //   CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   //dwStyle |= WS_CLIPSIBLINGS;
	//dwStyle &= ~(WS_CAPTION | WS_BORDER);
   //WS_OVERLAPPEDWINDOW & WS_SYSMENU
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & WS_SYSMENU ,
      CW_USEDEFAULT, CW_USEDEFAULT, 280, 280, NULL, NULL, hInstance, NULL);

   CreateWindow(   //按钮创建    
        "static",    
        "请用小慧扫码保存文件\r\n也可以使用快捷键Alt+s直接发送给小慧",    
        WS_CHILD | WS_VISIBLE | SS_CENTER,    
        30,290,280,35,    
        hWnd,    
        NULL,    
        hInstance,    
        0);    

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

bool LoadAndBlitBitmap(LPCWSTR szFileName, HDC hWinDC)
{
	// Load the bitmap image file
	HBITMAP hBitmap;
	hBitmap = (HBITMAP)::LoadImage(NULL, (LPCSTR)szFileName, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE);
	// Verify that the image was loaded
	if (hBitmap == NULL) {
		::MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
		return false;
	}

	// Create a device context that is compatible with the window
	HDC hLocalDC;
	hLocalDC = ::CreateCompatibleDC(hWinDC);
	// Verify that the device context was created
	if (hLocalDC == NULL) {
		::MessageBox(NULL, __T("CreateCompatibleDC Failed"), __T("Error"), MB_OK);
		return false;
	}

	// Get the bitmap's parameters and verify the get
	BITMAP qBitmap;
	int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),
		reinterpret_cast<LPVOID>(&qBitmap));
	if (!iReturn) {
		::MessageBox(NULL, __T("GetObject Failed"), __T("Error"), MB_OK);
		return false;
	}

	// Select the loaded bitmap into the device context
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hLocalDC, hBitmap);
	if (hOldBmp == NULL) {
		::MessageBox(NULL, __T("SelectObject Failed"), __T("Error"), MB_OK);
		return false;
	}

	// Blit the dc which holds the bitmap onto the window's dc
	BOOL qRetBlit = ::BitBlt(hWinDC, 50, 40, qBitmap.bmWidth, qBitmap.bmHeight,
		hLocalDC, 0, 0, SRCCOPY);
	if (!qRetBlit) {
		::MessageBox(NULL, __T("Blit Failed"), __T("Error"), MB_OK);
		return false;
	}

	// Unitialize and deallocate resources
	::SelectObject(hLocalDC, hOldBmp);
	::DeleteDC(hLocalDC);
	::DeleteObject(hBitmap);
	return true;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	char qr_bmp_file[MAX_PATH] = {0};
	char TempFilePath[MAX_PATH];
	if(!GetTempPath(sizeof(TempFilePath),TempFilePath)) {
		return 1;
	}
	sprintf(qr_bmp_file, "%s%s", TempFilePath, BMP_FILE);

	switch (message)
	{
	case WM_CREATE:
		int scrWidth,scrHeight;
		RECT rect;
		scrWidth=GetSystemMetrics(SM_CXSCREEN);
		scrHeight=GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(hWnd,&rect);
		rect.left=(scrWidth-rect.right)/2;
		rect.top=(scrHeight-rect.bottom)/2;
		SetWindowPos(hWnd,HWND_TOP,rect.left,rect.top,350,390,SWP_SHOWWINDOW);
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		//LoadAndBlitBitmap(LPCWSTR("C:\\tmp\\b.bmp"), hdc);
		LoadAndBlitBitmap(LPCWSTR(qr_bmp_file), hdc);
		EndPaint(hWnd, &ps);
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


static char *read_json_data_from_file(char const *filename)
{
    FILE *fp;
    struct stat filestatus;
    int file_size;
    char* file_contents;
    char *json_data;
    
    if (NULL == filename)
    {
        return NULL;
    }

    if ( stat(filename, &filestatus) != 0) {
            return NULL;
    }
    file_size = filestatus.st_size;
    file_contents = (char*)malloc(filestatus.st_size + 1);
    if ( file_contents == NULL) {
            return NULL;
    }

    fp = fopen(filename, "rt");
    if (fp == NULL) {
            fclose(fp);
            free(file_contents);
            return NULL;
    }
    if ( fread(file_contents, file_size, 1, fp) != 1 ) {
            fclose(fp);
            free(file_contents);
            return NULL;
    }
    fclose(fp);
    
    file_contents[file_size] = '\0';

    json_data = (char *)malloc(strlen(file_contents) + 1);
    if (NULL == json_data)
    {
		return NULL;
    }
    
    strcpy(json_data, file_contents);
    
    free(file_contents);
    return json_data;

}

//************************************
// Method: strrpl
// FullName: strrpl
// Access: public 
// Returns: void
// Qualifier: 字符串替换函数,能替换所有的要替换的字符串,被替换的字符串和替换的字符串不一定一样长.
// Parameter: char * pDstOut,输出字符串,要保证足够的空间可以存储替换后的字符串.
// Parameter: char * pSrcIn,输入字符串.
// Parameter: char * pSrcRpl,被替换的字符串.
// Parameter: char * pDstRpl,替换后的字符串.
// 注意:以上的字符串均要以 '\0 '结尾.
//************************************
static void strrpl(char* pDstOut, char* pSrcIn, const char* pSrcRpl, const char* pDstRpl)
{ 
	char* pi = pSrcIn; 
	char* po = pDstOut; 

	int nSrcRplLen = strlen(pSrcRpl); 
	int nDstRplLen = strlen(pDstRpl); 

	char *p = NULL; 
	int nLen = 0; 

	do 
	{
		// 找到下一个替换点
		p = strstr(pi, pSrcRpl); 

		if(p != NULL) 
		{ 
			// 拷贝上一个替换点和下一个替换点中间的字符串
			nLen = p - pi; 
			memcpy(po, pi, nLen);

			// 拷贝需要替换的字符串
			memcpy(po + nLen, pDstRpl, nDstRplLen); 
		} 
		else 
		{ 
			strcpy(po, pi); 

			// 如果没有需要拷贝的字符串,说明循环应该结束
			break;
		} 

		pi = p + nSrcRplLen; 
		po = po + nLen + nDstRplLen; 

	} while (p != NULL); 
}



