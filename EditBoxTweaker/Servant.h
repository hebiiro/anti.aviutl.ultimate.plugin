#pragma once

namespace fgo::editbox_tweaker
{
	//
	// このクラスはエディットボックスを微調整します。
	//
	inline struct EditBoxTweaker : Servant
	{
		BOOL unicodeInput;
		BOOL ctrlA;
		BOOL zoomable;
		int delta;
		struct Font {
			_bstr_t name;
			int height;
			int pitch;
			HFONT handle;
		} font;

		//
		// コンストラクタです。
		// コンフィグの初期値を設定します。
		//
		EditBoxTweaker()
		{
			unicodeInput = TRUE;
			ctrlA = TRUE;
			zoomable = TRUE;
			delta = 0;
			font.name = L"";
			font.height = 0;
			font.pitch = 0;
			font.handle = 0;
		}

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"EditBoxTweaker";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"エディットボックス微調整";
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"OptimizeEditBox.auf"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"EditBoxTweakerとOptimizeEditBox.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"EditBoxTweaker.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileInt(path, L"Config", L"unicodeInput", unicodeInput);
			getPrivateProfileInt(path, L"Config", L"ctrlA", ctrlA);
			getPrivateProfileInt(path, L"Config", L"zoomable", zoomable);
			getPrivateProfileInt(path, L"Config", L"delta", delta);
			getPrivateProfileBSTR(path, L"Config", L"font.name", font.name);
			getPrivateProfileInt(path, L"Config", L"font.height", font.height);
			getPrivateProfileInt(path, L"Config", L"font.pitch", font.pitch);

			if (wcslen(font.name) != 0)
			{
				// DPIに合わせてフォントのサイズを調整します。
				int dpi = ::GetSystemDpiForProcess(::GetCurrentProcess());
				int height = ::MulDiv(font.height, dpi, 96);

				// HFONTを作成します。
				// 複数行エディットボックスのWM_SETFONTでこのハンドルが渡されます。
				font.handle = ::CreateFontW(height, 0,
					0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
					0, 0, 0, font.pitch, font.name);
			}

			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			setPrivateProfileInt(path, L"Config", L"unicodeInput", unicodeInput);
			setPrivateProfileInt(path, L"Config", L"ctrlA", ctrlA);
			setPrivateProfileInt(path, L"Config", L"zoomable", zoomable);
			setPrivateProfileInt(path, L"Config", L"delta", delta);
			setPrivateProfileBSTR(path, L"Config", L"font.name", font.name);
			setPrivateProfileInt(path, L"Config", L"font.height", font.height);
			setPrivateProfileInt(path, L"Config", L"font.pitch", font.pitch);

			return TRUE;
		}

		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		// 拡張編集のコンスト値を書き換えます。
		//
		BOOL init()
		{
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.auin.GetExEdit();

			Tools::attach(GetMessageA);

			if (unicodeInput) {
				Tools::attach(DispatchMessageA);
				Tools::attach(PeekMessageA);
			}

			// 拡張編集内の::CreateWindowExW()の呼び出しをフックします。
			// コンスト値も書き換えます。
			if (delta != 0 || font.handle != nullptr) {
				Tools::attach_abs_call(editbox, exedit + 0x0008C46E);
				Tools::attach_abs_call(editbox, exedit + 0x00087658);
				Tools::add_int32(exedit + 0x0008CC56 + 1, delta);
				Tools::add_int32(exedit + 0x000876DE + 1, delta);
			}

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			if (font.handle)
				::DeleteObject(font.handle), font.handle = 0;

			return TRUE;
		}

		//
		// このクラスは::GetMesageA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook(LPMSG msg, HWND hwnd, UINT msgFilterMin, UINT msgFilterMax)
			{
				BOOL result = FALSE;
				if (servant.unicodeInput) {
					// ::GetMessageA()の代わりに::GetMessageW()を呼び出します。
					result = ::GetMessageW(msg, hwnd, msgFilterMin, msgFilterMax);
				} else {
					result = orig(msg, hwnd, msgFilterMin, msgFilterMax);
				}

				// Ctrl + A.
				if (servant.ctrlA &&
					msg->message == WM_KEYDOWN &&
					msg->wParam == 'A' &&
					::GetKeyState(VK_CONTROL) < 0 &&
					check_window_classname(msg->hwnd, WC_EDITW)) {

					// テキスト全選択してこのメッセージはディスパッチしない．次のメッセージまで待機．
					::SendMessageW(msg->hwnd, EM_SETSEL, 0, -1);
					return hook(msg, hwnd, msgFilterMin, msgFilterMax);
				}

				return result;
			}
			inline static decltype(&hook) orig = ::GetMessageA;
		} GetMessageA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		struct {
			inline static LRESULT CALLBACK subclassproc(
				HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR)
			{
				switch (msg) {
				case WM_SETFONT:
					MY_TRACE(_T("WM_SETFONT, 0x%08X, 0x%08X\n"), wparam, lparam);
					wparam = reinterpret_cast<WPARAM>(servant.font.handle); // handle is not null here.
				//[[fallthrough]];
				case WM_DESTROY:
					::RemoveWindowSubclass(hwnd, subclassproc, id);
					break;
				}

				return ::DefSubclassProc(hwnd, msg, wparam, lparam);
			}

			//
			// 拡張編集内の複数行エディットボックスを作成する関数(::CreateWindowExW())と置き換えられます。
			//
			inline static HWND WINAPI hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				MY_TRACE(_T("CreateWindowExW(%ws, %d, %d)\n"), className, w, h);

				HWND hwnd = orig(exStyle, className, windowName,
					style, x, y, w, h + servant.delta, parent, menu, instance, param);

				if (servant.zoomable)
					::SendMessage(hwnd, EM_SETEXTENDEDSTYLE, ES_EX_ZOOMABLE, ES_EX_ZOOMABLE);

				if (servant.font.handle != nullptr)
					::SetWindowSubclass(hwnd, subclassproc, 0, 0);

				return hwnd;
			}
			inline static decltype(&hook) orig = ::CreateWindowExW;
		} editbox;

		//
		// この構造体は::DispatchMesageA()を::DispatchMesageW()に置き換えるために使用されます。
		//
		struct {
			inline static decltype(&::DispatchMessageA) hook = ::DispatchMessageW;
			inline static decltype(hook) orig = ::DispatchMessageA;
		} DispatchMessageA;

		//
		// この構造体は::PeekMesageA()を::PeekMesageW()に置き換えるために使用されます。
		//
		struct {
			inline static decltype(&::PeekMessageA) hook = ::PeekMessageW;
			inline static decltype(hook) orig = ::PeekMessageA;
		} PeekMessageA;
	} servant;
}
