#pragma once
#include "Shuttle/Shuttle.h"
#include "DockSite.h"

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
		// このクラスはタイマーのIDです。
		//
		struct TimerID {
			inline static constexpr UINT_PTR UpdateInterseptors = 1000;
			inline static constexpr UINT_PTR UpdateWindowText = 1001;
		};

		//
		// このクラスのインスタンスのコレクションです。
		//
		inline static std::set<SubWindow*> collection;

		//
		// コンストラクタです。
		//
		SubWindow()
		{
			MY_TRACE_FUNC("");

			// インスタンスをコレクションに追加します。
			collection.emplace(this);
		}

		//
		// デストラクタです。
		//
		~SubWindow() override
		{
			MY_TRACE_FUNC("");

			// インスタンスをコレクションから削除します。
			collection.erase(this);
		}

		//
		// すべてのサブウィンドウを削除します。
		//
		static void clear()
		{
			// サブウィンドウを削除する度にコレクションの個数が変わってしまうので、まずコピーを作ります。
			auto copy = collection;
			for (auto subWindow : copy)
			{
				// 無駄な描画を減らすためにウィンドウを非表示にします。
				::ShowWindow(*subWindow, SW_HIDE);

				// サブウィンドウを削除します。
				subWindow->destroy();
			}

			// 念のためコレクションを空にします。
			collection.clear();
		}

		//
		// 使用可能なシャトル名を返します。
		//
		static void getAvailableName(LPTSTR name, size_t cch)
		{
			for (size_t i = 0; TRUE; i++)
			{
				::StringCchPrintf(name, cch, _T("サブ%d"), i + 1);
				MY_TRACE_TSTR(name);

				if (!shuttleManager.get(name))
					break;
			}
		}

		//
		// 指定されたウィンドウがサブウィンドウの場合はTRUEを返します。
		//
		static BOOL isSubWindow(HWND hwnd)
		{
			return isDockSite(hwnd);
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
			wc.lpszClassName = Hive::SubWindow::ClassName;
			::RegisterClass(&wc);

			return __super::create(
				WS_EX_NOPARENTNOTIFY,
				Hive::SubWindow::ClassName,
				name,
				WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX | WS_THICKFRAME |
				WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				parent, 0, hive.instance, 0);
		}

		//
		// ターゲットにアタッチします。
		//
		void init(const _bstr_t& name, HWND hwnd)
		{
			MY_TRACE_FUNC("");

			__super::init(name, hwnd);

			// フローティングコンテナのアイコンを設定します。
			HICON icon = (HICON)::GetClassLong(hive.aviutlWindow, GCL_HICON);
			::SendMessage(*floatContainer, WM_SETICON, ICON_SMALL, (LPARAM)icon);
			::SendMessage(*floatContainer, WM_SETICON, ICON_BIG, (LPARAM)icon);

			// フローティングコンテナのシステムメニューに項目を追加します。
			HMENU menu = ::GetSystemMenu(*floatContainer, FALSE);
			::InsertMenu(menu, 0, MF_BYPOSITION | MF_STRING, CommandID::RenameSubWindow, _T("名前を変更"));
			::InsertMenu(menu, 1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

			// ウィンドウ名の更新を促します。
			// ただし、この時点では他のシャトルの作成が完了していないので、
			// ここではメッセージを送信するだけにして、それをあとからハンドルします。
			fireUpdateInterseptors();
		}

		//
		// シャトルの名前が変更されたときに呼ばれます。
		//
		void onRename() override
		{
			MY_TRACE_FUNC("");

			// シャトルの名前が変更されたので
			// インターセプターを再設置します。
			updateInterseptors();

			// ウィンドウ名を更新します。
			updateWindowText();
		}

		//
		// インターセプターを再設置します。
		//
		void updateInterseptors()
		{
			MY_TRACE_FUNC("");

			// 一旦すべてのインターセプターを削除します。
			interseptors.clear();

			// このサブウィンドウのシャトル名を取得します。
			std::wstring text = (LPCWSTR)name;

			// シャトル名に入っている他のシャトルの名前を検索します。
			std::wregex regex(L"%(.+?)%");
			std::wsmatch results;
			auto start = text.cbegin();
			while (std::regex_search(start, text.cend(), results, regex))
			{
				// 検索結果である他のシャトルの名前を取得します。
				std::wstring result = results[1].str();

				// 装飾されたシャトル名を取得します。
				std::wstring name = Shuttle::decorateName(result);

				// 他のシャトルが見つかった場合は
				auto shuttle = shuttleManager.get(name.c_str());
				if (shuttle && !isSubWindow(*shuttle)) // 循環参照になる可能性があるのでサブウィンドウは除外します。
				{
					// インターセプターを作成してメッセージを傍受します。
					interseptors.emplace_back(std::make_shared<Interseptor>(this, shuttle, result));
				}

				// 次の検索位置に進めます。
				start = results[0].second;
			}
		}

		//
		// text内のsrcをdstに置換します。
		// https://learningprog.com/cpp-replace/
		//
		static void replace(std::wstring& text, const std::wstring& src, const std::wstring& dst)
		{
			size_t pos = text.find(src);  // 置換したい文字列が登場する位置を検索する
			size_t len = src.length();    // 置換したい文字列の長さ
			if (pos != text.npos)
				text.replace(pos, len, dst);  // 置換
		}

		//
		// ウィンドウ名を返します。
		// https://learn.microsoft.com/ja-jp/archive/msdn-magazine/2015/july/c-using-stl-strings-at-win32-api-boundaries
		//
		static std::wstring getWindowText(HWND hwnd)
		{
			// Get the length of the text string
			// (Note: +1 to consider the terminating NUL)
			const int bufferLength = ::GetWindowTextLengthW(hwnd) + 1;

			// Allocate string of proper size
			std::wstring text;
			text.resize(bufferLength);

			// Get the text of the specified control
			// Note that the address of the internal string buffer
			// can be obtained with the &text[0] syntax
			::GetWindowTextW(hwnd, &text[0], bufferLength);

			// Resize down the string to avoid bogus double-NUL-terminated strings
			text.resize(bufferLength - 1);

			return text;
		}

		//
		// ウィンドウ名を更新します。
		// 自分または他のシャトルの名前が変更されたときに呼び出してください。
		//
		void updateWindowText()
		{
			MY_TRACE_FUNC("");

			// このサブウィンドウのシャトル名を取得します。
			std::wstring text = (LPCWSTR)name;

			for (auto interseptor : interseptors)
			{
				// textの中にある他のシャトルの名前を含んだパターンを
				// そのシャトルのウィンドウ名に置換します。

				std::wstring src = interseptor->pattern;
				std::wstring dst = getWindowText(*interseptor->shuttle);

				replace(text, src, dst);
			}

			// ウィンドウ名を変更します。
			::SetWindowTextW(*this, text.c_str());
		}

		//
		// インターセプター更新メッセージを送信します。
		//
		void fireUpdateInterseptors()
		{
			::SetTimer(*this, TimerID::UpdateInterseptors, 0, 0);
		}

		//
		// ウィンドウ名更新メッセージを送信します。
		//
		void fireUpdateWindowText()
		{
			::SetTimer(*this, TimerID::UpdateWindowText, 0, 0);
		}

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

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("0x%08X, WM_CLOSE, 0x%08X, 0x%08X", hwnd,  wParam, lParam);

					if (::GetKeyState(VK_SHIFT) < 0)
					{
						// Shiftキーが押されている場合はサブウィンドウを削除します。

						// このサブウィンドウを削除していいのかどうかをユーザーに問い合わせます。
						if (IDYES != ::MessageBox(hwnd, _T("サブウィンドウを削除しますか？"), hive.DisplayName, MB_YESNO))
							return 0;

						// ウィンドウを破壊します。
						// インスタンスもここで削除されます。
						destroy();
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
			case WM_SYSCOMMAND: // フローティングコンテナのシステムメニューコマンドをハンドルします。
				{
					switch (wParam)
					{
					case CommandID::RenameSubWindow:
						{
							// サブウィンドウの名前変更ダイアログを表示します。
							shuttleManager.showRenameDialog(this, hwnd);

							break;
						}
					}

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE_FUNC("WM_SETFOCUS, 0x%08X, 0x%08X", wParam, lParam);

					break;
				}
			case WM_KILLFOCUS:
				{
					MY_TRACE_FUNC("WM_KILLFOCUS, 0x%08X, 0x%08X", wParam, lParam);

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
					// サブウィンドウにフォーカスがあっても
					// ショートカットキーが使用できるように
					// メッセージをAviUtlウィンドウに転送します。
					return ::SendMessage(hive.aviutlWindow, message, wParam, lParam);
				}
			case WM_TIMER:
				{
					switch (wParam)
					{
					case TimerID::UpdateInterseptors:
						{
							// インターセプターを更新します。
							updateInterseptors();

							// インターセプターが更新されたので
							// それに合わせてウィンドウ名を更新します。
							updateWindowText();

							// タイマーを終了します。
							::KillTimer(hwnd, wParam);

							break;
						}
					case TimerID::UpdateWindowText:
						{
							// 他のシャトルの名前が変更されたので
							// それに合わせてウィンドウ名を更新します。
							updateWindowText();

							// タイマーを終了します。
							::KillTimer(hwnd, wParam);

							break;
						}
					}

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}

		//
		// このクラスは他のシャトルのメッセージを傍受します。
		//
		struct Interseptor : Tools::WindowInterseptor
		{
			//
			// メッセージを受信するサブウィンドウです。
			//
			SubWindow* subWindow;

			//
			// メッセージを傍受されるシャトルです。
			//
			std::shared_ptr<Shuttle> shuttle;

			//
			// 文字列からシャトルを識別するためのパターンです。
			//
			std::wstring pattern;

			//
			// コンストラクタです。
			//
			Interseptor(SubWindow* subWindow, const std::shared_ptr<Shuttle>& shuttle, const std::wstring& name)
				: subWindow(subWindow)
				, shuttle(shuttle)
				, pattern(L"%" + name + L"%")
			{
				subclass(*shuttle);
			}

			//
			// デストラクタです。
			//
			~Interseptor() override
			{
				unsubclass();
			}

			//
			// ウィンドウプロシージャです。
			//
			LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
			{
				switch (message)
				{
				case WM_SETTEXT:
					{
						MY_TRACE_FUNC("0x%08X, WM_SETTEXT, 0x%08X, 0x%08X", hwnd,  wParam, lParam);

						// サブウィンドウにタイマーメッセージを送信します。
						subWindow->fireUpdateWindowText();

						break;
					}
				}

				return __super::onWndProc(hwnd, message, wParam, lParam);
			}
		};

		//
		// インターセプターのコレクションです。
		//
		std::vector<std::shared_ptr<Interseptor>> interseptors;
	};
}
