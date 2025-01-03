#pragma once

namespace apn::item_copy
{
	//
	// このクラスはドロップソースです。
	//
	struct ShellBrowser : IShellBrowser, ICommDlgBrowser, IServiceProvider
	{
		//
		// このクラスはドロップソースの生成を担当します。
		//
		struct Allocator
		{
			inline static auto create(HWND parent)
			{
				return new ShellBrowser(parent);
			}

			inline static auto release(ShellBrowser* p)
			{
				return delete p;
			}
		};

		//
		// このCOMオブジェクトの参照カウントです。
		//
		ULONG ref_count = 1;

		//
		// 親ウィンドウです。
		//
		HWND parent = nullptr;

		//
		// コンストラクタです。
		//
		ShellBrowser(HWND parent)
			: parent(parent)
		{
		}

		//
		// IUnknownの実装です。
		//
		STDMETHODIMP_(ULONG) AddRef() override
		{
			MY_TRACE_FUNC("");

			return ::InterlockedIncrement(&ref_count);
		}

		//
		// IUnknownの実装です。
		//
		STDMETHODIMP_(ULONG) Release() override
		{
			MY_TRACE_FUNC("");

			auto result = ::InterlockedDecrement(&ref_count);
			if (result == 0) Allocator::release(this);
			return result;
		}

		//
		// IUnknownの実装です。
		//
		STDMETHODIMP QueryInterface(REFIID iid, void** ppv) override
		{
//			MY_TRACE_FUNC("");

			if (::IsEqualIID(iid, __uuidof(IUnknown)) ||
				::IsEqualIID(iid, __uuidof(IShellBrowser)) ||
				::IsEqualIID(iid, __uuidof(IOleWindow)))
			{
				*ppv = static_cast<IShellBrowser*>(this);
			}
#if 0
			else if (::IsEqualIID(iid, __uuidof(ICommDlgBrowser)))
			{
				*ppv = static_cast<ICommDlgBrowser*>(this);
			}
			else if (::IsEqualIID(iid, __uuidof(IServiceProvider)))
			{
				*ppv = static_cast<IServiceProvider*>(this);
			}
#endif
			else
			{
				*ppv = nullptr;
				return E_NOINTERFACE;
			}

			AddRef();
			return S_OK;
		}

		//
		// IShellBrowserの実装です。
		//
		STDMETHODIMP InsertMenusSB(HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP SetMenuSB(HMENU hmenuShared, HOLEMENU holemenuRes, HWND hwndActiveObject) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP RemoveMenusSB(HMENU hmenuShared) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP SetStatusTextSB(LPCWSTR pszStatusText) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP EnableModelessSB(BOOL fEnable) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP TranslateAcceleratorSB(MSG* pmsg, WORD wID) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP BrowseObject(PCUIDLIST_RELATIVE pidl, UINT wFlags) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP GetViewStateStream(DWORD grfMode, IStream** ppStrm) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP GetControlWindow(UINT id, HWND* hwnd) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP SendControlMsg(UINT id, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT* pret) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP QueryActiveShellView(IShellView** ppshv) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP OnViewWindowActive(IShellView* pshv) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }
		STDMETHODIMP SetToolbarItems(LPTBBUTTONSB lpButtons, UINT nButtons, UINT uFlags) override { MY_TRACE_FUNC(""); return E_NOTIMPL; }

		//
		// IOleWindowの実装です。
		//
		STDMETHODIMP GetWindow(HWND* hwnd) override
		{
			MY_TRACE_FUNC("");

			if (!hwnd) return E_POINTER;
			*hwnd = parent;
			return S_OK;
		}

		//
		// IOleWindowの実装です。
		//
		STDMETHODIMP ContextSensitiveHelp(BOOL enter_mode) override
		{
			MY_TRACE_FUNC("");

			return E_NOTIMPL;
		}

		//
		// ICommDlgBrowserの実装です。
		//
		STDMETHODIMP OnDefaultCommand(IShellView*) override
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}

		STDMETHODIMP OnStateChange(IShellView* shell_view, ULONG change) override
		{
			MY_TRACE_FUNC("{:#010x}", change);

			return S_OK;
		}

		STDMETHODIMP IncludeObject(IShellView*, PCUITEMID_CHILD) override
		{
			MY_TRACE_FUNC("");

			return S_OK;
		}

		//
		// IServiceProviderの実装です。
		//
		STDMETHODIMP QueryService(REFGUID sid, REFIID iid, void** ppv) override
		{
			MY_TRACE_FUNC("");

			if (!ppv) return E_POINTER;
			*ppv = nullptr;

			if (::IsEqualGUID(sid, __uuidof(IShellBrowser)) || ::IsEqualGUID(sid, __uuidof(ICommDlgBrowser)))
				return QueryInterface(iid, ppv);

			return E_NOINTERFACE;
		}
	};
}
