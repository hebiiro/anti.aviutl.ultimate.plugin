#pragma once
#include "EaseWindow.h"

class CMainFrame : public CWnd
{
public:

	typedef std::vector<CRect> CParts;

public:

	CEaseWindow m_easeWindow;

	FileUpdateChecker m_fileUpdateChecker;
	BOOL m_isSettingsFileLoaded;

	int m_imageVersion;
	BOOL m_clamp;
	_bstr_t m_horz;
	_bstr_t m_vert;
	int m_alpha;
	int m_scale;
	Color m_selectedColor;
	Color m_hotColor;
	CImage m_image;
	CParts m_parts;
	int m_currentPart;
	int m_hotPart;

public:

	CMainFrame() noexcept;
	virtual ~CMainFrame();

	HRESULT loadSettings();
	HRESULT loadEasing(const MSXML2::IXMLDOMElementPtr& element);
	HRESULT saveSettings();
	HRESULT saveEasing(const MSXML2::IXMLDOMElementPtr& element);
	void loadImage();
	void setRect(int number, int x, int y);
	void setHotPart(int index);
	HWND getTarget();
	int getEasing();
	void setEasing(int index);
	void show(HWND target);
	void hide();

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	DECLARE_MESSAGE_MAP()
};
