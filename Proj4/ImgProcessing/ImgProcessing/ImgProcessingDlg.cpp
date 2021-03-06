
// ImgProcessingDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ImgProcessing.h"
#include "ImgProcessingDlg.h"
#include "afxdialogex.h"

#include<gdiplus.h>
using namespace Gdiplus;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CImgProcessingDlg 对话框



CImgProcessingDlg::CImgProcessingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_IMGPROCESSING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


	
}

void CImgProcessingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PIC_CRTL, m_picCtrl);
	DDX_Control(pDX, IDC_WINDOW_WIDTH, m_wd_slider);
	DDX_Control(pDX, IDC_WINDOW_LEVEL, m_wl_slider);
	DDX_Control(pDX, IDC_SLIDER2, m_N_slider);
	DDX_Control(pDX, IDC_SLIDER3, m_scale_slider);
	DDX_Control(pDX, IDC_SLIDER4, m_maxCG_slider);
}

BEGIN_MESSAGE_MAP(CImgProcessingDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_COMMAND(IDM_OPEN_IMAGE, &CImgProcessingDlg::OnOpenImage)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CImgProcessingDlg 消息处理程序

BOOL CImgProcessingDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	//GetClientRect(m_Rect);
	m_picCtrl.GetWindowRect(m_Rect);
	m_wd_slider.SetRange(0, 4096);
	m_wd_slider.SetPos(4096);
	m_wl_slider.SetRange(0, 4096);
	m_wl_slider.SetPos(2048);
	m_N_slider.SetRange(5, 200);
	m_N_slider.SetPos(50);
	m_scale_slider.SetRange(1, 300);
	m_scale_slider.SetPos(100);
	m_maxCG_slider.SetRange(2, 20);
	m_maxCG_slider.SetPos(3);
	
	m_zoom = 1.0f;
	m_imgX = 0.0f;
	m_imgY = 0.0f;
	m_PtStart.X = 0.0f;
	m_PtStart.Y = 0.0f;

	m_mousepressed = false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CImgProcessingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CImgProcessingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CImgProcessingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CImgProcessingDlg::Invalidate()
{

	//HDC hdc = ::GetDC(m_hWnd);
	HDC hdc = ::GetDC(m_picCtrl);
	CDC dc;
	dc.Attach(hdc);
	CDC memDC;
	CBitmap MemBitmap;
	// 设备描述表初始化
	memDC.CreateCompatibleDC(NULL);
	// 建立与屏幕显示兼容的内存显示设备
	MemBitmap.CreateCompatibleBitmap(&dc, m_Rect.Width(), m_Rect.Height());
	// 选取空白位图
	memDC.SelectObject(MemBitmap);
	memDC.FillSolidRect(0, 0, m_Rect.Width(), m_Rect.Height(), RGB(255, 255, 255));
	//画图
	Draw(&memDC);
	//拷贝到控件DC
	dc.BitBlt(0, 0, m_Rect.Width(), m_Rect.Height(), &memDC, 0, 0, SRCCOPY);
	MemBitmap.DeleteObject();
	memDC.DeleteDC();
	dc.Detach();
	::ReleaseDC(m_hWnd, hdc);
}

void CImgProcessingDlg::Draw(CDC *pDC)
{
	Image img(_T("lena.bmp")); //注意，请设置你自己的图片
	Bitmap *img2 = drImg.getBitmap();
	Graphics graph(pDC->GetSafeHdc());
	graph.SetInterpolationMode(InterpolationModeHighQualityBilinear); //设置缩放质量
	graph.ScaleTransform(m_zoom, m_zoom);                             //缩放
	//graph.DrawImage(&img, m_imgX, m_imgY);                            //m_imgX,m_imgY是原图应该偏移的量
	graph.DrawImage(img2, m_imgX, m_imgY);
}


void CImgProcessingDlg::OnOpenImage()
{
	// TODO: 在此添加命令处理程序代码
	//Invalidate();
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		(LPCTSTR)_TEXT("DR Files (*.dr)|*.dr|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		drImg.isOpen = false;
		if (drImg.open(dlg.GetPathName())) {
			m_zoom = 1.0f;
			m_imgX = 0.0f;
			m_imgY = 0.0f;
			m_PtStart.X = 0.0f;
			m_PtStart.Y = 0.0f;
			Invalidate();
			drImg.isOpen = true;
		}
		
	}
	else
	{
		return;
	}
}


void CImgProcessingDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (drImg.isOpen) {
		if (m_Rect.PtInRect(point))
			if (nFlags & MK_LBUTTON)
			{
				REAL deltaX = point.x - m_mouseDown.X; //x轴方向偏移
				REAL deltaY = point.y - m_mouseDown.Y; //y轴方向偏移

				m_imgX = (m_PtStart.X + (deltaX / m_zoom)); //在原始图像中X坐标偏移，这里计算的是在原始图片中的位移，原因在上面已经说明，全局的仿射变换会影响实际的位移
				m_imgY = (m_PtStart.Y + (deltaY / m_zoom)); //在原始图像中Y坐标偏移
				Invalidate();                            //重绘
			}
	}
	
	CDialog::OnMouseMove(nFlags, point);
}


BOOL CImgProcessingDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (drImg.isOpen)
	{
		if (m_Rect.PtInRect(pt))
		{
			REAL oldzoom = m_zoom; //保存当前的缩放系数，用于计算当前滚动时的坐标
			CPoint ppt = pt;
			CRect rect;
			if (zDelta == 120)
			{
				m_zoom += 0.4f;
			}
			if (zDelta == -120)
			{
				m_zoom = m_zoom - 0.4f;
			}

			GetWindowRect(rect); //注意这里的区域是窗口相对于整个屏幕的，因为cpoint pt这个点是相对于屏幕的

			ppt.x -= rect.left; //计算该点在对话框中的位置
			ppt.y -= rect.top;

			int x = ppt.x - m_Rect.left;
			int y = ppt.y - m_Rect.top;

			REAL oldimagex = (x / oldzoom); //缩放前鼠标在原图中的位置
			REAL oldimagey = (y / oldzoom);

			REAL newimagex = (x / m_zoom); //缩放后鼠标在原图中的位置
			REAL newimagey = (y / m_zoom);

			m_imgX = newimagex - oldimagex + m_imgX; //计算原图应该的偏移
			m_imgY = newimagey - oldimagey + m_imgY;

			
			Invalidate(); //绘图
		}
	}
	
	return CDialog::OnMouseWheel(nFlags, zDelta, pt);
}


void CImgProcessingDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	m_mousepressed = false;
	CDialog::OnLButtonUp(nFlags, point);
}


void CImgProcessingDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (drImg.isOpen)
	{
		if (nFlags & MK_LBUTTON)
		{
			if (m_Rect.PtInRect(point))
			{
				if (!m_mousepressed)
				{
					m_mousepressed = true;
					m_mouseDown.X = point.x;
					m_mouseDown.Y = point.y;
					m_PtStart.X = m_imgX;
					m_PtStart.Y = m_imgY;
				}
			}
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CImgProcessingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (drImg.isOpen)
	{
		if ((&m_wd_slider) == (CSliderCtrl *)pScrollBar)
		{
			int nPos = m_wd_slider.GetPos();
			drImg.windowTechInterface(nPos, windowLevel);
			windowWidth = nPos;
			CString str;
			str.Format(_T("%d"), nPos);
			SetDlgItemText(IDC_STATIC_WD, str);
			Invalidate();
		}
		else if ((&m_wl_slider) == (CSliderCtrl *)pScrollBar)
		{
			int nPos = m_wl_slider.GetPos();
			drImg.windowTechInterface(windowWidth, nPos);
			windowLevel = nPos;
			CString str;
			str.Format(_T("%d"), nPos);
			SetDlgItemText(IDC_STATIC_WL, str);
			Invalidate();
		}
		else if ((&m_N_slider) == (CSliderCtrl *)pScrollBar)
		{
			int nPos = m_N_slider.GetPos();
			drImg.setN(nPos);
			drImg.enhance();
			CString str;
			str.Format(_T("%d"), nPos);
			SetDlgItemText(IDC_STATIC_N, str);
			Invalidate();
		}
		else if ((&m_scale_slider) == (CSliderCtrl *)pScrollBar)
		{
			int nPos = m_scale_slider.GetPos();
			drImg.setScale(nPos);
			drImg.enhance();
			CString str;
			str.Format(_T("%d"), nPos);
			SetDlgItemText(IDC_STATIC_SCALE, str);
			Invalidate();
		}
		else if ((&m_maxCG_slider) == (CSliderCtrl *)pScrollBar)
		{
			int nPos = m_maxCG_slider.GetPos();
			drImg.setMaxCG(nPos);
			drImg.enhance();
			CString str;
			str.Format(_T("%d"), nPos);
			SetDlgItemText(IDC_STATIC_MAXCG, str);
			Invalidate();
		}
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

