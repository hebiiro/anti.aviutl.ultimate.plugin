#pragma once

namespace apn
{
	//
	// このクラスはブラウザのプロトタイプです。
	//
	struct PrototypicBrowser : my::Window
	{
		wil::com_ptr<ICoreWebView2Environment12> env;
		wil::com_ptr<ICoreWebView2Controller> controller;
		wil::com_ptr<ICoreWebView2_17> webview;
		EventRegistrationToken navigation_completed_token = {};
		EventRegistrationToken favicon_changed_token = {};
		EventRegistrationToken web_message_received_token = {};
		EventRegistrationToken web_resource_requested_token = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init(HINSTANCE instance)
		{
			MY_TRACE_FUNC("{:#010x}", instance);

			// ユーザーデータを保存するフォルダです。
			auto user_data_folder = my::get_module_file_name(instance).parent_path() / L"wv2";
			MY_TRACE_STR(user_data_folder);

			// 親ウィンドウ内に単一のWebViewを作成します。
			// ブラウザを見つけて、WebViewの環境をセットアップします。
			CreateCoreWebView2EnvironmentWithOptions(
				nullptr, user_data_folder.c_str(), nullptr,
				Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
				[&](HRESULT result, ICoreWebView2Environment* env) -> HRESULT
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}", result, env);

				env->QueryInterface(&this->env);

				// コントローラを作成します。
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

					_webview.try_query_to(&webview);
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

					// コントローラをリサイズします。
					resize_controller();

					// ウェブメッセージハンドラを追加します。
					webview->add_WebMessageReceived(
						Callback<ICoreWebView2WebMessageReceivedEventHandler>(
						[&](ICoreWebView2* webview, ICoreWebView2WebMessageReceivedEventArgs* args) -> HRESULT
					{
						MY_TRACE_FUNC("{:#010x}, {:#010x}", webview, args);

						{
							wil::unique_cotaskmem_string s;
							if (SUCCEEDED(args->TryGetWebMessageAsString(&s)))
							{
								if (on_web_message_as_string(s.get()))
									return S_OK;
							}
						}

						{
							wil::unique_cotaskmem_string json;
							if(SUCCEEDED(args->get_WebMessageAsJson(&json)))
							{
								if (on_web_message_as_json(json.get()))
									return S_OK;
							}
						}

						return S_OK;
					}).Get(), &web_message_received_token);

					// ナビゲーション完了ハンドラを追加します。
					webview->add_NavigationCompleted(
						Callback<ICoreWebView2NavigationCompletedEventHandler>(
						[&](ICoreWebView2* sender, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
					{
						MY_TRACE_FUNC("{:#010x}, {:#010x}", sender, args);

						BOOL success = FALSE;
						args->get_IsSuccess(&success);
						on_navigation_completed(success);

						return S_OK;
					}).Get(), &navigation_completed_token);

					// ファビコン変更ハンドラを追加します。
					webview->add_FaviconChanged(
						Callback<ICoreWebView2FaviconChangedEventHandler>(
						[&](ICoreWebView2* sender, IUnknown* args) -> HRESULT
					{
						MY_TRACE_FUNC("{:#010x}, {:#010x}", sender, args);
#if 0
						wil::unique_cotaskmem_string url;
						webview->get_FaviconUri(&url);
						MY_TRACE_STR(url.get());
#endif
						// ファビコンを取得します。
						webview->GetFavicon(
							COREWEBVIEW2_FAVICON_IMAGE_FORMAT_PNG,
							Callback<ICoreWebView2GetFaviconCompletedHandler>(
							[&](HRESULT error_code, IStream* icon_stream) -> HRESULT
						{
							Gdiplus::Bitmap icon_bitmap(icon_stream);
							wil::unique_hicon icon;
							if (icon_bitmap.GetHICON(&icon) == Gdiplus::Status::Ok)
							{
								if (auto hwnd = get_title_window())
								{
									::SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)icon.get());
									::SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)icon.get());
									::SetClassLong(hwnd, GCL_HICON, (LONG)icon.get());
									::SetClassLong(hwnd, GCL_HICONSM, (LONG)icon.get());
								}
							}

							return S_OK;
						}).Get());

						return S_OK;
					}).Get(), &favicon_changed_token);

					on_post_init();

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
		// ファイルパスをローカル用URLに変更して返します。
		//
		std::wstring get_local_url(const std::filesystem::path& file_name)
		{
			// file:///スキームを使用します。

			auto url = L"file:///" + file_name.wstring();
			for (size_t i = 0; i < url.size(); i++) if (url[i] == L'\\') url[i] = L'/';
			MY_TRACE_STR(url);

			return url;
		}

		//
		// 指定されたhtmlファイルをブラウズします。
		//
		BOOL navigate(const std::wstring& file_name)
		{
			MY_TRACE_FUNC("{}", file_name);

			if (!webview) return FALSE;

			auto url = get_local_url(file_name);

			webview->Navigate(url.c_str());

			return TRUE;
		}

		//
		// コンテンツにメッセージ(文字列)を送信します。
		//
		BOOL post_web_message_as_string(const std::wstring& s)
		{
			MY_TRACE_FUNC("{}", s);

			if (!webview) return FALSE;

			webview->PostWebMessageAsString(s.c_str());

			return TRUE;
		}

		//
		// コンテンツにメッセージ(json)を送信します。
		//
		BOOL post_web_message_as_json(const std::wstring& json)
		{
			MY_TRACE_FUNC("{}", json);

			if (!webview) return FALSE;

			webview->PostWebMessageAsJson(json.c_str());

			return TRUE;
		}

		//
		// この仮想関数は、タイトルバーを持つウィンドウを返します。
		//
		virtual HWND get_title_window()
		{
			return *this;
		}

		//
		// この仮想関数は、初期化が完了したあとに呼ばれます。
		//
		virtual BOOL on_post_init()
		{
			MY_TRACE_FUNC("");

			controller->put_IsVisible(TRUE);

			return TRUE;
		}

		//
		// この仮想関数は、ナビゲーションが完了したあとに呼ばれます。
		//
		virtual BOOL on_navigation_completed(BOOL success)
		{
			MY_TRACE_FUNC("{}", success);

			if (success)
			{
				if (auto hwnd = get_title_window())
				{
					wil::unique_cotaskmem_string title;
					webview->get_DocumentTitle(&title);
					::SetWindowTextW(hwnd, title.get());
				}
			}

			return TRUE;
		}

		//
		// コンテンツから送られてきたメッセージ(文字列)を処理します。
		//
		virtual BOOL on_web_message_as_string(const std::wstring& s)
		{
			MY_TRACE_FUNC("{}", s);

			return TRUE;
		}

		//
		// コンテンツから送られてきたメッセージ(json)を処理します。
		//
		virtual BOOL on_web_message_as_json(const std::wstring& json)
		{
			MY_TRACE_FUNC("{}", json);

			return TRUE;
		}

		//
		// コントローラの表示状態を変更します。
		//
		BOOL show_controller(BOOL visible)
		{
			MY_TRACE_FUNC("{}", visible);

			if (!controller) return FALSE;
			return SUCCEEDED(controller->put_IsVisible(visible));
		}

		//
		// コントローラをリサイズします。
		//
		BOOL resize_controller()
		{
			MY_TRACE_FUNC("");

			if (!controller) return FALSE;
			auto rc = my::get_client_rect(hwnd);
			return SUCCEEDED(controller->put_Bounds(rc));
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
	};
}
