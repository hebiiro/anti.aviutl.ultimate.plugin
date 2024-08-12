#pragma once

namespace apn::local_web_app
{
	//
	// このクラスはメインウィンドウです。
	//
	inline struct MainWindow : Browser
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			WNDCLASSEXW wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.hIcon = ::LoadIcon(hive.instance, MAKEINTRESOURCE(IDI_64x64));
			wc.hIconSm = ::LoadIcon(hive.instance, MAKEINTRESOURCE(IDI_64x64));
			wc.lpszClassName = _T("local_web_app.main_window");
			::RegisterClassExW(&wc);

			if (!__super::create(
				0,
				wc.lpszClassName,
				hive.c_display_name,
				WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
				nullptr, nullptr, hive.instance, nullptr))
			{
				hive.message_box(L"ブラウザウィンドウの作成に失敗しました");

				return FALSE;
			}

			return __super::init();
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			__super::exit();

			return destroy();
		}

		//
		// 相対パスを絶対パスに変換して返します。
		//
		std::filesystem::path get_abs_path(const std::filesystem::path& rel_path)
		{
			// カレントパスと相対パスを結合します。
			auto path = hive.current_file_path.parent_path() / rel_path;
#if 1
			// パスを正規化して返します。
			return path.lexically_normal();
#else
			// パスをそのまま返します。
			return path;
#endif
		}

		//
		// この仮想関数は、初期化が完了したあとに呼ばれます。
		//
		virtual BOOL on_post_init() override
		{
			MY_TRACE_FUNC("");
#if 0
			// ドラッグアンドドロップを無効化します。
			auto hr = webview->AddScriptToExecuteOnDocumentCreated(
				L"window.addEventListener('dragover',function(e){e.preventDefault();},false);"
				L"window.addEventListener('drop',function(e){"
					L"e.preventDefault();"
					L"console.log(e.dataTransfer);"
					L"console.log(e.dataTransfer.files[0])"
				L"}, false);", nullptr);
#endif
			if (!hive.current_file_path.empty())
				navigate(hive.current_file_path);

			return TRUE;
		}

		//
		// この仮想関数は、コンテンツからイベントが発行されたときに呼ばれます。
		//
		virtual BOOL on_content_event(const std::wstring& json) override
		{
			MY_TRACE_FUNC("{}", json);

			try
			{
				auto root = n_json::parse(my::wide_to_cp(json, CP_UTF8));

				for (const auto& node : root.items())
				{
					const auto& key = node.key();

					if (key == "request_write_json")
					{
						const auto& event_node = node.value();

						// 書き込み先のファイルパスを取得します。
						// このパスは相対パスでなければなりません。
						std::filesystem::path path = event_node["path"];
						MY_TRACE_STR(path);
						if (path.empty()) continue;

						// 絶対パスを取得します。
						auto abs_path = get_abs_path(path);
						MY_TRACE_STR(abs_path);

						// 書き込むデータ(json)を取得します。
						const auto& data_node = event_node["data"];

						// 指定されたデータを指定されたファイルに書き込みます。
						std::ofstream ofs(abs_path);
						ofs << std::setw(4) << data_node;

						n_json root;
						{
							n_json event_node;
							{
								event_node["path"] = my::wide_to_cp(path, CP_UTF8);
								event_node["abs_path"] = my::wide_to_cp(abs_path, CP_UTF8);
							}
							root["response_write_json"] = event_node;
						}

						// jsonを文字列化します。
						auto json = my::cp_to_wide(root.dump(4), CP_UTF8);

						// 文字列化したjsonをブラウザに送信します。
						post_json(json);
					}
					else if (key == "request_read_json")
					{
						const auto& event_node = node.value();

						try
						{
							// 読み込み先のファイルパスを取得します。
							// このパスは相対パスでなければなりません。
							std::filesystem::path path = event_node["path"];
							MY_TRACE_STR(path);
							if (path.empty()) continue;

							// 絶対パスを取得します。
							auto abs_path = get_abs_path(path);
							MY_TRACE_STR(abs_path);

							// 指定されたファイルを読み込みます。
							std::ifstream ifs(abs_path);
							auto data = n_json::parse(ifs);

							n_json root;
							{
								n_json event_node;
								{
									event_node["path"] = my::wide_to_cp(path, CP_UTF8);
									event_node["abs_path"] = my::wide_to_cp(abs_path, CP_UTF8);
									event_node["data"] = data;
								}
								root["response_read_json"] = event_node;
							}

							// jsonを文字列化します。
							auto json = my::cp_to_wide(root.dump(4), CP_UTF8);

							// 文字列化したjsonをブラウザに送信します。
							post_json(json);
						}
						catch (const std::exception& error)
						{
							(void)error;
						}
					}
				}
			}
			catch (const std::exception& error)
			{
				hive.message_box(my::ws(error.what()), *this);
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_CREATE:
				{
					MY_TRACE_FUNC("WM_CREATE, {:#010x}, {:#010x}", wParam, lParam);

					::DragAcceptFiles(hwnd, TRUE);

					// このウィンドウをメインウィンドウに設定します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {:#010x}, {:#010x}", wParam, lParam);

					// メッセージループを終了します。
					::PostQuitMessage(0);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {:#010x}, {:#010x}", wParam, lParam);

					// コンフィグを書き込みます。
					config_io.write();

					break;
				}
			case WM_DROPFILES:
				{
					MY_TRACE_FUNC("WM_DROP_FILES, {:#010x}, {:#010x}", wParam, lParam);

					auto drop = (HDROP)wParam;
					auto c = ::DragQueryFileW(drop, 0xffffffff, nullptr, 0);
					WCHAR file_name[MAX_PATH] = {};
					::DragQueryFileW(drop, 0, file_name, std::size(file_name));
					::DragFinish(drop);

					if (wcslen(file_name))
						navigate(file_name);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} main_window;
}
