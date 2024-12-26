#pragma once

namespace apn::one_for_text
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// 保存先テキストファイルのファイル名を返します。
		//
		std::wstring get_text_file_name()
		{
			// 保存先テキストファイルのファイル名が無効の場合は
			if (hive.text_file_name.empty())
			{
				// アセットフォルダ内のファイル名を返します。
				return magi.get_assets_file_name(hive.c_display_name) + L".txt";
			}
			// 保存先テキストファイルのファイル名が有効の場合は
			else
			{
				// 保存先テキストファイルのファイル名をそのまま返します。
				return hive.text_file_name;
			}
		}

		//
		// テキストオブジェクトの拡張データを返します。
		//
		ExEdit::Exdata::efText* get_text_exdata(ExEdit::Object* object, int32_t layer_front, int32_t layer_back)
		{
			// オブジェクトが無効の場合は除外します。
			if (!(object->flag & ExEdit::Object::Flag::Exist))
				return {};

			// レイヤー番号が範囲外の場合は除外します。
			if (object->layer_set < layer_front || object->layer_set > layer_back)
				return {};

			// テキストオブジェクトではない場合は除外します。
			if (object->filter_param[0].id != 3)
				return {};

			// テキストオブジェクトの拡張データを取得します。
			return (ExEdit::Exdata::efText*)magi.exin.get_exdata(object, 0);
		}

		//
		// テキストオブジェクトのテキストを一括して返します。
		//
		std::wstring get_mix_text(BOOL current_scene_only, int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("{}, {}, {}", current_scene_only, layer_front, layer_back);

			// 末尾レイヤー番号を調整します。
			if (layer_back < layer_front) layer_back = layer_front;

			//
			// オブジェクトからミックステキストパートを取得します。
			//
			auto get_mix_text_part = [&](ExEdit::Object* object) -> std::wstring
			{
				// テキストオブジェクトの拡張データを取得します。
				auto exdata = get_text_exdata(object, layer_front, layer_back);
				if (!exdata) return {};

				// ヘッダーテキストを構築します。
				auto header_text = std::format(
					LR"(/[/{{ "index":{}, "scene_set":{}, "layer_set":{}, "frame_begin":{}, "frame_end":{} }}/]/)",
					magi.exin.get_object_index(object),
					object->scene_set,
					object->layer_set,
					object->frame_begin,
					object->frame_end);

				// ヘッダーテキストと本体テキストを結合して返します。
				return header_text + L"\r\n" + exdata->text + L"\r\n";
			};

			std::wstring mix_text = L"\xfeff";

			// 現在のシーンに配置されているテキストオブジェクトのみに限定する場合は
			if (current_scene_only)
			{
				// ソート済みオブジェクトを走査します。
				auto c = magi.exin.get_sorted_object_count();
				for (auto i = 0; i < c; i++)
				{
					// ソート済みオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(i);
					if (!object) continue;

					// パートを取得して追加します。
					mix_text += get_mix_text_part(object);
				}
			}
			else
			{
				// すべてのオブジェクトを走査します。
				auto c = magi.exin.get_object_count();
				for (auto i = 0; i < c; i++)
				{
					// オブジェクトを取得します。
					auto object = magi.exin.get_object(i);
					if (!object) continue;

					// パートを取得して追加します。
					mix_text += get_mix_text_part(object);
				}
			}

			return mix_text;
		}

		//
		// テキストオブジェクトのテキストを一つにまとめてファイルに書き込みます。
		//
		BOOL write_file(BOOL current_scene_only, int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("");

			try
			{
				// 保存先テキストファイルのファイル名を取得します。
				auto text_file_name = get_text_file_name();

				// テキストオブジェクトのテキストを一つにまとめます。
				auto mix_text = get_mix_text(current_scene_only, layer_front, layer_back);

				// バイナリファイルストリームを開きます。
				std::ofstream ofs(text_file_name, std::ios::binary);

				// ユニコードテキストをバイナリとして書き込みます。
				ofs.write((LPCSTR)mix_text.data(), mix_text.size() * sizeof(WCHAR));
			}
			catch (const std::exception& e)
			{
				hive.message_box(my::format(L"ファイルの書き込みに失敗しました\n{}", e.what()));

				return FALSE;
			}

			return TRUE;
		}

		//
		// ミックステキストをテキストオブジェクトに適用します。
		//
		BOOL apply_mix_text(const std::wstring& mix_text, BOOL current_scene_only, int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("{}, {}, {}", current_scene_only, layer_front, layer_back);

			// 末尾レイヤー番号を調整します。
			if (layer_back < layer_front) layer_back = layer_front;

			const std::wstring prefix = L"/[/";
			const std::wstring suffix = L"/]/\r";

			// オブジェクトの数を取得しておきます。
			const auto c = magi.exin.get_object_count();
			const auto current_scene_set = magi.exin.get_current_scene_index();

			// 現在読み込み中のテキストオブジェクトです。
			ExEdit::Object* current_object = nullptr;
			ExEdit::Exdata::efText* current_exdata = nullptr;
			std::vector<int32_t> modified_objects;

			magi.exin.push_undo();

			// 文字列ストリームを作成します。
			std::wstringstream stream(mix_text);

			// 文字列を行毎に読み込みます。
			std::wstring line;
			while (std::getline(stream, line))
			{
				try
				{
					// ヘッダー行の場合は
					if (line.starts_with(prefix) && line.ends_with(suffix))
					{
						// ヘッダーのプレフィックスとサフィックスを取り除いた部分を取得します。
						auto header_text = line.substr(prefix.length(), line.length() - prefix.length() - suffix.length());

						// ヘッダーテキストをjsonに変換します。
						auto header_node = n_json::parse(header_text);

						// オブジェクトのインデックスを取得します。
						int32_t object_index = -1;
						get_int(header_node, "index", object_index);

						// オブジェクトのインデックスが無効の場合は何もしません。
						if (object_index < 0 || object_index >= c) continue;

						// オブジェクトを取得します。
						auto object = magi.exin.get_object(object_index);

						// 適用範囲がカレントシーンのみの場合は
						if (current_scene_only)
						{
							// カレントシーン外のオブジェクトの場合は除外します。
							if (object->scene_set != current_scene_set) continue;
						}

						// テキストオブジェクトの拡張データを取得します。
						current_exdata = get_text_exdata(object, layer_front, layer_back);

						// 拡張データを取得できた場合は
						if (current_exdata)
						{
							// オブジェクトをアンドゥの対象にします。
							magi.exin.create_undo(object_index, 0x00);

							// 編集対象のオブジェクトとして記録しておきます。
							modified_objects.emplace_back(object_index);

							// テキストを空にします。
							current_exdata->text[0] = L'\0';
						}
					}
					// 本体行の場合は
					else if (current_exdata && line.length())
					{
						// 末尾が改行の場合は正しく復元します。
						if (line.back() == L'\r') line.push_back(L'\n');

						// 拡張データにテキストを追加します。
						wcsncat_s(current_exdata->text, std::size(current_exdata->text), line.c_str(), _TRUNCATE);
					}
				}
				catch (...)
				{
				}
			}

			// 編集したオブジェクトを走査します。
			for (auto object_index : modified_objects)
			{
				// オブジェクトを取得します。
				auto object = magi.exin.get_object(object_index);

				// テキストオブジェクトの拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efText*)magi.exin.get_exdata(object, 0);

				::WideCharToMultiByte(CP_ACP, 0, exdata->text, -1,
					object->dispname, std::size(object->dispname), nullptr, nullptr);
				MY_TRACE_STR(object->dispname);
			}

			// 拡張編集を再描画します。
			magi.exin.update_object_table();
			magi.exin.redraw_timeline();
			magi.exin.set_current_object(magi.exin.get_current_object_index());

			// AviUtlを再描画します。
			magi.redraw();

			return TRUE;
		}

		//
		// 保存先テキストファイルをテキストオブジェクトに適用します。
		//
		BOOL read_file(BOOL current_scene_only, int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("");

			try
			{
				// 保存先テキストファイルのファイル名を取得します。
				auto text_file_name = get_text_file_name();

				// ファイルサイズを取得します。
				auto file_size = (size_t)std::filesystem::file_size(text_file_name);

				// バイナリファイルストリームを開きます。
				std::ifstream ifs(text_file_name, std::ios::binary);

				// バッファを確保します。
				std::wstring mix_text(file_size / sizeof(WCHAR), L'\0');

				// ユニコードテキストをバイナリとして読み込みます。
				ifs.read((LPSTR)mix_text.data(), mix_text.size() * sizeof(WCHAR));

				// BOMを取り除きます。
				if (mix_text.starts_with(L"\xfeff")) mix_text = mix_text.substr(1);

				// ミックステキストをテキストオブジェクトに適用します。
				apply_mix_text(mix_text, current_scene_only, layer_front, layer_back);
			}
			catch (const std::exception& e)
			{
				hive.message_box(my::format(L"ファイルの読み込みに失敗しました\n{}", e.what()));

				return FALSE;
			}

			return TRUE;
		}

		//
		// テキストオブジェクトのテキストを一つにまとめてファイルに書き込みます。
		//
		virtual BOOL write_file() override
		{
			return write_file(FALSE, 0, 99);
		}

		//
		// 保存先テキストファイルを開きます。
		//
		virtual BOOL show_file() override
		{
			MY_TRACE_FUNC("");

			auto text_file_name = get_text_file_name();

			// テキストエディタのパスが指定されている場合は
			if (hive.text_editor_path.length())
			{
				// テキストファイルを指定されているテキストエディタで開きます。
				return shell_execute(_T("open"), hive.text_editor_path.c_str(), text_file_name.c_str());
			}
			// テキストエディタのパスが指定されていない場合は
			else
			{
				// テキストファイルをそのまま開きます。
				return shell_execute(_T("open"), text_file_name.c_str(), nullptr);
			}

			return TRUE;
		}

		//
		// 保存先テキストファイルをテキストオブジェクトに適用します。
		//
		virtual BOOL read_file() override
		{
			return read_file(FALSE, 0, 99);
		}

		//
		// テキストオブジェクトのテキストを一つにまとめてファイルに書き込みます。
		//
		virtual BOOL write_file_limited() override
		{
			return write_file(hive.current_scene_only, hive.layer_front - 1, hive.layer_back - 1);
		}

		//
		// 保存先テキストファイルをテキストオブジェクトに適用します。
		//
		virtual BOOL read_file_limited() override
		{
			return read_file(hive.current_scene_only, hive.layer_front - 1, hive.layer_back - 1);
		}
	} app_impl;
}
