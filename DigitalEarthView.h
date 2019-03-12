// ��� MFC ʾ��Դ������ʾ���ʹ�� MFC Microsoft Office Fluent �û����� 
// (��Fluent UI��)����ʾ�������ο���
// ���Բ��䡶Microsoft ������ο����� 
// MFC C++ ������渽����ص����ĵ���
// ���ơ�ʹ�û�ַ� Fluent UI ����������ǵ����ṩ�ġ�
// ��Ҫ�˽��й� Fluent UI ��ɼƻ�����ϸ��Ϣ�������  
// http://msdn.microsoft.com/officeui��
//
// ��Ȩ����(C) Microsoft Corporation
// ��������Ȩ����

// DigitalEarthView.h : CDigitalEarthView ��Ľӿ�
//

#pragma once

#include "DigitalEarthDoc.h"
#include "OSGObject.h"


class CDigitalEarthView : public CView
{
protected: // �������л�����
	CDigitalEarthView();
	DECLARE_DYNCREATE(CDigitalEarthView)

// ����
public:
	CDigitalEarthDoc* GetDocument() const;

public:
	COSGObject* mOSG;
	HANDLE mThreadHandle;

	double chinaBoundariesOpt;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// ʵ��
public:
	virtual ~CDigitalEarthView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


public:
	//�Զ��������
	BOOL isShowChinaBoundary;

	//�Ƿ�����Ԥ����·��
	//bool isStartFly;
	//bool isTrack;








protected:

// ���ɵ���Ϣӳ�亯��
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

#ifndef _DEBUG  // DigitalEarthView.cpp �еĵ��԰汾
inline CDigitalEarthDoc* CDigitalEarthView::GetDocument() const
   { return reinterpret_cast<CDigitalEarthDoc*>(m_pDocument); }
#endif

