//openBmpImage.cpp: 定义应用程序的入口点。
//
#include "stdafx.h"
#include "openBmpImage.h"
#include<errno.h>
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

BITMAPFILEHEADER bf;
BITMAPINFOHEADER bi;
HPALETTE hPalette = NULL;
HBITMAP  hBitmap = NULL;
HGLOBAL  hImgData = NULL;
HWND hWnd;

char strFilename[MAX_PATH] = {  };//用于接收文件名


// 此代码模块中包含的函数的前向声明: 
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool openBmpImage(HWND hWnd);
bool openFile();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_OPENBMPIMAGE, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化: 
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OPENBMPIMAGE));

    MSG msg;

    // 主消息循环: 
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OPENBMPIMAGE));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_OPENBMPIMAGE);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择: 
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
			case ID_OPEN_BMP:
				if (hBitmap != NULL) {
					DeleteObject(hBitmap);
					hBitmap = NULL;
				}
				if (hPalette != NULL) {
					DeleteObject(hPalette);
					hPalette = NULL;
				}
				if (hImgData != NULL) {
					GlobalUnlock(hImgData);
					GlobalFree(hImgData);
					hImgData = NULL;
				}
				if (openFile()) {
					if (openBmpImage(hWnd)) {
						InvalidateRect(hWnd, NULL, TRUE);
					}
				}//成功，则重画窗口
				break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps), hMemDC;

		if (hBitmap)
		{
			hMemDC = CreateCompatibleDC(hdc);
			if (hPalette)
			{
				SelectPalette(hdc, hPalette, FALSE);
				SelectPalette(hMemDC, hPalette, FALSE);
				RealizePalette(hdc);
			}
			SelectObject(hMemDC, hBitmap);
			BitBlt(hdc, 0, 0, bi.biWidth, bi.biHeight, hMemDC, 0, 0, SRCCOPY);
			DeleteDC(hMemDC);
		}
		// TODO: 在此处添加使用 hdc 的任何绘图代码...
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

// “关于”框的消息处理程序。
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



bool openBmpImage(HWND hWnd)
{
	LPBITMAPINFOHEADER lpImgData;//指向BITMAPINFOHEADER结构的指针
	LOGPALETTE *pPal; //指向逻辑调色板结构的指针
	LPRGBQUAD lpRGB; //指向RGBQUAD结构的指针
	HPALETTE hPrevPalette = NULL; //用来保存设备中原来的调色板
	HDC hDc; //设备句柄
	HLOCAL hPal; //存储调色板的局部内存句柄
	DWORD LineBytes;  //每一行的字节数
	DWORD ImgSize;   //实际的图象数据占用的字节数
	DWORD NumColors;//实际用到的颜色数 ，即调色板数组中的颜色个数
	DWORD i;
	
	//HANDLE file 
	FILE *fp;
	errno_t err = _wfopen_s(&fp, (LPWSTR)(strFilename), (wchar_t*)("r\0b\0"));
	if (fp == NULL) {
		MessageBox(NULL, LPWSTR(err), LPWSTR("123"), 0);
		return FALSE; //打开文件错误，返回
	}
	fread((LPSTR)&bf,sizeof(BITMAPFILEHEADER),1,fp);//将BITMAPFILEHEADER结构从文件中读出，填写到bf中

	fread((LPSTR)&bi,  sizeof(BITMAPINFOHEADER),1, fp);//将BITMAPINFOHEADER结构从文件中读出，填写到bi中

	LineBytes = (DWORD)WIDTHBYTES(bi.biWidth*bi.biBitCount);//LineBytes为每一行的字节数

	ImgSize = (DWORD)LineBytes*bi.biHeight;//ImgSize为实际的图象数据占用的字节数

	if (bi.biClrUsed != 0) {//如果bi.biClrUsed不为零，即为实际用到的颜色数
		NumColors = (DWORD)bi.biClrUsed;//NumColors为实际用到的颜色数 ，即调色板数组中的颜色个数
	}
	else { //否则，用到的颜色数为2biBitCount。
		switch (bi.biBitCount) {
		case 1:
			NumColors = 2;
			break;
		case 4:
			NumColors = 16;
			break;
		case 8:
			NumColors = 256;
			break;
		case 24:
			NumColors = 0; //对于真彩色图，没用到调色板
			break;
		default: //不处理其它的颜色数，认为出错。
			MessageBox(hWnd, LPCWSTR("Invalid color numbers!"), LPCWSTR("Error Message"), MB_OK | MB_ICONEXCLAMATION);
			fclose(fp);
			return FALSE; //关闭文件，返回FALSE
		}
	}

	if (bf.bfOffBits != (DWORD)(NumColors * sizeof(RGBQUAD) + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER))){
		//计算出的偏移量与实际偏移量不符，一定是颜色数出错
		MessageBox(hWnd, LPCWSTR("Invalid color numbers!"), LPCWSTR("Error Message"), MB_OK | MB_ICONEXCLAMATION);
		fclose(fp);
		return FALSE; //关闭文件，返回FALSE
	}

	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD) + ImgSize;

	//分配内存，大小为BITMAPINFOHEADER结构长度加调色板+实际位图
	if ((hImgData = GlobalAlloc(GHND, (DWORD)(sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD) + ImgSize))) == NULL){
		//分配内存错误
		MessageBox(hWnd, LPCWSTR("Error alloc memory!"), LPCWSTR("ErrorMessage"), MB_OK | MB_ICONEXCLAMATION);
		fclose(fp);
		return FALSE; //关闭文件，返回FALSE
	}

	lpImgData = (LPBITMAPINFOHEADER)GlobalLock(hImgData);//指针lpImgData指向该内存区

	fseek(fp, sizeof(BITMAPFILEHEADER), SEEK_SET);//文件指针重新定位到BITMAPINFOHEADER开始处

	fread((char *)lpImgData, (long)sizeof(BITMAPINFOHEADER) + (long)NumColors * sizeof(RGBQUAD) + ImgSize,1,fp);//将文件内容读入lpImgData


	fclose(fp);//关闭文件

	if (NumColors != 0){ //NumColors不为零，说明用到了调色板
		//为逻辑调色板分配局部内存，大小为逻辑调色板结构长度加NumColors个PALETTENTRY
		hPal = LocalAlloc(LHND, sizeof(LOGPALETTE)+NumColors * sizeof(PALETTEENTRY));

		pPal = (LOGPALETTE *)LocalLock(hPal);//指针pPal指向该内存区

		pPal->palNumEntries = NumColors;//填写逻辑调色板结构的头

		pPal->palVersion = 0x300;

		lpRGB = (LPRGBQUAD)((LPSTR)lpImgData+(DWORD)sizeof(BITMAPINFOHEADER));//lpRGB指向的是调色板开始的位置

		//填写每一项
		for (i = 0; i < NumColors; i++){
			pPal->palPalEntry[i].peRed = lpRGB->rgbRed;

			pPal->palPalEntry[i].peGreen = lpRGB->rgbGreen;

			pPal->palPalEntry[i].peBlue = lpRGB->rgbBlue;

			pPal->palPalEntry[i].peFlags = (BYTE)0;

			lpRGB++; //指针移到下一项
		}
		hPalette = CreatePalette(pPal);//产生逻辑调色板，hPalette是一个全局变量

		LocalUnlock(hPal);//释放局部内存

		LocalFree(hPal);
	}

	hDc = GetDC(hWnd);//获得设备上下文句柄

	if (hPalette){ //如果刚才产生了逻辑调色板
		hPrevPalette = SelectPalette(hDc, hPalette, FALSE);//将新的逻辑调色板选入DC，将旧的逻辑调色板句柄保存在 hPrevPalette

		RealizePalette(hDc);
	}

	hBitmap = CreateDIBitmap(hDc, (LPBITMAPINFOHEADER)lpImgData,
		(LONG)CBM_INIT,
		(LPSTR)lpImgData + sizeof(BITMAPINFOHEADER) + NumColors * sizeof(RGBQUAD),
		(LPBITMAPINFO)lpImgData, DIB_RGB_COLORS);//产生位图句柄

	if (hPalette && hPrevPalette){
		SelectPalette(hDc, hPrevPalette, FALSE);
		RealizePalette(hDc);
	}//将原来的调色板(如果有的话)选入设备上下文句柄

	ReleaseDC(hWnd, hDc); //释放设备上下文

	GlobalUnlock(hImgData); //解锁内存区

	return TRUE; //成功返回
}

bool openFile() {
	OPENFILENAME ofn = { 0 };
	//TCHAR strFilename[MAX_PATH] = { 0 };//用于接收文件名
	ofn.lStructSize = sizeof(OPENFILENAME);//结构体大小
	ofn.hwndOwner = NULL;//拥有着窗口句柄，为NULL表示对话框是非模态的，实际应用中一般都要有这个句柄
	ofn.lpstrFilter = TEXT("bmp图像\0*.bmp*\0所有文件\0*.*\0\0");//设置过滤
	ofn.nFilterIndex = 1;//过滤器索引
	ofn.lpstrFile = LPWSTR(strFilename);//接收返回的文件名，注意第一个字符需要为NULL
	ofn.nMaxFile = sizeof(strFilename);//缓冲区长度
	ofn.lpstrInitialDir = NULL;//初始目录为默认
	ofn.lpstrTitle = TEXT("请选择一个文件");//使用系统默认标题留空即可
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;//文件、目录必须存在，隐藏只读选项
	if (GetOpenFileName(&ofn))
	{
		MessageBox(NULL, LPWSTR(strFilename), TEXT("选择的文件"), 0);
		return true;
	}
	else {
		return false;
	}
}