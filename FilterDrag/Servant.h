#pragma once
#include "Layout.h"
#include "KeyboardHook.h"
#include "Aim.h"
#include "Sight.h"

namespace fgo::filter_drag
{
	//
	// このクラスは拡張編集の設定ダイアログにフィルタのドラッグ機能を追加するサーヴァントです。
	//
	inline struct FilterDrag : Servant
	{
		std::unique_ptr<Aim> src, dst;
		std::unique_ptr<Sight> sight;

		ObjectHolder g_srcObject; // ドラッグ元のオブジェクト。
		FilterHolder g_srcFilter; // ドラッグ元のフィルタ。
		FilterHolder g_dstFilter; // ドラッグ先のフィルタ。
		BOOL g_isFilterDragging = FALSE; // ドラッグ中か判定するためのフラグ。

		//
		// コンストラクタです。
		// コンフィグの初期値を設定します。
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
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			getPrivateProfileBool(path, L"Sight", L"enable", sight->config.enable);
			getPrivateProfileInt(path, L"Sight", L"alpha", sight->config.alpha);
			getPrivateProfileColor(path, L"Sight", L"penColor", sight->config.penColor);
			getPrivateProfileReal(path, L"Sight", L"penWidth", sight->config.penWidth);
			getPrivateProfileColor(path, L"Sight", L"brushColor", sight->config.brushColor);
			getPrivateProfileInt(path, L"Sight", L"base", sight->config.base);
			getPrivateProfileInt(path, L"Sight", L"width", sight->config.width);
			getPrivateProfileBSTR(path, L"Sight", L"fontName", sight->config.fontName);
			getPrivateProfileReal(path, L"Sight", L"fontSize", sight->config.fontSize);
			getPrivateProfileReal(path, L"Sight", L"rotate", sight->config.rotate);
			getPrivateProfileInt(path, L"Sight", L"beginMoveX", sight->config.beginMove.X);
			getPrivateProfileInt(path, L"Sight", L"beginMoveY", sight->config.beginMove.Y);

			getPrivateProfileColor(path, L"Config", L"src.color", src->color);
			getPrivateProfileColor(path, L"Config", L"dst.color", dst->color);
			getPrivateProfileBool(path, L"Config", L"useShiftKey", keyboard_hook.useShiftKey);
			getPrivateProfileBool(path, L"Config", L"useCtrlKey", keyboard_hook.useCtrlKey);
			getPrivateProfileBool(path, L"Config", L"useAltKey", keyboard_hook.useAltKey);
			getPrivateProfileBool(path, L"Config", L"useWinKey", keyboard_hook.useWinKey);

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
			func_WndProc.replace(fate.fp->func_WndProc);

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

		struct CursorPos : POINT {
			CursorPos(HWND hwnd) {
				::GetCursorPos(this);
				::ScreenToClient(hwnd, this);
			}
		};

		void beginDrag(const Layout& layout)
		{
			MY_TRACE(_T("beginDrag()\n"));

			// マウスをキャプチャーする。
			::SetCapture(layout.getDialog());

			// フラグを立てる。
			g_isFilterDragging = TRUE;

			// ドラッグ元を表示します。
			src->show(layout, g_srcFilter);

			// サイトを表示します。
			sight->move(layout, g_srcFilter, TRUE);
		}

		void endDrag()
		{
			MY_TRACE(_T("endDrag()\n"));

			// フラグをリセットします。
			g_isFilterDragging = FALSE;

			src->hide();
			dst->hide();
			sight->hide();
		}

		void moveDrag(const Layout& layout)
		{
			MY_TRACE(_T("moveDrag()\n"));

			if (g_dstFilter != g_srcFilter)
			{
				// ドラッグ先を表示します。
				dst->show(layout, g_dstFilter);
			}
			else
			{
				// ドラッグ先の隠します。
				dst->hide();
			}

			// ドラッグ元の表示位置を更新します。
			src->show(layout, g_srcFilter);

			// サイトを動かします。
			sight->move(layout, g_dstFilter, FALSE);
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
			g_srcObject = ObjectHolder(fate.auin.GetCurrentObjectIndex());
			MY_TRACE_OBJECT_HOLDER(g_srcObject);
			if (!g_srcObject.isValid()) return settingDialogProc.orig(hwnd, message, wParam, lParam);

			// マウスカーソルの位置にあるドラッグ元フィルタを取得する。
			g_srcFilter = layout.getSrcFilter(pos, g_srcObject);
			MY_TRACE_FILTER_HOLDER(g_srcFilter);
			g_dstFilter = g_srcFilter;
			if (!g_srcFilter.isValid()) return settingDialogProc.orig(hwnd, message, wParam, lParam);

			MY_TRACE(_T("フィルタのドラッグを開始します\n"));

			// ドラッグを開始する。
			beginDrag(hwnd);

			return settingDialogProc.orig(hwnd, message, wParam, lParam);
		}

		LRESULT onLButtonUp(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			// リセットする前にフィルタをドラッグ中だったかチェックする。
			BOOL isDragging = ::GetCapture() == hwnd && g_isFilterDragging;

			// ドラッグを終了する。
			endDrag();

			// ドラッグ中だったならフィルタの移動を行う。
			if (isDragging)
			{
				::ReleaseCapture(); // ここで WM_CAPTURECHANGED が呼ばれる。

				MY_TRACE_FILTER_HOLDER(g_srcFilter);
				MY_TRACE_FILTER_HOLDER(g_dstFilter);

				// オブジェクトを取得する。
				ObjectHolder object(fate.auin.GetCurrentObjectIndex());
				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.isValid() || object != g_srcObject)
				{
					MY_TRACE(_T("ドラッグ開始時のオブジェクトではないのでフィルタの移動を中止します\n"));
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				// フィルタを取得する。
				FilterHolder filter(g_srcObject, g_srcFilter.getFilterIndex());
				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.isValid() || filter != g_srcFilter)
				{
					MY_TRACE(_T("ドラッグ開始時のフィルタではないのでフィルタの移動を中止します\n"));
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				int srcFilterIndex = g_srcFilter.getFilterIndex();
				MY_TRACE_INT(srcFilterIndex);

				int dstFilterIndex = g_dstFilter.getFilterIndex();
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
			if (::GetCapture() == hwnd && g_isFilterDragging)
			{
				// オブジェクトを取得する。
				ObjectHolder object(fate.auin.GetCurrentObjectIndex());
//				MY_TRACE_OBJECT_HOLDER(object);
				if (!object.isValid() || object != g_srcObject)
				{
					MY_TRACE(_T("ドラッグ開始時のオブジェクトではないのでドラッグを中止します\n"));
					::ReleaseCapture(); endDrag();
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				// フィルタを取得する。
				FilterHolder filter(g_srcObject, g_srcFilter.getFilterIndex());
//				MY_TRACE_FILTER_HOLDER(filter);
				if (!filter.isValid() || filter != g_srcFilter)
				{
					MY_TRACE(_T("ドラッグ開始時のフィルタではないのでドラッグを中止します\n"));
					::ReleaseCapture(); endDrag();
					return settingDialogProc.orig(hwnd, message, wParam, lParam);
				}

				Layout layout(hwnd);

				// マウスカーソルの位置を取得する。
				CursorPos pos(hwnd);
//				MY_TRACE_POINT(pos);

				FilterHolder oldDstFilter = g_dstFilter;

				// マウスカーソルの位置にあるドラッグ元フィルタを取得する。
				g_dstFilter = layout.getDstFilter(pos, g_srcObject);
				if (!g_dstFilter.isValid()) g_dstFilter = g_srcFilter;
//				MY_TRACE_FILTER_HOLDER(g_dstFilter);

				if (g_dstFilter != oldDstFilter)
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
				int srcFilterIndex = servant.g_srcFilter.getFilterIndex();
				MY_TRACE_INT(srcFilterIndex);

				int dstFilterIndex = servant.g_dstFilter.getFilterIndex();
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

		struct Func_WndProc
		{
			static BOOL hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
			{
				switch (message)
				{
				case AviUtl::FilterPlugin::WindowMessage::Init:
					{
						MY_TRACE(_T("func_WndProc(Init, 0x%08X, 0x%08X)\n"), wParam, lParam);

						servant.src = std::make_unique<Aim>();
						servant.dst = std::make_unique<Aim>();
						servant.sight = std::make_unique<Sight>();

						servant.src->color = RGB(0x00, 0x00, 0xff);
						servant.dst->color = RGB(0xff, 0x00, 0x00);
						servant.sight->config.enable = TRUE;
						servant.sight->config.alpha = 192;
						servant.sight->config.penColor = Color(192, 0, 0, 0);
						servant.sight->config.penWidth = 2.0f;
						servant.sight->config.brushColor = Color(255, 255, 255, 255);
						servant.sight->config.base = 16;
						servant.sight->config.width = 8;
						servant.sight->config.fontName = L"Segoe UI";
						servant.sight->config.fontSize = 32.0f;
						servant.sight->config.rotate = 7.77f;
						servant.sight->config.beginMove = Point(0, 100);

						servant.src->create(fate.fp->dll_hinst);
						servant.dst->create(fate.fp->dll_hinst);
						servant.sight->create(fate.fp->dll_hinst);

						break;
					}
				case AviUtl::FilterPlugin::WindowMessage::Exit:
					{
						MY_TRACE(_T("func_WndProc(Exit, 0x%08X, 0x%08X)\n"), wParam, lParam);

						servant.src->destroy();
						servant.dst->destroy();
						servant.sight->destroy();

						servant.src = 0;
						servant.dst = 0;
						servant.sight = 0;

						break;
					}
				}

				return servant.func_WndProc.orig(hwnd, message, wParam, lParam, editp, fp);
			}
		}; Tools::Hook<Func_WndProc> func_WndProc;

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
