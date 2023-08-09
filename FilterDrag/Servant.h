#pragma once
#include "Layout.h"
#include "KeyboardHook.h"
#include "Aim.h"
#include "Sight.h"

namespace fgo::filter_drag
{
	//
	// このクラスは拡張編集の設定ダイアログにフィルタのドラッグ機能を追加します。
	//
	inline struct FilterDrag : Servant
	{
		std::unique_ptr<Aim> src, dst;
		std::unique_ptr<Sight> sight;

		ObjectHolder m_srcObject; // ドラッグ元のオブジェクト。
		FilterHolder m_srcFilter; // ドラッグ元のフィルタ。
		FilterHolder m_dstFilter; // ドラッグ先のフィルタ。
		BOOL m_isFilterDragging = FALSE; // ドラッグ中か判定するためのフラグ。

		//
		// コンストラクタです。
		//
		FilterDrag()
		{
		}

		//
		// この仮想関数は、このサーヴァントの名前が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"FilterDrag";
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
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
			return fate.getConfigFileName(L"FilterDrag.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグを読み込み、有効の場合は TRUE を返します。
		//
		inline static BOOL isEnabled(LPCWSTR path, LPCWSTR name)
		{
			BOOL enable = FALSE;
			getPrivateProfileBool(path, name, L"enable", enable);
			return enable;
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileBool(path, L"Config", L"useShiftKey", keyboard_hook.useShiftKey);
			getPrivateProfileBool(path, L"Config", L"useCtrlKey", keyboard_hook.useCtrlKey);
			getPrivateProfileBool(path, L"Config", L"useAltKey", keyboard_hook.useAltKey);
			getPrivateProfileBool(path, L"Config", L"useWinKey", keyboard_hook.useWinKey);

			if (isEnabled(path, L"Src"))
			{
				src = std::make_unique<Aim>();
				src->alpha = 96;
				src->color = RGB(0x00, 0x00, 0xff);

				getPrivateProfileInt(path, L"Src", L"alpha", src->alpha);
				getPrivateProfileColor(path, L"Src", L"color", src->color);

				src->create(fate.fp->dll_hinst);
			}
			else
			{
				src = 0;
			}

			if (isEnabled(path, L"Dst"))
			{
				dst = std::make_unique<Aim>();
				dst->alpha = 96;
				dst->color = RGB(0xff, 0x00, 0x00);

				getPrivateProfileInt(path, L"Dst", L"alpha", dst->alpha);
				getPrivateProfileColor(path, L"Dst", L"color", dst->color);

				dst->create(fate.fp->dll_hinst);
			}
			else
			{
				dst = 0;
			}

			if (isEnabled(path, L"Sight"))
			{
				sight = std::make_unique<Sight>();
				sight->config.alpha = 192;
				sight->config.penColor = Color(192, 0, 0, 0);
				sight->config.penWidth = 2.0f;
				sight->config.brushColor = Color(255, 255, 255, 255);
				sight->config.base = 16;
				sight->config.width = 8;
				sight->config.fontName = L"Segoe UI";
				sight->config.fontSize = 32.0f;
				sight->config.rotate = 7.77f;
				sight->config.beginMove = Point(0, 100);

				getPrivateProfileInt(path, L"Sight", L"alpha", sight->config.alpha);
				getPrivateProfileColor(path, L"Sight", L"penColor", sight->config.penColor);
				getPrivateProfileReal(path, L"Sight", L"penWidth", sight->config.penWidth);
				getPrivateProfileColor(path, L"Sight", L"brushColor", sight->config.brushColor);
				getPrivateProfileInt(path, L"Sight", L"base", sight->config.base);
				getPrivateProfileInt(path, L"Sight", L"width", sight->config.width);
				getPrivateProfileBSTR(path, L"Sight", L"fontName", sight->config.fontName);
				getPrivateProfileReal(path, L"Sight", L"fontSize", sight->config.fontSize);
				getPrivateProfileReal(path, L"Sight", L"rotate", sight->config.rotate);
				getPrivateProfileInt(path, L"Sight", L"beginMove.X", sight->config.beginMove.X);
				getPrivateProfileInt(path, L"Sight", L"beginMove.Y", sight->config.beginMove.Y);

				sight->create(fate.fp->dll_hinst);
			}
			else
			{
				sight = 0;
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
			return TRUE;
		}

		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		//
		BOOL init()
		{
			using namespace Tools;

			config_file_checker.init();
			keyboard_hook.init();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			settingDialogProc.orig = fate.auin.HookSettingDialogProc(settingDialogProc.hook);
			swapFilter.attach(fate.auin.GetSwapFilter());

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			keyboard_hook.exit();
			config_file_checker.exit();

			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (!load()) return FALSE;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			save();

			src = 0;
			dst = 0;
			sight = 0;

			return FALSE;
		}

		struct CursorPos : POINT {
			CursorPos(HWND hwnd) {
				::GetCursorPos(this);
				::ScreenToClient(hwnd, this);
			}
		};

		void beginDrag(const Layout& layout)
		{
			MY_TRACE(_T("beginDrag()\n"));

			// マウスをキャプチャーします。
			::SetCapture(layout.getDialog());

			// フラグを立てます。
			m_isFilterDragging = TRUE;

			// ドラッグ元を表示します。
			if (src)
				src->show(layout, m_srcFilter);

			// サイトを表示します。
			if (sight)
				sight->move(layout, m_srcFilter, TRUE);
		}

		void endDrag()
		{
			MY_TRACE(_T("endDrag()\n"));

			// フラグをリセットします。
			m_isFilterDragging = FALSE;

			if (src) src->hide();
			if (dst) dst->hide();
			if (sight) sight->hide();
		}

		void moveDrag(const Layout& layout)
		{
			MY_TRACE(_T("moveDrag()\n"));

			if (dst)
			{
				if (m_dstFilter != m_srcFilter)
				{
					// ドラッグ先を表示します。
					dst->show(layout, m_dstFilter);
				}
				else
				{
					// ドラッグ先の隠します。
					dst->hide();
				}
			}

			// ドラッグ元の表示位置を更新します。
			if (src) src->show(layout, m_srcFilter);

			// サイトを動かします。
			if (sight) sight->move(layout, m_dstFilter, FALSE);
		}

		LRESULT onSetCursor(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			if ((HWND)wParam == hwnd && LOWORD(lParam) == HTCLIENT)
			{
				// ドラッグを開始できるかチェックする。
				if (!keyboard_hook.canBeginDrag())
					return settingDialogProc.orig(hwnd, message, wParam, lParam);

				Layout layout(hwnd);

				// マウスカーソルの位置を取得する。
				CursorPos pos(hwnd);
//				MY_TRACE_POINT(pos);

				// オブジェクトを取得する。
				ObjectHolder object(fate.auin.GetCurrentObjectIndex());
//				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.isValid()) return settingDialogProc.orig(hwnd, message, wParam, lParam);

				// マウスカーソルの位置に移動できるフィルタがあるかチェックする。
				FilterHolder filter = layout.getSrcFilter(pos, object);
//				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.isValid()) return settingDialogProc.orig(hwnd, message, wParam, lParam);

				// マウスカーソルを変更する。
				::SetCursor(::LoadCursor(0, IDC_SIZENS));
				return TRUE;
			}

			return settingDialogProc.orig(hwnd, message, wParam, lParam);
		}

		LRESULT onLButtonDown(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// ドラッグを開始できるかチェックする。
			if (!keyboard_hook.canBeginDrag())
				return settingDialogProc.orig(hwnd, message, wParam, lParam);

			Layout layout(hwnd);

			// マウスカーソルの位置を取得する。
			CursorPos pos(hwnd);
			MY_TRACE_POINT(pos);

			// オブジェクトを取得する。
			m_srcObject = ObjectHolder(fate.auin.GetCurrentObjectIndex());
			MY_TRACE_OBJECT_HOLDER(m_srcObject);
			if (!m_srcObject.isValid()) return settingDialogProc.orig(hwnd, message, wParam, lParam);

			// マウスカーソルの位置にあるドラッグ元フィルタを取得する。
			m_srcFilter = layout.getSrcFilter(pos, m_srcObject);
			MY_TRACE_FILTER_HOLDER(m_srcFilter);
			m_dstFilter = m_srcFilter;
			if (!m_srcFilter.isValid()) return settingDialogProc.orig(hwnd, message, wParam, lParam);

			MY_TRACE(_T("フィルタのドラッグを開始します\n"));

			// ドラッグを開始する。
			beginDrag(hwnd);

			return settingDialogProc.orig(hwnd, message, wParam, lParam);
		}

		LRESULT onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// リセットする前にフィルタをドラッグ中だったかチェックする。
			BOOL isDragging = ::GetCapture() == hwnd && m_isFilterDragging;

			// ドラッグを終了する。
			endDrag();

			// ドラッグ中だったならフィルタの移動を行う。
			if (isDragging)
			{
				::ReleaseCapture(); // ここで WM_CAPTURECHANGED が呼ばれる。

				MY_TRACE_FILTER_HOLDER(m_srcFilter);
				MY_TRACE_FILTER_HOLDER(m_dstFilter);

				// オブジェクトを取得する。
				ObjectHolder object(fate.auin.GetCurrentObjectIndex());
				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.isValid() || object != m_srcObject)
				{
					MY_TRACE(_T("ドラッグ開始時のオブジェクトではないのでフィルタの移動を中止します\n"));
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				// フィルタを取得する。
				FilterHolder filter(m_srcObject, m_srcFilter.getFilterIndex());
				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.isValid() || filter != m_srcFilter)
				{
					MY_TRACE(_T("ドラッグ開始時のフィルタではないのでフィルタの移動を中止します\n"));
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				int srcFilterIndex = m_srcFilter.getFilterIndex();
				MY_TRACE_INT(srcFilterIndex);

				int dstFilterIndex = m_dstFilter.getFilterIndex();
				MY_TRACE_INT(dstFilterIndex);

				// フィルタのインデックスの差分を取得する。
				int sub = dstFilterIndex - srcFilterIndex;
				MY_TRACE_INT(sub);

				if (sub != 0)
				{
					MY_TRACE(_T("ドラッグ先にフィルタを移動します\n"));

					swapFilter.enableHook = TRUE;
					::SendMessage(hwnd, WM_COMMAND, sub < 0 ? 1116 : 1117, 0);
					swapFilter.enableHook = FALSE;
				}
			}

			return settingDialogProc.orig(hwnd, message, wParam, lParam);
		}

		LRESULT onMouseMove(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// フィルタをドラッグ中かチェックする。
			if (::GetCapture() == hwnd && m_isFilterDragging)
			{
				// オブジェクトを取得する。
				ObjectHolder object(fate.auin.GetCurrentObjectIndex());
//				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.isValid() || object != m_srcObject)
				{
					MY_TRACE(_T("ドラッグ開始時のオブジェクトではないのでドラッグを中止します\n"));
					::ReleaseCapture(); endDrag();
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				// フィルタを取得する。
				FilterHolder filter(m_srcObject, m_srcFilter.getFilterIndex());
//				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.isValid() || filter != m_srcFilter)
				{
					MY_TRACE(_T("ドラッグ開始時のフィルタではないのでドラッグを中止します\n"));
					::ReleaseCapture(); endDrag();
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				Layout layout(hwnd);

				// マウスカーソルの位置を取得する。
				CursorPos pos(hwnd);
//				MY_TRACE_POINT(pos);

				FilterHolder oldDstFilter = m_dstFilter;

				// マウスカーソルの位置にあるドラッグ元フィルタを取得する。
				m_dstFilter = layout.getDstFilter(pos, m_srcObject);
				if (!m_dstFilter.isValid()) m_dstFilter = m_srcFilter;
//				MY_TRACE_FILTER_HOLDER(m_dstFilter);

				if (m_dstFilter != oldDstFilter)
				{
					// マークを動かす。
					moveDrag(layout);
				}
			}

			return settingDialogProc.orig(hwnd, message, wParam, lParam);
		}

		LRESULT onCaptureChanged(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			endDrag();

			return settingDialogProc.orig(hwnd, message, wParam, lParam);
		}

		struct SettingDialogProc {
			//
			// 拡張編集内の設定ダイアログのウィンドウプロシージャと置き換えられます。
			//
			static LRESULT WINAPI hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
//				MY_TRACE(_T("0x%08X, 0x%08X, 0x%08X, 0x%08X\n"), hwnd, message, wParam, lParam);

				switch (message)
				{
				case WM_SETCURSOR: return servant.onSetCursor(hwnd, message, wParam, lParam);
				case WM_LBUTTONDOWN: return servant.onLButtonDown(hwnd, message, wParam, lParam);
				case WM_LBUTTONUP: return servant.onLButtonUp(hwnd, message, wParam, lParam);
				case WM_MOUSEMOVE: return servant.onMouseMove(hwnd, message, wParam, lParam);
				case WM_CAPTURECHANGED: return servant.onCaptureChanged(hwnd, message, wParam, lParam);
				}

				return servant.settingDialogProc.orig(hwnd, message, wParam, lParam);
			}
		}; Tools::Hook<SettingDialogProc> settingDialogProc;

		struct SwapFilter
		{
			BOOL enableHook = FALSE;

			void moveFilter(int objectIndex)
			{
				int srcFilterIndex = servant.m_srcFilter.getFilterIndex();
				MY_TRACE_INT(srcFilterIndex);

				int dstFilterIndex = servant.m_dstFilter.getFilterIndex();
				MY_TRACE_INT(dstFilterIndex);

				// フィルタのインデックスの差分を取得する。
				int sub = dstFilterIndex - srcFilterIndex;
				MY_TRACE_INT(sub);

				// フィルタを移動する。
				if (sub < 0)
				{
					// 上に移動
					for (int i = sub; i < 0; i++)
						servant.swapFilter.orig(objectIndex, srcFilterIndex--, -1);
				}
				else
				{
					// 下に移動
					for (int i = sub; i > 0; i--)
						servant.swapFilter.orig(objectIndex, srcFilterIndex++, 1);
				}
			}

			static void CDECL hook(int objectIndex, int filterIndex, int relativeIndex)
			{
				if (servant.swapFilter.enableHook)
					servant.swapFilter.moveFilter(objectIndex);
				else
					servant.swapFilter.orig(objectIndex, filterIndex, relativeIndex);
			}
		}; Tools::Hook<SwapFilter> swapFilter;

		//
		// このクラスはコンフィグファイルの更新を監視します。
		// これにより、ユーザーがコンフィグファイルを直接編集した場合でも、
		// 変更が即時に反映されるようになります。
		//
		struct ConfigFileChecker : FileUpdateChecker
		{
			BOOL init()
			{
				MY_TRACE(_T("ConfigFileChecker::init()\n"));

				// ファイルアップデートチェッカー(スーパークラス)を初期化します。
				__super::init(getConfigFileName().c_str());

				// コンフィグファイルを監視するタイマーを作成します。
				::SetTimer(0, 0, 1000, timerProc);

				return TRUE;
			}

			BOOL exit()
			{
				MY_TRACE(_T("ConfigFileChecker::exit()\n"));

				return TRUE;
			}

			static void CALLBACK timerProc(HWND hwnd, UINT message, UINT_PTR timerId, DWORD time)
			{
				if (servant.config_file_checker.isFileUpdated())
				{
					MY_TRACE(_T("コンフィグファイルが更新されました\n"));

					servant.load(); // コンフィグファイルを再ロードします。
				}
			}
		} config_file_checker;
	} servant;
}
