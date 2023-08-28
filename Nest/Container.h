#pragma once

//---------------------------------------------------------------------

class Container
{
public:

	HWND m_hwnd = 0;
	Shuttle* m_shuttle = 0;

	Container(Shuttle* shuttle, DWORD style);
	virtual ~Container();

	virtual void onResizeDockContainer(LPCRECT rc);
	virtual void onResizeFloatContainer();
	virtual BOOL onSetContainerPos(WINDOWPOS* wp);
	virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	static BOOL registerWndClass();
	static LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Container* getContainer(HWND hwndContainer);
	static void setContainer(HWND hwndContainer, Container* container);
	static void removeContainer(HWND hwndContainer);
};

//---------------------------------------------------------------------

class SpreadContainer : public Container
{
public:

	SpreadContainer(Shuttle* shuttle, DWORD style);
	virtual ~SpreadContainer();

	virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//---------------------------------------------------------------------

class ScrollContainer : public Container
{
public:

	ScrollContainer(Shuttle* shuttle, DWORD style);
	virtual ~ScrollContainer();

	void updateScrollBar(HWND hwndContainer);
	void scroll(HWND hwndContainer, int bar, WPARAM wParam);
	void recalcLayout(HWND hwndContainer);
	virtual void onResizeDockContainer(LPCRECT rc);
	virtual void onResizeFloatContainer();
	virtual BOOL onSetContainerPos(WINDOWPOS* wp);
	virtual LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onTargetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//---------------------------------------------------------------------
