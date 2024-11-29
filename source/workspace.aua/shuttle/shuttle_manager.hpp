#pragma once

namespace apn::workspace
{
	//
	// このクラスはシャトルを管理します。
	//
	inline struct ShuttleManager : Shuttle::StaticListener
	{
		//
		// シャトルのコレクションです。
		//
		std::map<std::wstring, std::shared_ptr<Shuttle>> collection;

		//
		// コンストラクタです。
		//
		ShuttleManager()
		{
			MY_TRACE_FUNC("");

			// シャトルマネージャーをシャトルのスタティックリスナーに設定します。
			Shuttle::static_listener = this;
		}

		//
		// デストラクタです。
		//
		virtual ~ShuttleManager()
		{
			MY_TRACE_FUNC("");

			// シャトルマネージャーをシャトルのスタティックリスナーから解除します。
			Shuttle::static_listener = nullptr;
		}

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
		// 指定された名前のシャトルを返します。
		//
		std::shared_ptr<Shuttle> get(const std::wstring& name)
		{
			auto it = collection.find(name);
			if (it == collection.end()) return nullptr;
			return it->second;
		}

		// シャトルの名前を変更します。
		BOOL rename(Shuttle* shuttle, const std::wstring& new_name)
		{
			// new_nameが既存の名前の場合はFALSEを返します。
			if (collection.find(new_name) != collection.end())
				return FALSE;

			// シャトルの以前の名前を取得しておきます。
			auto old_name = shuttle->name;

			// シャトルの名前を変更します。
			shuttle->name = new_name;

			// 新しい名前でコレクションに追加します。
			collection[new_name] = collection[old_name];

			// 以前の名前をマップから削除します。
			collection.erase(old_name);

			// シャトルに名前の変更を通知します。
			// 通常は新しい名前をウィンドウ名に設定します。
			shuttle->on_rename();

			return TRUE;
		}

		// シャトルをリネームするためのダイアログを表示します。
		int show_rename_dialog(Shuttle* shuttle, HWND parent)
		{
			RenameDialog dialog(shuttle);

			return dialog.do_modal(parent);
		}

		//
		// このクラスはシャトルをリネームするためのダイアログです。
		//
		struct RenameDialog : my::Dialog2
		{
			Shuttle* shuttle = nullptr;

			RenameDialog(Shuttle* shuttle)
				: shuttle(shuttle)
			{
			}

			int do_modal(HWND parent)
			{
				create(hive.instance, MAKEINTRESOURCE(IDD_RENAME_SHUTTLE), parent);

				auto name = shuttle->name;
				bind_text(IDC_NEW_NAME, name);

				auto ret_value = __super::do_modal2(parent);

				if (IDOK != ret_value)
					return ret_value;

				// 古い名前と新しい名前が違うなら
				if (shuttle->name != name)
				{
					// シャトルの名前を変更します。
					shuttle_manager.rename(shuttle, name);
				}

				return ret_value;
			}

			virtual void on_ok() override
			{
				auto new_name = get_text(IDC_NEW_NAME);

				// 新しい名前が現在のシャトルの名前と違い、
				// なおかつ新しい名前と同じ名前のシャトルがすでに存在する場合は
				if (new_name != shuttle->name && shuttle_manager.get(new_name))
				{
					// メッセージボックスを表示して拒否します。
					hive.message_box(_T("名前が重複しています"), *this);

					return;
				}

				__super::on_ok();
			}

			virtual void on_cancel() override
			{
				__super::on_cancel();
			}
		};

		//
		// シャトルをコレクションに追加します。
		//
		BOOL add(const std::shared_ptr<Shuttle>& shuttle, const std::wstring& name)
		{
			// 指定された名前のシャトルがすでに存在している場合は
			if (get(name))
			{
				// メッセージボックスを出して処理を中止します。
				hive.message_box(std::format(L"ウィンドウ名が重複しています\nウィンドウ名 : {}", name), hive.main_window);

				return FALSE;
			}

			// シャトルをコレクションに追加します。
			collection[name] = shuttle;

			return TRUE;
		}

		//
		// 指定されたシャトルをコレクションから取り除きます。
		//
		BOOL erase(const std::shared_ptr<Shuttle>& shuttle)
		{
			// リスナーにシャトルの削除を通知します。
			shuttle->fire_release_shuttle();

			// コレクションから削除します。
			return !!collection.erase(shuttle->name);
		}

		//
		// シャトルが作成されたときに呼ばれます。
		//
		void on_init_shuttle(const std::shared_ptr<Shuttle>& shuttle) override
		{
			MY_TRACE_FUNC("{}", shuttle->name);

			add(shuttle, shuttle->name);
		}

		//
		// シャトルが破壊されたときに呼ばれます。
		//
		void on_exit_shuttle(const std::shared_ptr<Shuttle>& shuttle) override
		{
			MY_TRACE_FUNC("{}", shuttle->name);

			// このシャトルをコレクションから削除します。
			erase(shuttle);
		}
	} shuttle_manager;
}
