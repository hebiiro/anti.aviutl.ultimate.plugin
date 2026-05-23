#pragma once

namespace apn::filter_copy::model
{
	//
	// このクラスはモデルのコンテキスト(気体データ)です。
	//
	struct context_t
	{
		//
		// コンテキストが存在する間のファイル操作をアトミックにします。
		//
		my::Synchronizer synchronizer;

		//
		// 操作対象のオブジェクトです。
		//
		ObjectHolder object;

		//
		// テンポラリフォルダのパスです。
		//
		std::filesystem::path temp_folder_path;

		//
		// コピーデータのパスです。
		//
		std::filesystem::path copy_data_path;

		//
		// コピー済みフィルタの数です。
		//
		int32_t nb_copied_filters = 0;

		//
		// コピペ用データです。
		//
		struct copy_data_t {
			ExEdit::Object::Flag src_object_flag;
			int32_t nb_copied_filters;
		};

		//
		// コンストラクタです。
		//
		context_t(int32_t object_index)
			: synchronizer(model::property.mutex)
			, object(object_index)
		{
			MY_TRACE_FUNC("");

			// 操作対象のオブジェクトが無効の場合は何もしません。
			if (!object.is_valid()) return;

			// テンポラリフォルダのパスを取得します。
			temp_folder_path = my::get_temp_path();
			MY_TRACE_STR(temp_folder_path);

			// コピーデータのパスを取得します。
			copy_data_path = temp_folder_path / L"aviutl_filter_copy.bin";
			MY_TRACE_STR(copy_data_path);
		}

		//
		// コンテキストが有効の場合はTRUEを返します。
		//
		BOOL is_valid() const { return object.is_valid(); }

		//
		// コピーデータをファイルから読み込みます。
		//
		copy_data_t read_copy_data()
		{
			copy_data_t copy_data = {};

			try
			{
				// 入力ストリームを開きます。
				std::ifstream ifs(copy_data_path, std::ios::binary);

				// 入力ストリームからコピーデータを読み込みます。
				ifs.read((char*)&copy_data, sizeof(copy_data));
			}
			catch (...)
			{
			}

			return copy_data;
		}

		//
		// コピーデータをファイルに書き込みます。
		//
		BOOL write_copy_data(const copy_data_t& copy_data)
		{
			try
			{
				// 出力ストリームを開きます。
				std::ofstream ofs(copy_data_path, std::ios::binary);

				// 出力ストリームにコピーデータを書き込みます。
				ofs.write((const char*)&copy_data, sizeof(copy_data));

				return TRUE;
			}
			catch (...)
			{
			}

			return FALSE;
		}

		//
		// エイリアスのパスを返します。
		//
		std::filesystem::path get_alias_path(int32_t index)
		{
			return temp_folder_path / my::format(
				L"aviutl_filter_copy_{/}.exa", index);
		}

		//
		// 指定されたフィルタのエイリアスファイルを作成します。
		//
		BOOL write_alias(int32_t filter_index)
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
			auto temp_file_name = get_alias_path(nb_copied_filters);
			MY_TRACE_STR(temp_file_name);

			// 一時ファイルにフィルタのエイリアスを保存します。
			if (!magi.exin.save_filter_alias(object.get_object_index(), filter_index, temp_file_name.string().c_str()))
			{
				MY_TRACE("magi.exin.save_filter_alias()が失敗しました\n");

				return FALSE;
			}

			// コピー済みフィルタの数を増やします。
			nb_copied_filters++;

			return TRUE;
		}
	};
}
