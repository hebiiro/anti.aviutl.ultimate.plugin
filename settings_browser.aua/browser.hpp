#pragma once

namespace apn::settings_browser
{
	//
	// このクラスはアイテムの設定をブラウザで表示します。
	//
	inline struct Browser : my::Window
	{
		wil::com_ptr<ICoreWebView2Controller> controller;
		wil::com_ptr<ICoreWebView2_3> webview;
		EventRegistrationToken navigation_completed_token = {};
		EventRegistrationToken web_message_received_token = {};

		//
		// コンテンツのコンフィグです。
		// コンテンツから発行されるコンフィグの書き込みイベントで更新されます。
		// (実際にファイルに書き込まれるのはプロセス終了時です)
		//
		ptree config;

		//
		// 初期化処理を実行します。
		//
		BOOL init(HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}", parent);

			WNDCLASSEXW wc = { sizeof(wc) };
			wc.style = CS_HREDRAW | CS_VREDRAW;
			wc.lpfnWndProc = ::DefWindowProc;
			wc.hInstance = hive.instance;
			wc.hCursor = ::LoadCursor(nullptr, IDC_ARROW);
			wc.lpszClassName = _T("settings_browser.browser");
			::RegisterClassExW(&wc);

			if (!__super::create(
				WS_EX_NOPARENTNOTIFY,
				wc.lpszClassName,
				wc.lpszClassName,
				WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 600, 600,
				parent, (HMENU)IDC_BROWSER, hive.instance, nullptr))
			{
				hive.message_box(L"ブラウザウィンドウの作成に失敗しました");

				return FALSE;
			}

			// 親ウィンドウ内に単一のWebViewを作成します。
			// ブラウザを見つけて、WebViewの環境をセットアップします。
			CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
				Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}", result, env);

				env->CreateCoreWebView2Controller(*this,
					Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[&](HRESULT result, ICoreWebView2Controller* _controller) -> HRESULT
				{
					MY_TRACE_FUNC("{:#010x}, {:#010x}", result, _controller);

					controller = _controller;
					MY_TRACE_HEX(controller.get());
					if (!controller) return E_FAIL;

					wil::com_ptr<ICoreWebView2> _webview;
					controller->get_CoreWebView2(&_webview);
					MY_TRACE_HEX(_webview.get());
					if (!_webview) return E_FAIL;

					webview = _webview.try_query<ICoreWebView2_3>();
					MY_TRACE_HEX(webview.get());
					if (!webview) return E_FAIL;

					{
						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						MY_TRACE_HEX(settings.get());
						if (!settings) return E_FAIL;
						settings->put_IsScriptEnabled(TRUE);
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);
						settings->put_IsWebMessageEnabled(TRUE);
					}

					resize_controller();

					webview->SetVirtualHostNameToFolderMapping(hive.c_name,
						hive.assets_folder_name.c_str(), COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY_CORS);

					webview->add_WebMessageReceived(
						Callback<ICoreWebView2WebMessageReceivedEventHandler>(
						[&](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT
					{
						MY_TRACE_FUNC("{:#010x}, {:#010x}", webview, args);

						{
							wil::unique_cotaskmem_string s;
							if (SUCCEEDED(args->TryGetWebMessageAsString(&s)))
							{
								if (::lstrcmpW(s.get(), L"fire_show_settings") == 0)
								{
									if (!hive.settings_json.empty())
										post_json(hive.settings_json);

									return S_OK;
								}
							}
						}

						{
							wil::unique_cotaskmem_string json;
							if(SUCCEEDED(args->get_WebMessageAsJson(&json)))
								on_content_event(json.get());
						}

						return S_OK;
					}).Get(), &web_message_received_token);

					return S_OK;
				}).Get());

				return S_OK;
			}).Get());

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			webview.reset();
			controller.reset();

			return TRUE;
		}

		//
		// 指定されたhtmlファイルをブラウズします。
		//
		BOOL navigate(const std::wstring& file_name)
		{
			MY_TRACE_FUNC("{}", file_name);

			if (!webview) return FALSE;

			std::filesystem::path path(file_name);
			MY_TRACE_STR(path);
			if (path.is_absolute())
				path = path.lexically_relative(hive.assets_folder_name);
			MY_TRACE_STR(path);
			auto url = L"https://"s + hive.c_name + L"/" + path.wstring();
			MY_TRACE_STR(url);

			webview->Navigate(url.c_str());

			return TRUE;
		}

		//
		// コンテンツにイベントを発行します。
		//
		BOOL post_json(const std::wstring& json)
		{
			MY_TRACE_FUNC("");

			if (!webview) return FALSE;

			webview->PostWebMessageAsJson(json.c_str());

			return TRUE;
		}

		//
		// コンテンツから発行されたイベントを処理します。
		//
		BOOL on_content_event(const std::wstring& json)
		{
			MY_TRACE_FUNC("");

			ptree root;
			std::stringstream stream(my::wide_to_cp(json, CP_UTF8));
			read_json(stream, root);
			for (const auto& it : root)
			{
				if (it.first == "write_config")
				{
					config = it.second;
				}
			}

			return TRUE;
		}

		//
		// コントローラをリサイズします。
		//
		BOOL resize_controller()
		{
			MY_TRACE_FUNC("");

			if (!controller) return FALSE;
			auto rc = my::get_client_rect(hwnd);
			controller->put_Bounds(rc);
			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_SIZE:
				{
					MY_TRACE_FUNC("WM_SIZE, {:#010x}, {:#010x}", wParam, lParam);

					// コントローラをリサイズします。
					resize_controller();

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} browser;
}
