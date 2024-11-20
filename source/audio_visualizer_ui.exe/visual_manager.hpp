#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはビジュアルマネージャです。
	// ビジュアルを管理します。
	//
	inline struct VisualManager
	{
		//
		// ビジュアルのコレクションです。
		//
		std::unordered_map<HWND, std::shared_ptr<Visual>> collection;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			collection.clear();

			return TRUE;
		}

		//
		// 名前をキーにしたビジュアルのマップを作成して返します。
		//
		auto create_visual_map()
		{
			// ビジュアルのマップを作成します。
			// キーはビジュアルのウィンドウ名です。
			std::map<std::wstring, std::shared_ptr<Visual>> visuals;
			for (const auto& pair : collection)
				visuals[my::get_window_text(pair.first)] = pair.second;

			return visuals;
		}

		//
		// ビジュアルを作成して返します。
		//
		std::shared_ptr<Visual> create_visual(HWND co_window)
		{
			MY_TRACE_FUNC("{:#010x}", co_window);

			// ビジュアルのインスタンスを作成します。
			auto visual = std::make_shared<Visual>();

			// ビジュアルを初期化します。
			if (!visual->init(co_window)) return nullptr;

			// ビジュアルをコレクションに追加します。
			collection[*visual] = visual;

			// ビジュアルを返します。
			return visual;
		}

		//
		// ビジュアルを削除します。
		//
		BOOL erase_visual(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			// ビジュアルを取得します。
			auto it = collection.find(hwnd);
			if (it == collection.end()) return FALSE;

			// ビジュアルを終了します。
			it->second->exit();

			// ビジュアルをコレクションから削除します。
			collection.erase(it);

			return TRUE;
		}

		//
		// すべてのビジュアルにオプションを送信します。
		//
		BOOL notify_update_option(HWND visual, const share::Option& option)
		{
			MY_TRACE_FUNC("{:#010x}", visual);

			// jsonを構築します。
			n_json root = {
				{
					"range", {
						{ "min", option.range.min },
						{ "max", option.range.max },
					},
				},
				{
					"marker", {
						{ "base", option.marker.base },
						{ "area", option.marker.area },
					},
				},
			};

			// jsonからウェブメッセージ文字列を作成します。
			auto json = Browser::create_web_message_as_json("update_option", root);

			if (visual)
			{
				auto it = collection.find(visual);
				if (it == collection.end()) return FALSE;

				// ウェブメッセージをビジュアルのスクリプトに送信します。
				it->second->browser.post_web_message_as_json(json);
			}
			else
			{
				// すべてのビジュアルを走査します。
				for (const auto& pair : collection)
				{
					// ウェブメッセージをビジュアルのスクリプトに送信します。
					pair.second->browser.post_web_message_as_json(json);
				}
			}

			return TRUE;
		}

		//
		// すべてのビジュアルに音声アーティファクトを送信します。
		//
		BOOL notify_update_audio(const share::AudioArtifact& audio_artifact)
		{
			MY_TRACE_FUNC("");

			// jsonを構築します。
			n_json root = {
				{ "frame", audio_artifact.frame },
				{ "time", audio_artifact.time },
				{ "spectre_option", audio_artifact.spectre_option },
				{ "channel", audio_artifact.channels },
			};

			// jsonからウェブメッセージ文字列を作成します。
			auto json = Browser::create_web_message_as_json("update_audio", root);

			// すべてのビジュアルを走査します。
			for (const auto& pair : collection)
			{
				// ビジュアルを取得します。
				auto& visual = pair.second;

				// ビジュアルエディタが表示されている場合は何もしません。
				// (何故か操作不能になってしまうので)
				if (::IsWindowVisible(visual->editor))
					continue;

				// ウェブメッセージをビジュアルのスクリプトに送信します。
				visual->browser.post_web_message_as_json(json);
			}

			return TRUE;
		}
	} visual_manager;
}
