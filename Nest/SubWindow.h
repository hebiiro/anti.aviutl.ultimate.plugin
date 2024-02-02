#pragma once
#include "Shuttle/Shuttle.h"
#include "DockSite.h"
#include "SubWindowManager.h"

namespace fgo::nest
{
	//
	// このクラスはサブウィンドウです。
	// このウィンドウはドッキング機能を有しながら、
	// このウィンドウ自体を他のドックサイトにドッキングさせることもできます。
	//
	struct SubWindow : DockSite<Shuttle>
	{
		//
		// 使用されていないシャトル名を返します。
		//
		static void getEmptyName(LPTSTR name, int cch)
		{
			for (int i = 0; TRUE; i++)
			{
				::StringCchPrintf(name, cch, _T("サブ%d"), i + 1);
				MY_TRACE_TSTR(name);

				if (!shuttleManager.get(name))
					break;
			}
		}

		//
		// ウィンドウを作成します。
		//
		BOOL create(LPCTSTR name, HWND parent)
		{
			MY_TRACE_FUNC("%s, 0x%08X", name, parent);

			WNDCLASS wc = {};
			wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
			wc.hCursor = ::LoadCursor(0, IDC_ARROW);
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.lpszClassName = hive.SubWindowClassName;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				hive.SubWindowClassName,
				name,
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, hive.instance, 0);
		}

		//
		// Shuttle の init() を呼び出して，システムメニューに項目を追加します．
		//
		void init(const _bstr_t& name, HWND hwnd) {
			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			HICON icon = (HICON)::GetClassLong(hive.aviutlWindow, GCL_HICON);
			::SendMessage(*floatContainer, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*floatContainer, WM_SETICON, ICON_BIG, (LPARAM)icon);

			// フローティングコンテナのシステムメニューにメニュー項目とセパレータを追加します．
			HMENU menu = ::GetSystemMenu(*floatContainer, FALSE);
			::InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, CommandID::RENAME_SUB_WINDOW, _T("名前を変更"));
			::InsertMenu(menu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, nullptr);
		}

		//
		// SubWindow かどうかを判別する必要がある場面が多いため，識別子を設定しておきます．
		//
		LPCTSTR getWindowId() override { return hive.SubWindowClassName; }

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("0x%08X, WM_CREATE, 0x%08X, 0x%08X", hwnd,  wParam, lParam);

					// このサブウィンドウをマネージャーに追加します。
					// これにより、現存するすべてのサブウィンドウにアクセスできるようになります。
					subWindowManager.add(hwnd);

					break;
				}
			case WM_KEYDOWN:
			case WM_KEYUP:
			case WM_CHAR:
			case WM_DEADCHAR:
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_SYSCHAR:
			case WM_SYSDEADCHAR:
				{
					// Nest の MainWindow にフォーカスがある状態から
					// フロート状態の SubWindow のタイトルをクリックしてフォーカスを移動すると，
					// ショートカットキーが利かなくなっていた問題に対処．
					// メッセージをAviUtlのメインウィンドウに転送します．
					return ::SendMessage(hive.aviutlWindow, message, wParam, lParam);
				}
			case WM_SYSCOMMAND:
				{
					// SubWindow を保持する Container クラスから送られてきます．
					switch (wParam) {
					case CommandID::RENAME_SUB_WINDOW:
						{
							auto container = getCurrentContainer();
							if (!container) break;

							// サブウィンドウの名前変更ダイアログを表示します．
							shuttleManager.showRenameDialog(this, *container);
							break;
						}
					}
					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("0x%08X, WM_CLOSE, 0x%08X, 0x%08X", hwnd,  wParam, lParam);

					if (::GetKeyState(VK_SHIFT) < 0)
					{
						// Shiftキーが押されている場合はサブウィンドウを削除します。

						// このサブウィンドウを削除していいのかどうかをユーザーに問い合わせます。
						if (IDYES != ::MessageBox(hwnd, _T("サブウィンドウを削除しますか？"), hive.AppName, MB_YESNO))
							return 0;

						// このサブウィンドウをマネージャーから削除します。
						// これにより、このウィンドウは破壊されます。
						subWindowManager.erase(hwnd);
					}
					else
					{
						// サブウィンドウの表示状態をトグルで切り替えます。

						if (::IsWindowVisible(hwnd))
						{
							::ShowWindow(hwnd, SW_HIDE);
						}
						else
						{
							::ShowWindow(hwnd, SW_SHOW);
						}
					}

					if (!::IsWindowVisible(hwnd))
					{
						// 何もしないとメインウィンドウが他のウィンドウの後ろに隠れてしまうので、
						// 手動でフォアグラウンドにします。
						::SetActiveWindow(hive.mainWindow);
					}

					return 0;
				}
			case WM_DESTROY:
				{
					// タイトルテキストの同期を終了します．
					ResetTitleSource();
					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		// タイトルテキスト同期の仲介オブジェクトです．
		struct SyncTitle : Prelistener {
			SubWindow* owner; // 同期先の SubWindow へのポインタです．
			std::weak_ptr<Shuttle> source; // 同期元の Shuttle への weak pointer です．
			SyncTitle(SubWindow* owner, const std::shared_ptr<Shuttle>& source)
				: owner{ owner }, source{ source } {}

			//
			// ターゲットのテキストが変更されたとき、シャトルから通知されます。
			//
			void onChangeText(Shuttle* shuttle, LPCTSTR newText) override
			{
				// タイトルテキストを同期します．
				::SetWindowText(*owner, newText);
			}
		};
		std::shared_ptr<SyncTitle> title_sync_token = {};

		// タイトルテキストの同期を仕込みます．
		void SetTitleSource(const std::shared_ptr<Shuttle>& source)
		{
			if (!source) {
				// 同期を解除してタイトルを戻します．
				ResetTitleSource();
				::SetWindowText(hwnd, name);
				return;
			}

			// 循環参照を避けるために SubWindow からの同期を回避します．
			if (::StrCmpI(source->getWindowId(), hive.SubWindowClassName) == 0) return;

			// 同期の仲介オブジェクトを作成，同期先に仕込みます．
			title_sync_token.reset(new SyncTitle{ this, source });
			source->addTitleSyncToken(title_sync_token);

			// 最初の同期をしておきます．
			TCHAR title[MAX_PATH] = {};
			::GetWindowText(source->hwnd, title, std::size(title));
			::SetWindowText(hwnd, title);
		}
		// タイトルテキストの同期を解除します．
		void ResetTitleSource() { title_sync_token.reset(); }
		// タイトルテキストの同期元を取得します．
		std::shared_ptr<Shuttle> GetTitleSource()
		{
			return !title_sync_token ? nullptr : title_sync_token->source.lock();
		}
	};

	// 定義順の関係で ShuttleManager の一部関数の実装をここに記述．
	// SubWindow を与えてタイトルテキストの同期元を取得する関数です．
	inline std::shared_ptr<Shuttle> ShuttleManager::RenameDialog::GetSubWindowTitleSource(Shuttle* subwindow) {
		// Shuttle* subwindow は必ず SubWindow クラスへのポインタが渡ってきます．
		return dynamic_cast<SubWindow*>(subwindow)->GetTitleSource();
	}
	// SubWindow と Shuttle を与えてタイトルテキストの同期を設定する関数です．
	inline void ShuttleManager::RenameDialog::SetSubWindowTitleSource(Shuttle* subwindow, const std::shared_ptr<Shuttle>& source) {
		// Shuttle* subwindow は必ず SubWindow クラスへのポインタが渡ってきます．
		dynamic_cast<SubWindow*>(subwindow)->SetTitleSource(source);
	}
}
