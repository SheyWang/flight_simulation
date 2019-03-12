// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// DigitalEarthView.cpp : CDigitalEarthView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "DigitalEarth.h"
#endif

#include "DigitalEarthDoc.h"
#include "DigitalEarthView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CDigitalEarthView

IMPLEMENT_DYNCREATE(CDigitalEarthView, CView)

BEGIN_MESSAGE_MAP(CDigitalEarthView, CView)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_COMMAND(ID_CHINA_BOUND, &CDigitalEarthView::OnChinaBound)
	ON_COMMAND(ID_SET_CHINA_BOUND, &CDigitalEarthView::OnSetChinaBound)
	ON_COMMAND(ID_SHOW_CHINABOUND, &CDigitalEarthView::OnShowChinabound)
	ON_UPDATE_COMMAND_UI(ID_SHOW_CHINABOUND, &CDigitalEarthView::OnUpdateShowChinabound)
	ON_COMMAND(ID_START_AIRPORTFLY, &CDigitalEarthView::OnStartAirportfly)
	ON_UPDATE_COMMAND_UI(ID_START_AIRPORTFLY, &CDigitalEarthView::OnUpdateStartAirportfly)
	ON_COMMAND(ID_CHECK5_TRACK, &CDigitalEarthView::OnCheck5Track)
	ON_UPDATE_COMMAND_UI(ID_CHECK5_TRACK, &CDigitalEarthView::OnUpdateCheck5Track)
	ON_COMMAND(ID_PLAY_MUSIC, &CDigitalEarthView::OnPlayMusic)
END_MESSAGE_MAP()



// CDigitalEarthView 构造/析构

CDigitalEarthView::CDigitalEarthView()
{
	mOSG = 0;
	mThreadHandle = 0;
	isShowChinaBoundary = true;
	chinaBoundariesOpt = 1.0;
	isStartFly = false;
	isTrack = false;
}

CDigitalEarthView::~CDigitalEarthView()
{
}

BOOL CDigitalEarthView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CDigitalEarthView 绘制

void CDigitalEarthView::OnDraw(CDC* /*pDC*/)
{
	CDigitalEarthDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}

void CDigitalEarthView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CDigitalEarthView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CDigitalEarthView 诊断

#ifdef _DEBUG
void CDigitalEarthView::AssertValid() const
{
	CView::AssertValid();
}

void CDigitalEarthView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDigitalEarthDoc* CDigitalEarthView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDigitalEarthDoc)));
	return (CDigitalEarthDoc*)m_pDocument;
}
#endif //_DEBUG


// CDigitalEarthView 消息处理程序


int CDigitalEarthView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	mOSG = new COSGObject(m_hWnd);

	return 0;
}


BOOL CDigitalEarthView::OnEraseBkgnd(CDC* pDC)
{
	if(0 == mOSG)
	{
		return CView::OnEraseBkgnd(pDC);
	}
	else
	{
		return FALSE;
	}

	return CView::OnEraseBkgnd(pDC);
}


void CDigitalEarthView::OnDestroy()
{
	CView::OnDestroy();

	if(mOSG != 0) delete mOSG;
	WaitForSingleObject(mThreadHandle,1000);
	
}


void CDigitalEarthView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	mOSG->InitOSG();
	mThreadHandle = (HANDLE)_beginthread(&COSGObject::Render,0,mOSG);
}


void CDigitalEarthView::OnChinaBound()
{
	
}


void CDigitalEarthView::OnSetChinaBound()
{
	CDigitalEarthApp* pApp = (CDigitalEarthApp*)AfxGetApp();
	CMainFrame* pWnd = (CMainFrame*)pApp->GetMainWnd();
	CMFCRibbonEdit*edit = dynamic_cast<CMFCRibbonEdit*>(pWnd->m_wndRibbonBar.FindByID(ID_CHINA_BOUND));
	
	if(edit)
	{
		CString str = edit->GetEditText();
		std::string strTemp(str.GetBuffer());
		double opt = std::atof(strTemp.c_str());
		if(opt < 0)
		{
			MessageBox("透明度必须为数字且为正值","错误",MB_OK|MB_ICONERROR);
			str.Format("%f",chinaBoundariesOpt);
			edit->SetEditText(str);
		}
		else
		{
			chinaBoundariesOpt = opt;
			mOSG->setChinaBoundariesOpacity(opt);
		}
	}
}


void CDigitalEarthView::OnShowChinabound()
{
	theApp.bNeedModify = TRUE;
	while(!theApp.bCanModify) Sleep(1);
	isShowChinaBoundary = !isShowChinaBoundary;
	if(isShowChinaBoundary)
	{
		mOSG->addChinaBoundaryes();
	}
	else
	{
		mOSG->rmvChinaBoundaryes();
	}
	theApp.bNeedModify = FALSE;
}


void CDigitalEarthView::OnUpdateShowChinabound(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(isShowChinaBoundary);
}


void CDigitalEarthView::OnStartAirportfly()
{
	isStartFly = !isStartFly;
	isTrack = true;
	mOSG->DoPreLineNow();
}


void CDigitalEarthView::OnUpdateStartAirportfly(CCmdUI *pCmdUI)
{
	 pCmdUI->SetCheck(isStartFly);
}


void CDigitalEarthView::OnCheck5Track()
{
	isTrack = !isTrack;
	mOSG->isTrackFly(isTrack);
}


void CDigitalEarthView::OnUpdateCheck5Track(CCmdUI *pCmdUI)
{
	if(isStartFly == false)
	{
		pCmdUI->Enable(false);
		pCmdUI->SetCheck(false);
		isTrack =false;
	}

	else
	{
		pCmdUI->SetCheck(isTrack);
	}
}


void CDigitalEarthView::OnPlayMusic()
{
	WinExec("F:\\002.OpenSceneGraph\\019.Earth\\builder\\Mp3Player\\Debug\\Mp3Player.exe",SW_SHOW); // 以最大化的方式打开Mp3Player.exe
}

