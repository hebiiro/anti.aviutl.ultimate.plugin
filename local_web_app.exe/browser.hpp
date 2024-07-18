#pragma once

namespace apn::local_web_app
{
	//
	// このクラスはhtmlを表示するブラウザウィンドウです。
	//
	inline struct Browser : my::Window
	{
		wil::com_ptr<ICoreWebView2Environment12> env;
		wil::com_ptr<ICoreWebView2Controller> controller;
		wil::com_ptr<ICoreWebView2_17> webview;
		EventRegistrationToken navigation_completed_token = {};
		EventRegistrationToken favicon_changed_token = {};
		EventRegistrationToken web_message_received_token = {};
		EventRegistrationToken web_resource_requested_token = {};

		//
		// urlを仮想化して返します。
		//
		std::wstring get_virtual_url(const std::wstring& url)
		{
			return L"https://"s + hive.c_name + L"/" + url;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 親ウィンドウ内に単一のWebViewを作成します。
			// ブラウザを見つけて、WebViewの環境をセットアップします。
			CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
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

					// 仮想URLをマッピングします。
					webview->SetVirtualHostNameToFolderMapping(hive.c_name,
						hive.origin_folder_path.c_str(), COREWEBVIEW2_HOST_RESOURCE_ACCESS_KIND_DENY_CORS);
#if 0
					// 以下のようにカスタムプロトコルハンドラを設定しても、
					// ハンドラ自体が呼ばれないので何もできません。
					auto filter = get_virtual_url(L"*");
					auto hr = webview->AddWebResourceRequestedFilter(
//						L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
						filter.c_str(), COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
//						filter.c_str(), COREWEBVIEW2_WEB_RESOURCE_CONTEXT_DOCUMENT);

					hr = webview->add_WebResourceRequested(
						Callback<ICoreWebView2WebResourceRequestedEventHandler>(
						[&](ICoreWebView2* sender, ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT
					{
						wil::com_ptr<ICoreWebView2WebResourceRequest> request;
						args->get_Request(&request);

						wil::unique_cotaskmem_string uri;
						request->get_Uri(&uri);

						std::wstring url(uri.get());
						if (url.find(L".html_local") != std::wstring::npos)
						{
							wil::com_ptr<ICoreWebView2HttpRequestHeaders> headers;
							request->get_Headers(&headers);

							headers->SetHeader(L"Content-Type", L"text/html");
						}

						return S_OK;
					}).Get(), &web_resource_requested_token);
#endif
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
								if (on_content_event(s.get()))
									return S_OK;
							}
						}

						{
							wil::unique_cotaskmem_string json;
							if(SUCCEEDED(args->get_WebMessageAsJson(&json)))
							{
								if (on_content_event(json.get()))
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
						if (success)
						{
							wil::unique_cotaskmem_string title;
							webview->get_DocumentTitle(&title);
							::SetWindowTextW(*this, title.get());
						}

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
								::SendMessage(*this, WM_SETICON, ICON_SMALL, (LPARAM)icon.get());
								::SendMessage(*this, WM_SETICON, ICON_BIG, (LPARAM)icon.get());
								::SetClassLong(*this, GCL_HICON, (LONG)icon.get());
								::SetClassLong(*this, GCL_HICONSM, (LONG)icon.get());
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
		// 指定されたhtmlファイルをブラウズします。
		//
		BOOL navigate(const std::wstring& file_name)
		{
			MY_TRACE_FUNC("{}", file_name);

			if (!webview) return FALSE;

			std::filesystem::path abs_path(file_name);
			MY_TRACE_STR(abs_path);

			auto rel_path = abs_path;
			if (rel_path.is_absolute())
				rel_path = rel_path.lexically_relative(hive.origin_folder_path);
			MY_TRACE_STR(rel_path);

			auto url = rel_path.wstring();
			for (size_t i = 0; i < url.size(); i++)
				if (url[i] == L'\\') url[i] = L'/';
			MY_TRACE_STR(url);

			url = get_virtual_url(url);
			MY_TRACE_STR(url);
#if 0
			// 以下のように文字列として読み込むとhtmlファイル内で相対urlが使用できなくなります。
			std::ifstream ifs(abs_path);
			std::string str((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			webview->NavigateToString(my::cp_to_wide(str, CP_UTF8).c_str());
#elif 1
			webview->Navigate(url.c_str());
#else
			// 以下のようにコンテンツタイプを指定しても、拡張子が.html_localの場合は
			// プレーンテキストとして読み込まれてしまいます。
			wil::com_ptr<ICoreWebView2WebResourceRequest> request;
			auto hr = env->CreateWebResourceRequest(
				url.c_str(), L"GET", nullptr, L"Content-Type: text/html", &request);
			hr = webview->NavigateWithWebResourceRequest(request.get());
#endif
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
		virtual BOOL on_content_event(const std::wstring& json)
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// この仮想関数は、初期化が完了したあとに呼ばれます。
		//
		virtual BOOL on_post_init()
		{
			MY_TRACE_FUNC("");

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
