#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはブラウザです。
	//
	struct Browser : PrototypicBrowser
	{
		//
		// 表示するコンテンツ(html)ファイルのパスです。
		//
		std::filesystem::path contents_file_name;

		//
		// スクリプトにメッセージを送信します。
		//
		static std::wstring create_web_message_as_json(const std::string& key, const n_json& value)
		{
			// jsonを構築します。
			n_json root;
			root[key] = value;

			// jsonを文字列化して返します。
			return my::cp_to_wide(root.dump(4), CP_UTF8);
		}

		//
		// コンテンツをナビゲートします。
		//
		BOOL navigate()
		{
			MY_TRACE_FUNC("");

			return contents_file_name.empty() ?
				FALSE : __super::navigate(contents_file_name);
		}

		//
		// この仮想関数は、初期化が完了したあとに呼ばれます。
		//
		virtual BOOL on_post_init() override
		{
			MY_TRACE_FUNC("");

			if (!__super::on_post_init()) return FALSE;

			return navigate();
		}

		//
		// この仮想関数は、スクリプトからメッセージが発行されたときに呼ばれます。
		//
		virtual BOOL on_web_message_as_json(const std::string& key, const n_json& json)
		{
			return TRUE;
		}

		//
		// この仮想関数は、スクリプトからメッセージが発行されたときに呼ばれます。
		//
		virtual BOOL on_web_message_as_json(const std::wstring& json) override
		{
			MY_TRACE_FUNC("{}", json);

			try
			{
				auto root = n_json::parse(my::wide_to_cp(json, CP_UTF8));

				for (const auto& node : root.items())
					on_web_message_as_json(node.key(), node.value());
			}
			catch (const std::exception& error)
			{
				hive.message_box(my::ws(error.what()), *this);
			}

			return TRUE;
		}
	};
}
