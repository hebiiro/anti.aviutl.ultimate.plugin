#pragma once

//---------------------------------------------------------------------

class Shuttle : public std::enable_shared_from_this<Shuttle>
{
public:

	HWND m_hwnd = 0;
	ContainerPtr m_dockContainer;
	ContainerPtr m_floatContainer;
	WNDPROC m_targetWndProc = 0;
	Pane* m_pane = 0;
	_bstr_t m_name = L"";

	virtual ~Shuttle()
	{
		MY_TRACE(_T("Shuttle::~Shuttle(), 0x%08X, %ws\n"), m_hwnd, (BSTR)m_name);

		// このままだとコンテナが削除されるときターゲットウィンドウも削除されてしまうので、
		// ターゲットウィンドウの親ウィンドウを 0 にしておく。
		::SetParent(m_hwnd, 0);
	}

	virtual void init(HWND hwnd);
	virtual Container* onCreateDockContainer();
	virtual Container* onCreateFloatContainer();
	virtual DWORD onGetTargetNewStyle();
	virtual void onSetTargetWindowPos(LPRECT rc);
	virtual LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	void showTargetWindow();
	void dockWindow();
	void floatWindow();

	virtual void resizeDockContainer(LPCRECT rc);
	virtual void resizeFloatContainer();

	static LRESULT CALLBACK targetWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	static Shuttle* getShuttle(HWND hwnd);
	static void setShuttle(HWND hwnd, Shuttle* shuttle);
};

//---------------------------------------------------------------------

class AviUtlWindow : public Shuttle
{
public:

	virtual void init(HWND hwnd);
	virtual LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//---------------------------------------------------------------------

class ExEditWindow : public Shuttle
{
public:

	HWND m_dummy = 0;

	virtual void init(HWND hwnd);
	virtual DWORD onGetTargetNewStyle();
	virtual void onSetTargetWindowPos(LPRECT rc);
	virtual LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//---------------------------------------------------------------------

class SettingDialog : public Shuttle
{
public:

	virtual void init(HWND hwnd);
	virtual Container* onCreateDockContainer();
	virtual Container* onCreateFloatContainer();
	virtual LRESULT onContainerWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
	virtual LRESULT onTargetWndProc(Container* container, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
};

//---------------------------------------------------------------------
