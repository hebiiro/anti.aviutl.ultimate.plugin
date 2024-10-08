#pragma once

namespace apn::settings_browser
{
	//
	// このクラスはアイテムの設定をブラウザで表示します。
	//
	inline struct Browser : apn::Browser
	{
		//
		// コンテンツのコンフィグです。
		// コンテンツから発行されるコンフィグの書き込みイベントで更新されます。
		// (実際にファイルに書き込まれるのはプロセス終了時です)
		//
		n_json config;

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
			wc.lpszClassName = L"settings_browser.browser";
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

			return __super::init(hive.instance);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return __super::exit();
		}

		//
		// コンテンツファイルをセットしてからナビゲートします。
		//
		BOOL navigate(const std::wstring& file_name)
		{
			contents_file_name = file_name;
			if (contents_file_name.is_relative())
				contents_file_name = hive.assets_folder_name / contents_file_name;
			MY_TRACE_STR(contents_file_name);

			return __super::navigate();
		}

		//
		// コンテンツから送られてきたメッセージ(文字列)を処理します。
		//
		virtual BOOL on_web_message_as_string(const std::wstring& s) override
		{
			MY_TRACE_FUNC("{}", s);

			if (s == L"fire_show_settings")
			{
				if (!hive.settings_json.empty())
					post_web_message_as_json(hive.settings_json);
			}

			return TRUE;
		}

		//
		// コンテンツから送られてきたメッセージ(json)を処理します。
		//
		virtual BOOL on_web_message_as_json(const std::string& key, const n_json& json) override
		{
			try
			{
				if (key == "write_config")
				{
					config = json;
				}
			}
			catch (...)
			{
			}

			return TRUE;
		}

		//
		// この仮想関数は、エラーが発生したときに呼ばれます。
		//
		virtual BOOL on_error(const std::wstring& what) override
		{
			return hive.message_box(what, *this);
		}
	} browser;
}
