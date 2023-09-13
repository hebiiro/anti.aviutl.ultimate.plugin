#pragma once

namespace fgo::filer
{
	//
	// このクラスはファイラウィンドウです。
	// ファイラダイアログを保持します。
	//
	struct FilerWindow : Tools::AviUtl::PluginWindow
	{
		struct Listener {
			virtual BOOL onInitFilerWindow(FilerWindow* filerWindow) = 0;
			virtual BOOL onExitFilerWindow(FilerWindow* filerWindow) = 0;
		};

		//
		// ファイラウィンドウのリスナーです。
		//
		inline static std::weak_ptr<Listener> listener;

		//
		// ファイラウィンドウのコレクションです。
		//
		inline static std::vector<std::shared_ptr<FilerWindow>> collection;

		//
		// コンストラクタです。
		//
		FilerWindow(LPCTSTR name)
		{
			MY_TRACE(_T("FilerWindow::FilerWindow(%s)\n"), name);

			if (!Tools::AviUtl::PluginWindowExtension::create(
				*this, hive.instance, magi.auin.GetAviUtlWindow(), name))
			{
				throw _T("ファイラウィンドウの作成に失敗しました");
			}
		}

		//
		// デストラクタです。
		//
		~FilerWindow()
		{
			MY_TRACE(_T("FilerWindow::~FilerWindow()\n"));
		}

		//
		// 指定された名前のファイラウィンドウを作成して返します。
		//
		static std::shared_ptr<FilerWindow> createFilerWindow(LPCTSTR name)
		{
			MY_TRACE(_T("FilerWindow::create(%s, %d)\n"), name);

			// フィルタウィンドウを作成し、コレクションに追加します。
			auto filerWindow = collection.emplace_back(std::make_shared<FilerWindow>(name));

			// リスナーが存在する場合はファイラウィンドウが作成されたことを通知します。
			if (auto listener = filerWindow->listener.lock())
				listener->onInitFilerWindow(filerWindow.get());

			return filerWindow;
		}

		//
		// カレントオブジェクトに関連するファイルパスを取得します。
		//
		BOOL getItemFileName(LPTSTR buffer, int bufferSize)
		{
			// カレントオブジェクトのインデックスを取得します。
			int objectIndex = magi.auin.GetCurrentObjectIndex();
			if (objectIndex < 0) return FALSE;

			// カレントオブジェクトを取得します。
			ExEdit::Object* object = magi.auin.GetObject(objectIndex);
			if (!object) return FALSE;

			// 先頭のフィルタを取得します。
			int filterIndex = 0;
			ExEdit::Filter* filter = magi.auin.GetFilter(object, filterIndex);
			if (!filter) return FALSE;

			// 拡張データへのオフセットとサイズを取得します。
			int offset = -1, size = -1;
			int id = object->filter_param[filterIndex].id;
			switch (id)
			{
			case 0: offset = 0; size = 260; break; // 動画ファイル
			case 1: offset = 4; size = 256; break; // 画像ファイル
			case 2: offset = 0; size = 260; break; // 音声ファイル
			case 6: offset = 0; size = 260; break; // 音声波形表示
			case 82: offset = 0; size = 260; break; // 動画ファイル合成
			case 83: offset = 4; size = 256; break; // 画像ファイル合成
			}
			if (size <= 0) return FALSE; // サイズを取得できなかった場合は何もしません。

			// 拡張データからファイル名を取得します。
			BYTE* exdata = magi.auin.GetExdata(object, filterIndex);
			LPCSTR fileName = (LPCSTR)(exdata + offset);

			// 整形してバッファに格納します。
			::StringCchPrintf(buffer, bufferSize, _T("%hs"), fileName);
			::PathRemoveFileSpec(buffer);

			return TRUE;
		}

		void updateWindowName(LPCTSTR name)
		{
			::SetWindowText(*this, name);
			::SetWindowText(windowHolderExtension->getTarget(), name);
		}

		void updateWindowName()
		{
			TCHAR name[MAX_PATH] = {};
			::GetWindowText(*this, name, std::size(name));
			::SetWindowText(windowHolderExtension->getTarget(), name);
		}

		//
		// 設定を読み込みます。
		//
		HRESULT load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("FilerWindow::load()\n"));

			getPrivateProfileWindow(element, L"placement", *this);

			return S_OK;
		}

		//
		// 設定を保存します。
		//
		HRESULT save(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("FilerWindow::save()\n"));

			setPrivateProfileWindow(element, L"placement", *this);

			return S_OK;
		}

		//
		// ウィンドウ作成時のスタイルやウィンドウサイズを指定します。
		//
		BOOL create(DWORD exStyle, LPCTSTR className, LPCTSTR windowName, DWORD style,
			int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param) override
		{
			return __super::create(
				exStyle | WS_EX_TOOLWINDOW,
				className,
				windowName,
				style,
				100, 100, 600, 400,
				parent, menu, instance, param);
		}

		//
		// ウィンドウ破壊時の最終処理を行います。
		//
		void postNcDestroy() override
		{
			std::shared_ptr<FilerWindow> keeper;

			// コレクション内のファイラウィンドウを検索します。
			auto it = std::find_if(collection.begin(), collection.end(),
				[this](const auto& x){ return x.get() == this; });
			if (it != collection.end())
			{
				// スマートポインタを取得しておきます。
				// これにより、このスコープ内でthisポインタが削除されないようになります。
				keeper = *it;

				// コレクションから取り除きます。
				collection.erase(it);
			}

			// リスナーが存在する場合はファイラウィンドウが破壊されたことを通知します。
			if (auto listener = this->listener.lock())
				listener->onExitFilerWindow(this);

			// ここでHWNDとの関連付けが解除されます。
			__super::postNcDestroy();
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_DESTROY:
				{
					MY_TRACE(_T("FilerWindow::onWndProc(WM_DESTROY, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// ファイラダイアログをポップアップウィンドウに変更します。
					// これにより、ファイラウィンドウが破壊されたときにファイラダイアログまで破壊されることがなくなります。
					HWND dialog = windowHolderExtension->getTarget();
					::ShowWindow(dialog, SW_HIDE);
					::SetParent(dialog, 0);
					modifyStyle(dialog, WS_CHILD, WS_POPUP | WS_CAPTION | WS_THICKFRAME);

					// ファイラウィンドウの後始末が完了したことをファイラダイアログに通知します。
					// これにより、クライアントプロセス側でファイラダイアログを破壊することができます。
					::PostMessage(dialog, Share::Filer::Message::PostExitFilerWindow, 0, 0);

					break;
				}
			case WM_SETFOCUS:
				{
					MY_TRACE(_T("FilerWindow::onWndProc(WM_SETFOCUS, 0x%08X, 0x%08X)\n"), wParam, lParam);

					hive.activeWindow = hwnd;

					break;
				}
			case Share::Filer::Message::PostInitFilerDialog: // このメッセージはファイラダイアログの作成が完了したときに通知されます。
				{
					MY_TRACE(_T("FilerWindow::onWndProc(PostInitFilerDialog, 0x%08X, 0x%08X)\n"), wParam, lParam);

					HWND dialog = (HWND)wParam;
					HWND browser = (HWND)lParam;

					// ファイラダイアログを子ウィンドウスタイルにしてから子ウィンドウに変更します。
					modifyStyle(dialog, WS_POPUP | WS_CAPTION | WS_THICKFRAME, WS_CHILD);
					::SetParent(dialog, hwnd);
					::SetWindowPos(dialog, 0, 0, 0, 0, 0,
						SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);

					// ファイラダイアログの作成が完了したので、ターゲットに指定し、リサイズします。
					windowHolderExtension->setTarget(dialog);
					windowHolderExtension->resize(hwnd);

					// ターゲットのウィンドウ名を更新します。
					updateWindowName();

					// ファイラダイアログの状態が整ったので表示します。
					::ShowWindow(dialog, SW_SHOW);

					break;
				}
			case Share::Filer::Message::GetFileName: // このメッセージはファイラダイアログで「取得」ボタンが押されたときに通知されます。
				{
					MY_TRACE(_T("FilerWindow::onWndProc(GetFileName, 0x%08X, 0x%08X)\n"), wParam, lParam);

					// カレントオブジェクトに関連するファイル名を取得します。
					TCHAR fileName[MAX_PATH] = {};
					getItemFileName(fileName, std::size(fileName));
					if (_tcslen(fileName))
					{
						// ファイル名を取得できた場合はダイアログに送信します。
						HWND dialog = (HWND)wParam;
						HWND folder = (HWND)lParam;
						::SendMessage(folder, WM_SETTEXT, 0, (LPARAM)fileName);
						::PostMessage(dialog, WM_COMMAND, IDOK, 0);
					}

					break;
				}
			}

			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
