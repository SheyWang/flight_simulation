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

// DigitalEarthView.h : CDigitalEarthView 类的接口
//

#pragma once

#include "DigitalEarthDoc.h"
#include "OSGObject.h"


class CDigitalEarthView : public CView
{
protected: // 仅从序列化创建
	CDigitalEarthView();
	DECLARE_DYNCREATE(CDigitalEarthView)

// 特性
public:
	CDigitalEarthDoc* GetDocument() const;

public:
	COSGObject* mOSG;
	HANDLE mThreadHandle;

	double chinaBoundariesOpt;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// 实现
public:
	virtual ~CDigitalEarthView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


public:
	//自定义变量区
	BOOL isShowChinaBoundary;

	//是否启动预设置路径
	//bool isStartFly;
	//bool isTrack;








protected:

// 生成的消息映射函数
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	afx_msg void OnChinaBound();
	afx_msg void OnSetChinaBound();
	afx_msg void OnShowChinabound();
	afx_msg void OnUpdateShowChinabound(CCmdUI *pCmdUI);
	afx_msg void OnStartAirportfly();
	afx_msg void OnUpdateStartAirportfly(CCmdUI *pCmdUI);
	afx_msg void OnCheck5Track();
	afx_msg void OnUpdateCheck5Track(CCmdUI *pCmdUI);
	afx_msg void OnPlayMusic();
};

#ifndef _DEBUG  // DigitalEarthView.cpp 中的调试版本
inline CDigitalEarthDoc* CDigitalEarthView::GetDocument() const
   { return reinterpret_cast<CDigitalEarthDoc*>(m_pDocument); }
#endif

