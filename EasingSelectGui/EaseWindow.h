#pragma once

class CEaseWindow : public CWnd
{
public:

	struct Points
	{
		static const int first = 0;
		static const int second= 1;
		static const int maxSize = 2;
		static const int none = -1;
	};

	struct Position
	{
		static const int startBase = 25;
		static const int endBase = 75;

		static const int startX = 0;
		static const int startY = 0;
		static const int endX = 99;
		static const int endY = 99;
	};

	CPoint m_points[2];
	int m_hot;

	BOOL m_enable;
	_bstr_t m_origin;
	BOOL m_clamp;
	_bstr_t m_horz;
	_bstr_t m_vert;
	int m_alpha;
	int m_margin;
	int m_hitDistance;
	CSize m_windowSize;
	Color m_backgroundColor;
	Color m_borderColor;
	REAL m_borderWidth;
	Color m_curveColor;
	REAL m_curveWidth;
	Color m_invalidCurveColor;
	REAL m_invalidCurveWidth;
	Color m_handleColor;
	REAL m_handleWidth;
	Color m_pointColor;
	Color m_hotPointColor;
	int m_pointRadius;
	int m_segmentCount;
	BOOL m_hideCursor;
	BOOL m_immediately;

	CEaseWindow();
	virtual ~CEaseWindow();

	HRESULT loadEase(const MSXML2::IXMLDOMElementPtr& element);
	HRESULT saveEase(const MSXML2::IXMLDOMElementPtr& element);
	void getWorkarea(LPRECT rc);
	POINT LPToClient(POINT point);
	POINT LPToClient(POINT point, LPCRECT rc);
	POINT ClientToLP(POINT point);
	POINT ClientToLP(POINT point, LPCRECT rc);
	int hitTest(POINT point);
	void outputEaseText();
	void receiveNumber();
	void sendNumber();
	void show(HWND numberWindow, HWND easingWindow);
	void hide();

public:

	virtual BOOL Create(CWnd* parent);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg void OnCaptureChanged(CWnd* pWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
};
