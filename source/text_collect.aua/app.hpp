#pragma once

namespace apn::text_collect
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// 保存先テキストファイルのファイル名を返します。
		//
		inline static std::wstring get_text_file_name()
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
		inline static ExEdit::Exdata::efText* get_text_exdata(ExEdit::Object* object)
		{
			// オブジェクトが無効の場合は除外します。
			if (!(object->flag & ExEdit::Object::Flag::Exist))
				return {};

			// 中間点が存在する場合は
			if (object->index_midpt_leader >= 0)
			{
				// オブジェクトのインデックスを取得します。
				auto object_index = magi.exin.get_object_index(object);

				// 中間点リーダーではない場合は除外します。
				if (object_index != object->index_midpt_leader)
					return {};
			}

			// テキストオブジェクトではない場合は除外します。
			if (object->filter_param[0].id != 3)
				return {};

			// テキストオブジェクトの拡張データを取得します。
			return (ExEdit::Exdata::efText*)magi.exin.get_exdata(object, 0);
		}

		//
		// テキストオブジェクトのテキストを一括して返します。
		//
		inline static std::wstring get_mix_text(int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("{/}, {/}", layer_front, layer_back);

			// 末尾レイヤー番号を調整します。
			if (layer_back < layer_front) layer_back = layer_front;

			//
			// オブジェクトからミックステキストパートを取得します。
			//
			auto get_mix_text_part = [&](int32_t sorted_index, ExEdit::Object* object) -> std::wstring
			{
				// レイヤー番号が範囲外の場合は除外します。
				if (object->layer_set < layer_front || object->layer_set > layer_back)
					return {};

				// テキストオブジェクトの拡張データを取得します。
				auto exdata = get_text_exdata(object);
				if (!exdata) return {};

				// ヘッダーテキストを構築します。
				auto header_text = my::format(
					LR"(/[/{ "sorted_index":{/}, "index":{/}, "scene_set":{/}, "layer_set":{/}, "frame_begin":{/}, "frame_end":{/} }/]/)",
					sorted_index,
					magi.exin.get_object_index(object),
					object->scene_set,
					object->layer_set,
					object->frame_begin,
					object->frame_end);

				// ヘッダーテキストと本体テキストを結合して返します。
				return header_text + L"\r\n" + exdata->text;
			};

			// ミックステキストです。
			// 先頭にUTF-16LEのBOMを付与します。
			std::wstring mix_text = L"\xfeff";

			// ソート済みオブジェクトを走査します。
			auto c = magi.exin.get_sorted_object_count();
			for (auto i = 0; i < c; i++)
			{
				// ソート済みオブジェクトを取得します。
				auto object = magi.exin.get_sorted_object(i);
				if (!object) continue;

				// パートを取得します。
				auto part_text = get_mix_text_part(i, object);
				if (part_text.length())
				{
					// 先頭行以外の場合は改行を追加します。
					if (i) mix_text += L"\r\n";

					// パート追加します。
					mix_text += part_text;
				}
			}

			return mix_text;
		}

		//
		// テキストオブジェクトのテキストを一つにまとめてファイルに書き込みます。
		//
		BOOL write_file(int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("{/}, {/}", layer_front, layer_back);

			try
			{
				// 保存先テキストファイルのファイル名を取得します。
				auto text_file_name = get_text_file_name();

				// テキストオブジェクトのテキストを一つにまとめます。
				auto mix_text = get_mix_text(layer_front, layer_back);

				// バイナリファイルストリームを開きます。
				std::ofstream ofs(text_file_name, std::ios::binary);

				// ユニコードテキストをバイナリとして書き込みます。
				ofs.write((LPCSTR)mix_text.data(), mix_text.size() * sizeof(WCHAR));
			}
			catch (const std::exception& e)
			{
				hive.message_box(my::format(L"ファイルの書き込みに失敗しました\n{/}", e.what()));

				return FALSE;
			}

			return TRUE;
		}

		//
		// ミックステキストをテキストオブジェクトに適用します。
		//
		BOOL apply_mix_text(const std::wstring& mix_text, int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("{/}, {/}", layer_front, layer_back);

			// 末尾レイヤー番号を調整します。
			if (layer_back < layer_front) layer_back = layer_front;

			const std::wstring prefix = L"/[/";
			const std::wstring suffix = L"/]/\r";

			// オブジェクトの数を取得しておきます。
			const auto c = magi.exin.get_sorted_object_count();
			const auto current_scene_set = magi.exin.get_current_scene_index();

			const auto get_text_exdata_external =
				[&](int32_t sorted_index) -> ExEdit::Exdata::efText*
			{
				// オブジェクトのインデックスが無効の場合は何もしません。
				if (sorted_index < 0 || sorted_index >= c) return nullptr;

				// オブジェクトを取得します。
				auto object = magi.exin.get_sorted_object(sorted_index);

				// シーンのインデックスが無効の場合は何もしません。
				if (object->scene_set < 0 || object->scene_set >= 50) return nullptr;

				// カレントシーン外のオブジェクトの場合は除外します。
				if (object->scene_set != current_scene_set) return nullptr;

				// レイヤー番号が範囲外の場合は除外します。
				if (object->layer_set < layer_front || object->layer_set > layer_back)
					return nullptr;

				// テキストオブジェクトの拡張データを取得します。
				return get_text_exdata(object);
			};

			//
			// この構造体は編集対象のオブジェクトのです。
			//
			struct Node {
				int32_t sorted_index;
				std::wstring text;
			};

			// 編集に失敗した場合はTRUEになります。
			BOOL failed = FALSE;

			// 現在の編集対象オブジェクトです。
			Node* current_node = nullptr;

			// 編集対象オブジェクトのコレクションです。
			std::vector<Node> objects;

			// 文字列ストリームを作成します。
			std::wstringstream stream(mix_text);

			// 文字列を行毎に読み込みます。
			std::wstring line;
			while (std::getline(stream, line))
			{
				// jsonの解析などで例外が発生するかもしれないので
				// １行毎にtryを使用しています。
				try
				{
					// ヘッダー行の場合は
					if (line.starts_with(prefix) && line.ends_with(suffix))
					{
						// ヘッダーのプレフィックスとサフィックスを取り除いた部分を取得します。
						auto header_text = line.substr(prefix.length(), line.length() - prefix.length() - suffix.length());

						// ヘッダーテキストをjsonに変換します。
						auto header_node = n_json::parse(header_text);

						// ソート済みオブジェクトのインデックスを取得します。
						int32_t sorted_index = -1;
						read_int(header_node, "sorted_index", sorted_index);

						// テキストオブジェクトの拡張データを取得します。
						auto exdata = get_text_exdata_external(sorted_index);

						// 拡張データを取得できた場合は
						if (exdata)
						{
							// 現在の編集対象オブジェクトが有効の場合は
							if (current_node && current_node->text.ends_with(L"\r\n"))
							{
								// 余計な改行を削除します。
								current_node->text.resize(current_node->text.size() - 2);
							}

							// 編集対象オブジェクトを追加します。
							objects.emplace_back(sorted_index);

							// 追加した編集対象オブジェクトを現在の編集対象オブジェクトとします。
							current_node = &objects.back();
						}
						// 拡張データを取得できなかった場合は
						else
						{
							// 編集に失敗したのでフラグを立てます。
							failed = TRUE;

							// 現在の編集対象オブジェクトをクリアします。
							current_node = nullptr;
						}
					}
					// 本体行の場合は
					else if (current_node && line.length())
					{
						// 末尾が改行の場合は正しく復元します。
						if (line.back() == L'\r') line.push_back(L'\n');

						// 現在の編集対象オブジェクトのテキストに追加します。
						current_node->text += line;
					}
				}
				catch (...)
				{
				}
			}

			// 編集に失敗した場合は
			if (failed)
			{
				// 処理を続けるかユーザーに問い合わせます。
				if (IDOK != hive.message_box(
					L"変更を適用できないオブジェクトが存在します\n処理を続けますか？",
					nullptr, MB_OKCANCEL | MB_ICONWARNING))
				{
					return FALSE;
				}
			}

			// 最初のオブジェクトの場合はTRUEになります。
			BOOL first_object = TRUE;

			// 変更したオブジェクトの数です。
			int32_t modified_count = 0;

			// 変更しなかったオブジェクトの数です。
			int32_t unmodified_count = 0;

			// 編集対象オブジェクトを走査します。
			for (const auto& node : objects)
			{
				// オブジェクトを取得します。
				auto object = magi.exin.get_sorted_object(node.sorted_index);

				// 最初のオブジェクトの場合は
				if (first_object)
				{
					// フラグを消去します。
					first_object = FALSE;

					// 現在のシーンを変更します。
//					magi.exin.set_scene(object->scene_set, nullptr, nullptr);

					// 新しいアンドゥを作成します。
					magi.exin.push_undo();
				}

				// オブジェクトのインデックスを取得します。
				auto object_index = magi.exin.get_object_index(object);

				// オブジェクトをアンドゥの対象にします。
				magi.exin.create_undo(object_index, 0x00);

				// テキストオブジェクトの拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efText*)magi.exin.get_exdata(object, 0);

				// テキストが異なる場合は
				if (node.text != exdata->text)
				{
					// テキストオブジェクトのテキストを書き換えます。
					wcscpy_s(exdata->text, std::size(exdata->text), node.text.c_str());

					// テキストオブジェクトの表示名を書き換えます。
					::WideCharToMultiByte(CP_ACP, 0, exdata->text, -1,
						object->dispname, std::size(object->dispname), nullptr, nullptr);

					// 変更したオブジェクトの数を増やします。
					modified_count++;
				}
				// テキストが同一の場合は
				else
				{
					// 変更しなかったオブジェクトの数を増やします。
					unmodified_count++;
				}
			}

			// 拡張編集を再描画します。
			magi.exin.update_object_table();
			magi.exin.redraw_timeline();
			magi.exin.set_current_object(magi.exin.get_current_object_index());

			// AviUtlを再描画します。
			magi.redraw();

			// 処理結果をユーザーに通知します。
			hive.message_box(my::format(
				L"{/}個のテキストを変更しました\n"
				L"{/}個のテキストは変更しませんでした",
				modified_count, unmodified_count));

			return TRUE;
		}

		//
		// 保存先テキストファイルをテキストオブジェクトに適用します。
		//
		BOOL read_file(int32_t layer_front, int32_t layer_back)
		{
			MY_TRACE_FUNC("{/}, {/}", layer_front, layer_back);

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
				apply_mix_text(mix_text, layer_front, layer_back);
			}
			catch (const std::exception& e)
			{
				hive.message_box(my::format(L"ファイルの読み込みに失敗しました\n{/}", e.what()));

				return FALSE;
			}

			return TRUE;
		}

		//
		// テキストオブジェクトのテキストを一つにまとめてファイルに書き込みます。
		//
		virtual BOOL write_file() override
		{
			return write_file(hive.layer_front - 1, hive.layer_back - 1);
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
			return read_file(hive.layer_front - 1, hive.layer_back - 1);
		}
	} app_impl;
}
