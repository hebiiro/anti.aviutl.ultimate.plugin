#pragma once

namespace apn::audio_visualizer
{
	//
	// このクラスはコウィンドウマネージャです。
	// コウィンドウを管理します。
	//
	inline struct CoWindowManager
	{
		//
		// コウィンドウのコレクションです。
		//
		std::vector<std::shared_ptr<CoWindow>> collection;

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
		// コウィンドウを返します。
		//
		std::shared_ptr<CoWindow> get_co_window(size_t index)
		{
			return index < collection.size() ? collection[index] : nullptr;
		}

		//
		// コウィンドウを作成して返します。
		//
		std::shared_ptr<CoWindow> create_co_window(const std::wstring& name)
		{
			MY_TRACE_FUNC("{}", name);

			// コウィンドウのインスタンスを作成します。
			auto co_window = std::make_shared<CoWindow>();

			// コウィンドウを初期化します。
			if (!co_window->create(name)) return nullptr;

			// コウィンドウをコレクションに追加します。
			collection.emplace_back(co_window);

			// コウィンドウを返します。
			return co_window;
		}

		//
		// コウィンドウを削除します。
		//
		BOOL erase_co_window(size_t co_window_index)
		{
			MY_TRACE_FUNC("{}", co_window_index);

			// コウィンドウを取得します。
			auto it = collection.begin() + co_window_index;

			// コウィンドウをコレクションから削除します。
			collection.erase(it);

			return TRUE;
		}

		//
		// いずれかのコウィンドウが表示されている場合はTRUEを返します。
		//
		BOOL is_visible()
		{
			for (const auto& co_window : collection)
				if (::IsWindowVisible(*co_window)) return TRUE;
			return FALSE;
		}
	} co_window_manager;
}
