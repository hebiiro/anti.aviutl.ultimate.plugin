#pragma once

namespace apn::filter_copy::model
{
	//
	// このクラスはモデルのコンテキスト(気体データ)です。
	//
	struct context_t
	{
		//
		// 操作対象のオブジェクトです。
		//
		ObjectHolder object;

		//
		// テンポラリフォルダのパスです。
		//
		std::string temp_folder_path;

		//
		// カレントプロセスのIDです。
		//
		DWORD pid = 0;

		//
		// コンストラクタです。
		//
		context_t()
			: object(magi.exin.get_current_object_index())
		{
			MY_TRACE_FUNC("");

			// 操作対象のオブジェクトが無効の場合は何もしません。
			if (!object.is_valid()) return;

			// テンポラリフォルダのパスを取得します。
			temp_folder_path = my::hs(my::get_temp_path());
			MY_TRACE_STR(temp_folder_path);

			// カレントプロセスのIDを取得します。
			pid = ::GetCurrentProcessId();
			MY_TRACE_INT(pid);
		}

		//
		// コンテキストが有効の場合はTRUEを返します。
		//
		BOOL is_valid() const { return !!pid; }

		//
		// オーダーを初期化します。
		//
		BOOL init_order()
		{
			MY_TRACE_FUNC("");

			state.flag = object.get_object()->flag;
			state.orders.clear();

			return TRUE;
		}

		//
		// 指定されたフィルタのコピーをオーダーします。
		// 指定されたフィルタのエイリアスを一時ファイルに保存し、
		// ファイル名をコレクションに追加します。
		//
		BOOL order(int32_t filter_index)
		{
			MY_TRACE_FUNC("{/}", filter_index);

			// 先頭のフィルタはコピーしません。
			if (filter_index == 0) return FALSE;

			// 移動不可能なフィルタはコピーしません。
			if (!magi.exin.is_moveable(object.get_object(), filter_index)) return FALSE;

			// フィルタを取得します。
			auto filter = magi.exin.get_filter(object.get_object(), filter_index);
			if (!filter) return FALSE;

			// 一時ファイルのファイル名を作成します。
			auto temp_file_name = my::format(
				"{/}\\aviutl_filter_copy_{/}_{/}.exa",
				temp_folder_path, pid, filter_index);
			MY_TRACE_STR(temp_file_name);

			// 一時ファイルにフィルタのエイリアスを保存します。
			if (!magi.exin.save_filter_alias(object.get_object_index(), filter_index, temp_file_name.c_str()))
			{
				MY_TRACE("magi.exin.save_filter_alias()が失敗しました\n");

				return FALSE;
			}

			// フィルタをオーダーに追加します。
			state.orders.emplace_back(filter_index, temp_file_name);

			return TRUE;
		}
	};
}
