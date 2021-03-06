
// ImgProcessingDlg.h: 头文件
//

#pragma once
#include"MyImage.h"
#include "afxwin.h"
#include "afxcmn.h"

// CImgProcessingDlg 对话框
class CImgProcessingDlg : public CDialog
{
private:
	REAL m_zoom;  //缩放系数
	CRect m_Rect; //对话框的矩形区域

	REAL m_imgX; //图片左上角X轴偏移量
	REAL m_imgY; //图片左上角Y轴偏移量

	PointF m_PtStart;    //按下鼠标左键，记录当前的m_imgX,m_imgY
	PointF m_mouseDown;  //按下鼠标左键时，鼠标在图片控件中的位置；
	bool m_mousepressed; //记录是否按下了鼠标左键

	MyImage drImg;
	CWnd m_PicWnd;

	int windowWidth;
	int windowLevel;
// 构造
public:
	CImgProcessingDlg(CWnd* pParent = NULL);	// 标准构造函数

	void Invalidate();
	void Draw(CDC *pDC);
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMGPROCESSING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnOpenImage();
	CStatic m_picCtrl;
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	CSliderCtrl m_wd_slider;
	CSliderCtrl m_wl_slider;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_N_slider;
	CSliderCtrl m_scale_slider;
	CSliderCtrl m_maxCG_slider;
};
