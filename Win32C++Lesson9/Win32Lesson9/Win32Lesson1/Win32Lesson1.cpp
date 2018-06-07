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
#include <Windows.h>
#include <Ntsecapi.h>
#include <assert.h>
#include <lm.h>
#include <wincodec.h>
#include <windowsx.h>   
#include <windef.h>
#include <atlimage.h>  

#pragma comment(lib, "netapi32.lib")


#define    DOMAIN_SUFFIX		          ".com"
#define	   DEFAULT_APPCC_HOST             "https://appcc.cloudak47.com"
#define    CONF_FILE                      "config.json"
#define    BMP_FILE			              "qr.bmp"

/* 请用小慧扫码保存文件\r\n也可以使用快捷键Alt+s直接发送给小慧的ASCII */
#define PROMPT_MESSAGE		 \
	"\u8bf7\u7528\u5c0f\u6167\u626b\u7801\u4fdd\u5b58\u6587\u4ef6\r\n"		\
	"\u4e5f\u53ef\u4ee5\u4f7f\u7528\u5feb\u6377"							\
	"\u952eAlt+s\u76f4\u63a5\u53d1\u9001\u7ed9\u5c0f\u6167"

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
void Convert(const char* strIn, char* strOut, int sourceCodepage, int targetCodepage);
int  get_domain_name(char *domain_name);
IStream * CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType);
HBITMAP LoadSplashImage(LPCTSTR lpName, LPCTSTR lpType);
void SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash);
bool LoadPngImage(LPCTSTR lpName, LPCTSTR lpType, HWND hWnd);
HBRUSH m_brush;


ULONG_PTR gdiplusToken = 0; 
char qr_bmp_file[MAX_PATH] = {0};
int moveX;
int moveY;
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;  
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);  
	MSG msg;
	HACCEL hAccelTable;

	if (NULL == lpCmdLine || 0 == strlen(lpCmdLine)) // check the arg is null or not
	{
		::MessageBox(NULL, __T("filepath is null or empty!"), __T("Error"), MB_OK);
		return FALSE;
	}

	if (0 != _access(lpCmdLine, 0))   // check is the file exist
	{
		::MessageBox(NULL, __T("file no exsist!"), __T("Error"), MB_OK);
		return FALSE;
	}

	size_t len =  strlen(lpCmdLine);

	if (MAX_PATH < strlen(lpCmdLine))   // check file length
	{
		::MessageBox(NULL, 
					 __T("the filename for the path is too long, "
					 "you shuold shorten the filepath"), 
					 __T("Error"), MB_OK);
		return FALSE;
	}


	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32LESSON1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);


	char str_final_ansi[MAX_PATH] = {0};
	char str_final_utf8[MAX_PATH] = {0};
	strrpl(str_final_ansi, lpCmdLine, "\\", "\\\\");
	Convert(str_final_ansi, str_final_utf8, CP_ACP, CP_UTF8);

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
	char temp3[2048] = {0};
	char qr_data_utf8[2048] = {0};
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
	char domain_name[MAX_PATH] = {0};
	get_domain_name(domain_name);
	
	char *temp = "/s?T=12&t=";
	sprintf(temp2, "{\"username\":\"%s\", \"domain\":\"%s\", \"filepath\":\"%s\"}",
		    username, domain_name, str_final_utf8);
	temp4 = curl_escape(temp2, strlen(temp2));
	sprintf(temp3, "%s%s%s&d=%s", appcc_host, temp, strtime, temp4);

	int result = genernate_qrcode_ex(temp3);
	if (0 != result)
	{
		::MessageBox(NULL, __T("Gernate qrcode file Failed"), __T("Error"), MB_OK);
		printf("error to gernate qrcode file\n");
		return FALSE;
	}

	/* cleanup */
	curl_free(temp4);

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

	Gdiplus::GdiplusShutdown(gdiplusToken);  
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	m_brush = wcex.hbrBackground;
	//wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32LESSON1);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON3));

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
HWND hWndmain = NULL, mWndControl = NULL, hWndButton = NULL;
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hStaticWnd;

	hInst = hInstance; // Store instance handle in our global variable
	hWndmain = CreateWindowEx(WS_EX_LAYERED, szWindowClass, NULL, WS_POPUP | WS_VISIBLE,
		0, 0, 330, 482, NULL, NULL, hInstance, NULL);

	if (!hWndmain)
	{
		return FALSE;
	}

	HDC hdc, hdcControl;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hWndmain, &ps);
	LoadPngImage(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), hWndmain);
	EndPaint(hWndmain, &ps);  

	// 创建第二个窗口，用来添加控件，并加载二维码 
	mWndControl = CreateWindow(szWindowClass, NULL, WS_POPUP ,
		CW_USEDEFAULT, CW_USEDEFAULT, 330, 482, hWndmain, NULL, hInstance, NULL);
	
	// 设置整个窗口位透明
	LONG t = GetWindowLong(mWndControl, GWL_EXSTYLE);
	t |= WS_EX_LAYERED;
	SetWindowLong(mWndControl, GWL_EXSTYLE, t);
	//::SetLayeredWindowAttributes(mWndControl, RGB(255, 0, 0), 111, LWA_COLORKEY | LWA_ALPHA); 
	RECT rect;
	GetWindowRect(mWndControl, &rect);

	HRGN hRgn = CreateRectRgn(rect.left, rect.top, rect.right, rect.bottom);
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
	HDC hDC = GetWindowDC(mWndControl);
	SelectObject(hDC, hRgn);
	SelectObject(hDC, hBrush);
	FillRgn(hDC, hRgn, hBrush); 

	SetLayeredWindowAttributes(mWndControl, RGB(0, 0, 0), 255/*any*/, LWA_COLORKEY);

	ReleaseDC(mWndControl, hDC);
	DeleteObject(hBrush);
	DeleteObject(hRgn);

	DWORD dwErr = GetLastError();
	HWND hWndStatic = CreateWindow(_T("STATIC"),
	// 在第二个窗口上创建static控件
		NULL,
		WS_CHILD | SS_BITMAP | WS_VISIBLE,
		65, 130, 0, 0, mWndControl,0,  hInst, NULL);

	// 在static控件上加载BMP图片
	HBITMAP hBitmap;
	CImage fcimage;
	HRESULT hResult = fcimage.Load(qr_bmp_file);
	if (FAILED(hResult)) 
	{
		MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
		return false;
	}

	hBitmap = fcimage.Detach();
	// Verify that the image was loaded
	if (hBitmap == NULL) {
		DWORD dErr = GetLastError();
		::MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
		return false;
	}
	SendMessage(hWndStatic, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);

	//在第二个窗口上创建关闭按钮
	LONG BtnStyle = WS_POPUP | WS_VISIBLE | BS_PUSHBUTTON;
	/*HWND hWndButton = CreateWindow(TEXT("button"), "",  
		BtnStyle,  
		rect.right - rect.left - 40, 20, 28, 28, mWndControl, NULL,  
		hInst, NULL); */
	hWndButton = CreateWindowEx(WS_EX_LAYERED, szWindowClass, "", BtnStyle,
		rect.right - rect.left - 40, 20, 28, 28, mWndControl, NULL, hInstance, NULL);

	LoadPngImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), _T("PNG"), hWndButton);

	ShowWindow(hWndmain, nCmdShow);
	UpdateWindow(hWndmain);

	return TRUE;
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
	RECT rect;
	TRACKMOUSEEVENT tme = {0};

	switch (message)
	{
	case WM_CREATE:
		{
			int scrWidth,scrHeight;
			RECT rect;
			// Load the bitmap image file
			HBITMAP hBitmap;
			CImage fcimage;
			HRESULT hResult = fcimage.Load(qr_bmp_file);
			if (FAILED(hResult)) 
			{
				MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
				return false;
			}
			hBitmap = fcimage.Detach();
			// Verify that the image was loaded
			if (hBitmap == NULL) {
				::MessageBox(NULL, __T("LoadImage Failed"), __T("Error"), MB_OK);
				return false;
			}
			// Get the bitmap's parameters and verify the get
			BITMAP qBitmap;
			int iReturn = GetObject(reinterpret_cast<HGDIOBJ>(hBitmap), sizeof(BITMAP),
				reinterpret_cast<LPVOID>(&qBitmap));
			if (!iReturn) {
				::MessageBox(NULL, __T("GetObject Failed"), __T("Error"), MB_OK);
				::DeleteObject(hBitmap);
				return false;
			}

			scrWidth=GetSystemMetrics(SM_CXSCREEN);
			scrHeight=GetSystemMetrics(SM_CYSCREEN);
			GetWindowRect(hWnd,&rect);
			rect.left=(scrWidth-rect.right)/2;
			rect.top=(scrHeight-rect.bottom)/2;
			SetWindowPos(hWnd, HWND_TOP, rect.left, rect.top, 
				330, 482, SWP_SHOWWINDOW);

			PostMessageW(hWnd,WM_PAINT,NULL,NULL);  

			::DeleteObject(hBitmap);
		}
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
		LoadPngImage(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), hWndmain);
		EndPaint(hWnd, &ps);  
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_WINDOWPOSCHANGED:
	case WM_SIZE:
	case WM_WINDOWPOSCHANGING:
		{
			WINDOWPOS * winPos = (WINDOWPOS*)lParam;
			if (hWnd == mWndControl)
			{
				MoveWindow(hWndmain, winPos->x, winPos->y, winPos->cx, winPos->cy,TRUE);
			}
			
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_ERASEBKGND:
		//LoadPngImage(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), hWndmain);
		return 0;

	case WM_LBUTTONDOWN:
		if (hWnd == hWndButton)
			DestroyWindow(hWndmain);
		if (hWnd == mWndControl)
			SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		break;

	//case WM_LBUTTONDBLCLK:
	case WM_MOUSEMOVE:
		{
			POINT pt;
			GetCursorPos(&pt);
			//ScreenToClient(hWnd, &pt);
			GetWindowRect(hWndButton, &rect);

			if (PtInRect(&rect, pt))
				LoadPngImage(MAKEINTRESOURCE(IDB_PNG_CLOSE_PRESS), _T("PNG"), hWndButton);
			else
				LoadPngImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), _T("PNG"), hWndButton);

			tme.cbSize = sizeof(tme);
			tme.dwFlags = TME_HOVER | TME_LEAVE;
			tme.hwndTrack = hWnd;
			tme.dwHoverTime = 50; //鼠标在窗口上停留50毫秒时发送WM_MOUSEHOVER，可以改
			TrackMouseEvent(&tme);
		}
		break;
	case WM_MOUSELEAVE:
		if (hWndButton == hWnd)
			LoadPngImage(MAKEINTRESOURCE(IDB_PNG_CLOSE), _T("PNG"), hWndButton);
		if (hWndmain == hWnd)
			LoadPngImage(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), hWndmain);
		break;
	//case WM_LBUTTONUP:
	case WM_MOVE:
	//case WM_MOUSEACTIVATE:
	//case WM_ACTIVATEAPP:
	//case WM_NCCALCSIZE:
	//case WM_WINDOWPOSCHANGED:
	//case WM_SIZE:
	case WM_NCMOUSEMOVE:
		if (hWnd == hWndmain)
			LoadPngImage(MAKEINTRESOURCE(IDB_PNG_BG), _T("PNG"), hWndmain);
//			return DefWindowProc(hWnd, message, wParam, lParam);
		break;

	case WM_CTLCOLOR:
		m_brush = CreateSolidBrush(RGB(255, 0, 255));
		return (INT_PTR)m_brush;

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


void Convert(const char* strIn, char* strOut, int sourceCodepage, int targetCodepage)  
{  
	int len = lstrlen(strIn);  
	int unicodeLen = MultiByteToWideChar(sourceCodepage, 0, strIn, -1, NULL, 0);  

	wchar_t* pUnicode = NULL;  
	pUnicode = new wchar_t[unicodeLen + 1];  
	memset(pUnicode, 0, (unicodeLen + 1)*sizeof(wchar_t));  
	MultiByteToWideChar(sourceCodepage, 0, strIn, -1, (LPWSTR)pUnicode, unicodeLen);  

	BYTE * pTargetData = NULL;  
	int targetLen = WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, 0, NULL, NULL);  

	pTargetData = new BYTE[targetLen + 1];  
	memset(pTargetData, 0, targetLen + 1);  
	WideCharToMultiByte(targetCodepage, 0, (LPWSTR)pUnicode, -1, (char *)pTargetData, targetLen, NULL, NULL);  
	lstrcpy(strOut, (char*)pTargetData);  

	delete pUnicode;  
	delete pTargetData;  
} 




int  get_domain_name(char *domain_name)
{
	NET_API_STATUS nStatus;

    LPCWSTR lpDcName = NULL;

	if (NULL == domain_name)
	{
		fprintf(stderr, "Error: the domain name can not be null\n");
		return -1;
	}

    //
    // Call the NetGetDCName function
    //
    nStatus = NetGetDCName(NULL, NULL, (LPBYTE *) &lpDcName);
    //
    // If the call succeeds,
    //
    if (nStatus == NERR_Success) {
        wprintf(L"NetGetDCName was successful\n", nStatus);
        wprintf(L"DC Name = %ws\n", lpDcName);

		DWORD ret = GetEnvironmentVariable("USERDOMAIN", domain_name, MAX_PATH);
		if ( 0 == ret) {
			fprintf(stderr, "Failed to get domain name\n");
			memset(domain_name, 0 , MAX_PATH);
		} else {
			
			fprintf(stdout, "The domain name is ========== %s\n", domain_name);
			strcat(domain_name, DOMAIN_SUFFIX);
			fprintf(stdout, "After strcat domain name is %s\n", domain_name);
			for (char* ptr = domain_name; *ptr; ptr++) {  
				*ptr = tolower(*ptr);  //change to lower case
			}  
			fprintf(stdout, "The lower case domain name is %s\n", domain_name);
		}
        // Need to free the returned buffer
        nStatus = NetApiBufferFree( (LPVOID) lpDcName);
        if (nStatus != NERR_Success)
            wprintf(L"NetApiBufferFree failed with error: %lu (0x%lx)\n",
                nStatus, nStatus);
    } else {
        wprintf(L"NetGetDCName failed with error: %lu (0x%lx)\n", nStatus,
                nStatus);
        wprintf(L"   Error = ");
		memset(domain_name, 0 , MAX_PATH);
        switch (nStatus) {
        case ERROR_INVALID_PARAMETER:
            wprintf(L"ERROR_INVALID_PARAMETER\n");
            break;
        case ERROR_NO_SUCH_DOMAIN:
            wprintf(L"ERROR_NO_SUCH_DOMAIN\n");
            break;
        case ERROR_NOT_SUPPORTED:
            wprintf(L"ERROR_NOT_SUPPORTED\n");
            break;
        case ERROR_BAD_NETPATH:
            wprintf(L"ERROR_BAD_NETPATH\n");
            break;
        case ERROR_INVALID_COMPUTERNAME:
            wprintf(L"ERROR_INVALID_COMPUTERNAME\n");
            break;
        case DNS_ERROR_INVALID_NAME_CHAR:
            wprintf(L"DNS_ERROR_INVALID_NAME_CHAR\n");
            break;
        case DNS_ERROR_NON_RFC_NAME:
            wprintf(L"DNS_ERROR_NON_RFC_NAME\n");
            break;
        case ERROR_INVALID_NAME:
            wprintf(L"ERROR_INVALID_NAME\n");
            break;
        case NERR_DCNotFound:
            wprintf(L"NERR_DCNotFound\n");
            break;
        case NERR_WkstaNotStarted:
            wprintf(L"NERR_WkstaNotStarted\n");
            break;
        case RPC_S_SERVER_UNAVAILABLE:
            wprintf(L"RPC_S_SERVER_UNAVAILABLE\n");
            break;
        case RPC_E_REMOTE_DISABLED:
            wprintf(L"RPC_E_REMOTE_DISABLED\n");
            break;
        default:
            wprintf(L"Other error, see Winerror.h or lmerr.h)\n");
            break;
        }
    }

	return strlen(domain_name);
}

// Creates a stream object initialized with the data from an executable resource.
IStream * CreateStreamOnResource(LPCTSTR lpName, LPCTSTR lpType)
{
    // initialize return value
    IStream * ipStream = NULL;
 
    // find the resource
    HRSRC hrsrc = FindResource(NULL, lpName, lpType);
    if (hrsrc == NULL)
        goto Return;
 
    // load the resource
    DWORD dwResourceSize = SizeofResource(NULL, hrsrc);
    HGLOBAL hglbImage = LoadResource(NULL, hrsrc);
    if (hglbImage == NULL)
        goto Return;
 
    // lock the resource, getting a pointer to its data
    LPVOID pvSourceResourceData = LockResource(hglbImage);
    if (pvSourceResourceData == NULL)
        goto Return;
 
    // allocate memory to hold the resource data
    HGLOBAL hgblResourceData = GlobalAlloc(GMEM_MOVEABLE, dwResourceSize);
    if (hgblResourceData == NULL)
        goto Return;
 
    // get a pointer to the allocated memory
    LPVOID pvResourceData = GlobalLock(hgblResourceData);
    if (pvResourceData == NULL)
        goto FreeData;
 
    // copy the data from the resource to the new memory block
    CopyMemory(pvResourceData, pvSourceResourceData, dwResourceSize);
    GlobalUnlock(hgblResourceData);
 
    // create a stream on the HGLOBAL containing the data
    if (SUCCEEDED(CreateStreamOnHGlobal(hgblResourceData, TRUE, &ipStream)))
        goto Return;
 
FreeData:
    // couldn't create stream; free the memory
    GlobalFree(hgblResourceData);
 
Return:
    // no need to unlock or free the resource
    return ipStream;
}

// Loads a PNG image from the specified stream (using Windows Imaging Component).
IWICBitmapSource * LoadBitmapFromStream(IStream * ipImageStream)
{
    // initialize return value
    IWICBitmapSource * ipBitmap = NULL;
 
    // load WIC's PNG decoder
    IWICBitmapDecoder * ipDecoder = NULL;
    if (FAILED(CoCreateInstance(CLSID_WICPngDecoder, NULL, CLSCTX_INPROC_SERVER, __uuidof(ipDecoder), reinterpret_cast<void**>(&ipDecoder))))
        goto Return;
 
    // load the PNG
    if (FAILED(ipDecoder->Initialize(ipImageStream, WICDecodeMetadataCacheOnLoad)))
        goto ReleaseDecoder;
 
    // check for the presence of the first frame in the bitmap
    UINT nFrameCount = 0;
    if (FAILED(ipDecoder->GetFrameCount(&nFrameCount)) || nFrameCount != 1)
        goto ReleaseDecoder;
 
    // load the first frame (i.e., the image)
    IWICBitmapFrameDecode * ipFrame = NULL;
    if (FAILED(ipDecoder->GetFrame(0, &ipFrame)))
        goto ReleaseDecoder;
 
    // convert the image to 32bpp BGRA format with pre-multiplied alpha
    //   (it may not be stored in that format natively in the PNG resource,
    //   but we need this format to create the DIB to use on-screen)
    WICConvertBitmapSource(GUID_WICPixelFormat32bppPBGRA, ipFrame, &ipBitmap);
    ipFrame->Release();
 
ReleaseDecoder:
    ipDecoder->Release();
Return:
    return ipBitmap;
}

HBITMAP CreateHBITMAP(IWICBitmapSource * ipBitmap)
{
    // initialize return value
    HBITMAP hbmp = NULL;
 
    // get image attributes and check for valid image
    UINT width = 0;
    UINT height = 0;
    if (FAILED(ipBitmap->GetSize(&width, &height)) || width == 0 || height == 0)
        goto Return;
 
    // prepare structure giving bitmap information (negative height indicates a top-down DIB)
    BITMAPINFO bminfo;
    ZeroMemory(&bminfo, sizeof(bminfo));
    bminfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bminfo.bmiHeader.biWidth = width;
    bminfo.bmiHeader.biHeight = -((LONG) height);
    bminfo.bmiHeader.biPlanes = 1;
    bminfo.bmiHeader.biBitCount = 32;
    bminfo.bmiHeader.biCompression = BI_RGB;
 
    // create a DIB section that can hold the image
    void * pvImageBits = NULL;
    HDC hdcScreen = GetDC(NULL);
    hbmp = CreateDIBSection(hdcScreen, &bminfo, DIB_RGB_COLORS, &pvImageBits, NULL, 0);
    ReleaseDC(NULL, hdcScreen);
    if (hbmp == NULL)
        goto Return;
 
    // extract the image into the HBITMAP
    const UINT cbStride = width * 4;
    const UINT cbImage = cbStride * height;
    if (FAILED(ipBitmap->CopyPixels(NULL, cbStride, cbImage, static_cast<BYTE *>(pvImageBits))))
    {
        // couldn't extract image; delete HBITMAP
        DeleteObject(hbmp);
        hbmp = NULL;
    }
 
Return:
    return hbmp;
}

// Loads the PNG containing the splash image into a HBITMAP.
HBITMAP LoadSplashImage(LPCTSTR lpName, LPCTSTR lpType)
{
    HBITMAP hbmpSplash = NULL;
 
    // load the PNG image data into a stream
    IStream * ipImageStream = CreateStreamOnResource(lpName, lpType);
    if (ipImageStream == NULL)
        goto Return;
 
    // load the bitmap with WIC
    IWICBitmapSource * ipBitmap = LoadBitmapFromStream(ipImageStream);
    if (ipBitmap == NULL)
        goto ReleaseStream;
 
    // create a HBITMAP containing the image
    hbmpSplash = CreateHBITMAP(ipBitmap);
    ipBitmap->Release();
 
ReleaseStream:
    ipImageStream->Release();
Return:
    return hbmpSplash;
}


// Calls UpdateLayeredWindow to set a bitmap (with alpha) as the content of the splash window.
void SetSplashImage(HWND hwndSplash, HBITMAP hbmpSplash)
{
    // get the size of the bitmap
    BITMAP bm;
    GetObject(hbmpSplash, sizeof(bm), &bm);
    SIZE sizeSplash = { bm.bmWidth, bm.bmHeight };
 
    // get the primary monitor's info
    POINT ptZero = { 0 };
    HMONITOR hmonPrimary = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorinfo = { 0 };
    monitorinfo.cbSize = sizeof(monitorinfo);
    GetMonitorInfo(hmonPrimary, &monitorinfo);
 
    // center the splash screen in the middle of the primary work area
    /*const RECT & rcWork = monitorinfo.rcWork;
    POINT ptOrigin;
    ptOrigin.x = rcWork.left + (rcWork.right - rcWork.left - sizeSplash.cx) / 2;
    ptOrigin.y = rcWork.top + (rcWork.bottom - rcWork.top - sizeSplash.cy) / 2;*/
	RECT windowRect;
	
	POINT ptOrigin;
	if (hWndmain == hwndSplash)
	{
		if (!mWndControl)
		{
			GetWindowRect(hwndSplash,&windowRect);
			ptOrigin.x = windowRect.left;
			ptOrigin.y = windowRect.top;
		}
		else
		{
			GetWindowRect(mWndControl,&windowRect);
			ptOrigin.x = windowRect.left;
			ptOrigin.y = windowRect.top;
		}
	}
	else if ((hWndButton == hwndSplash))
	{
		GetWindowRect(mWndControl,&windowRect);
		ptOrigin.x = windowRect.right - 60;
		ptOrigin.y = windowRect.top + 30;
	}
	
    // create a memory DC holding the splash bitmap
    HDC hdcScreen = ::GetDC(hwndSplash);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmpOld = (HBITMAP) SelectObject(hdcMem, hbmpSplash);
 
    // use the source image's alpha channel for blending
    BLENDFUNCTION blend = { 0 };
    blend.BlendOp = AC_SRC_OVER;
    blend.SourceConstantAlpha = 250;
    blend.AlphaFormat = AC_SRC_ALPHA;
 
    // paint the window (in the right location) with the alpha-blended bitmap
	/*SetWindowLong (hwndSplash, 
               GWL_EXSTYLE, 
               GetWindowLong(hwndSplash,GWL_EXSTYLE) & ~WS_EX_LAYOUTRTL);*/

	/*if ( !UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash,
        hdcMem, &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA))
	{
		DWORD dwErr = GetLastError();
		assert(L"UpdateLayeredWindow 调用失败");
		TCHAR tmp[255] = {_T('\0')};
	}*/

	if ( !UpdateLayeredWindow(hwndSplash, hdcScreen, &ptOrigin, &sizeSplash,
        hdcMem, &ptZero, RGB(0, 0, 0), &blend, ULW_ALPHA))
	{
		DWORD dwErr = GetLastError();
		assert(L"UpdateLayeredWindow 调用失败");
		TCHAR tmp[255] = {_T('\0')};
	}
 
    // delete temporary objects
    SelectObject(hdcMem, hbmpOld);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);
}

bool LoadPngImage(LPCTSTR lpName, LPCTSTR lpType, HWND hWnd)
{
	// Load the bitmap image file
	HBITMAP hBitmap;
	hBitmap = LoadSplashImage(lpName, lpType);
 	SetSplashImage(hWnd, hBitmap);

	::DeleteObject(hBitmap);
	return true;
}
