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
			wc.lpszClassName = L"local_web_app.main_window";
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

			return __super::init(hive.instance);
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
		// コンテンツから送られてきたメッセージ(json)を処理します。
		//
		virtual BOOL on_web_message_as_json(const std::string& key, const n_json& node) override
		{
			try
			{
				if (key == "request_write_json")
				{
					// 書き込み先のファイルパスを取得します。
					// このパスは相対パスでなければなりません。
					std::filesystem::path path = node["path"];
					MY_TRACE_STR(path);
					if (path.empty()) return FALSE;

					// 絶対パスに変換します。
					auto abs_path = get_abs_path(path);
					MY_TRACE_STR(abs_path);

					// フォルダが存在しないかもしれないので作成します。
					std::filesystem::create_directories(abs_path.parent_path());

					// 書き込むデータ(json)を取得します。
					const auto& data_node = node["data"];

					// 指定されたデータを指定されたファイルに書き込みます。
					std::ofstream ofs(abs_path);
					ofs << data_node.dump(1, '\t');

					n_json root = {
						{
							"response_write_json", {
								{ "path", my::wide_to_cp(path, CP_UTF8) },
								{ "abs_path", my::wide_to_cp(abs_path, CP_UTF8) },
							},
						},
					};

					// jsonを文字列化します。
					auto json = my::cp_to_wide(root.dump(1, '\t'), CP_UTF8);

					// 文字列化したjsonをブラウザに送信します。
					post_web_message_as_json(json);
				}
				else if (key == "request_read_json")
				{
					try
					{
						// 読み込み先のファイルパスを取得します。
						// このパスは相対パスでなければなりません。
						std::filesystem::path path = node["path"];
						MY_TRACE_STR(path);
						if (path.empty()) return FALSE;

						// 絶対パスに変換します。
						auto abs_path = get_abs_path(path);
						MY_TRACE_STR(abs_path);

						// 指定されたファイルを読み込みます。
						std::ifstream ifs(abs_path);
						if (!ifs) return FALSE;

						auto data = n_json::parse(ifs);

						n_json root = {
							{
								"response_read_json", {
									{ "path", my::wide_to_cp(path, CP_UTF8) },
									{ "abs_path", my::wide_to_cp(abs_path, CP_UTF8) },
									{ "data", data },
								},
							},
						};

						// jsonを文字列化します。
						auto json = my::cp_to_wide(root.dump(1, '\t'), CP_UTF8);

						// 文字列化したjsonをブラウザに送信します。
						post_web_message_as_json(json);
					}
					catch (const std::exception& error)
					{
						(void)error;
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
					MY_TRACE_FUNC("WM_CREATE, {/hex}, {/hex}", wParam, lParam);

					::DragAcceptFiles(hwnd, TRUE);

					// このウィンドウをメインウィンドウに設定します。
					hive.main_window = hwnd;

					break;
				}
			case WM_DESTROY:
				{
					MY_TRACE_FUNC("WM_DESTROY, {/hex}, {/hex}", wParam, lParam);

					// メッセージループを終了します。
					::PostQuitMessage(0);

					break;
				}
			case WM_CLOSE:
				{
					MY_TRACE_FUNC("WM_CLOSE, {/hex}, {/hex}", wParam, lParam);

					// コンフィグを書き込みます。
					config_io.write();

					break;
				}
			case WM_DROPFILES:
				{
					MY_TRACE_FUNC("WM_DROP_FILES, {/hex}, {/hex}", wParam, lParam);

					auto drop = (HDROP)wParam;
					auto c = ::DragQueryFileW(drop, 0xffffffff, nullptr, 0);
					WCHAR file_name[MAX_PATH] = {};
					::DragQueryFileW(drop, 0, file_name, std::size(file_name));
					::DragFinish(drop);

					if (::lstrlenW(file_name))
						navigate(file_name);

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} main_window;
}
