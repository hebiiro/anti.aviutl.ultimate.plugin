#pragma once

namespace apn::item_wave::ui
{
	//
	// このクラスはウェイターです。
	// キャッシュを作成する予定のファイル名を保持します。
	//
	struct Waiter
	{
		//
		// 読み込む予定のファイルの名前です。
		//
		std::string file_name;

		//
		// コンストラクタです。
		//
		Waiter(LPCSTR file_name)
			: file_name(file_name)
		{
		}
	};

	//
	// このクラスはウェイターを管理します。
	//
	inline struct WaiterManager
	{
		//
		// ウェイターのコレクションです。
		//
		std::deque<std::shared_ptr<Waiter>> collection;

		//
		// ウェイターを作成して返します。
		//
		BOOL create_waiter(LPCSTR file_name)
		{
			MY_TRACE_FUNC("{}", file_name);

			auto it = std::find_if(collection.begin(), collection.end(),
				[file_name](const std::shared_ptr<Waiter>& waiter) {
					return waiter->file_name == file_name;
				});
			if (it != collection.end()) return FALSE; // 既にキューに存在する場合は何もしません。

			auto waiter = std::make_shared<Waiter>(file_name);
			collection.emplace_back(waiter);
			return TRUE;
		}

		//
		// 先頭のウェイターを返します。
		//
		auto pop_front()
		{
			auto waiter = collection.front();
			collection.pop_front();
			return waiter;
		}

		//
		// ウェイターを保有している場合はTRUEを返します。
		//
		BOOL available()
		{
			return collection.size() > 0;
		}
	} waiter_manager;
}
