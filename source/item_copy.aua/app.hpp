#pragma once

namespace apn::item_copy
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// exoファイルのパスを作成して返します。
		//
		inline static std::wstring create_exo_path()
		{
			SYSTEMTIME local_time = {};
			::GetLocalTime(&local_time);

			auto file_spec = std::format(
				L"{}{:04d}年{:02}月{:02}日{:02}時{:02}分{:02}.{:03}秒.exo",
				hive.prefix,
				local_time.wYear,
				local_time.wMonth,
				local_time.wDay,
				local_time.wHour,
				local_time.wMinute,
				local_time.wSecond,
				local_time.wMilliseconds);

			std::filesystem::path folder_path(magi.get_config_file_name(hive.c_name));

			return folder_path / file_spec;
		}

		//
		// 指定されたexoファイルをタイムラインに読み込みます。
		//
		BOOL read_file(const std::wstring& exo_file_name)
		{
			auto fp = magi.auin.get_filter_plugin(magi.fp, "拡張編集");
			auto editp = magi.exin.get_editp();
			auto frame = magi.fp->exfunc->get_frame(editp);
			auto layer = std::max(0, hive.insert_layer - 1);

			magi.exin.load_exo(my::hs(exo_file_name).c_str(), frame, layer, fp, editp);

			return TRUE;
		}

		//
		// タイムラインの選択アイテムを指定されたexoファイルに書き込みます。
		//
		BOOL write_file(const std::wstring& exo_file_name)
		{
			auto exedit = magi.exin.get_exedit();
			auto get_scene_size = (void (*)(int32_t scene_index, int32_t* w, int32_t* h, int32_t u4))(exedit + 0x2B980);
			auto is_scene_alpha_enabled = (BOOL (*)(int32_t scene_index))(exedit + 0x2BA00);
			auto filter_to_string = (LPSTR (*)(
				LPSTR buffer,
				ExEdit::Object* object,
				int32_t filter_index,
				uint32_t is_midpt))(exedit + 0x28830);
			auto string_buffer = *(LPSTR*)(exedit + 0x1A5328);
			auto fi = (AviUtl::FileInfo*)(exedit + 0x178E78);

			// 対象アイテムを取得します。
			std::unordered_set<int32_t> target_objects;
			{
				//
				// 指定されたオブジェクトを対象にします。
				// 中間点が存在する場合は中間点も対象にします。
				//
				const auto add_target = [&target_objects](int32_t object_index)
				{
					// オブジェクトのインデックスが無効の場合は何もしません。
					if (object_index < 0) return;

					// オブジェクトを取得します。
					auto object = magi.exin.get_object(object_index);
					if (!object) return;

					// 中間点リーダーを取得します。
					auto midpt_index = object->index_midpt_leader;

					// 中間点リーダーがコレクションに存在しない場合は
					if (!target_objects.contains(midpt_index))
					{
						// 中間点を走査します。
						while (midpt_index >= 0)
						{
							// 中間点をコレクションに追加します。
							target_objects.emplace(midpt_index);

							// 次の中間点を取得します。
							midpt_index = magi.exin.get_next_object_index(midpt_index);
						}
					}

					// オブジェクトをコレクションに追加します。
					target_objects.emplace(object_index);
				};

				// 選択アイテムが存在する場合は
				if (auto c = magi.exin.get_object_selection_count())
				{
					// すべての選択アイテムを対象にします。
					for (auto i = 0; i < c; i++)
						add_target(magi.exin.get_object_selection(i));
				}
				// 選択アイテムが存在しない場合は
				else
				{
					// カレントアイテムを対象にします。
					add_target(magi.exin.get_current_object_index());
				}

				// 対象が存在しない場合は何もしません。
				if (target_objects.empty()) return FALSE;
			}

			// 原点を取得します。
			int32_t frame_origin = INT_MAX;
			int32_t layer_origin = INT_MAX;
			{
				// ソート済みオブジェクトを走査します。
				auto c = magi.exin.get_sorted_object_count();
				for (auto i = 0; i < c; i++)
				{
					// ソート済みオブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(i);
					auto object_index = magi.exin.get_object_index(object);

					// 対象ではない場合は除外します。
					if (!target_objects.contains(object_index)) continue;

					// 最も左または上にあるアイテムの位置を取得します。
					frame_origin = std::min(frame_origin, object->frame_begin);
					layer_origin = std::min(layer_origin, object->layer_disp);
				}

				// 原点が取得できなかった場合は何もしません。
				if (frame_origin == INT_MAX || layer_origin == INT_MAX)
					return FALSE;
			}

			// バイナリファイル出力ストリームを開きます。
			std::ofstream ofs(exo_file_name, std::ios::binary);

			// 現在のシーンを取得します。
			auto current_scene_index = magi.exin.get_current_scene_index();
			auto current_scene = magi.exin.get_scene_setting(current_scene_index);

			ofs << "[exedit]\r\n";

			auto width = fi->w;
			auto height = fi->h;

			// 現在のシーンがルートではない場合は
			if (current_scene_index)
			{
				// シーンのサイズを取得します。
				int32_t scene_width = 0, scene_height = 0;
				get_scene_size(current_scene_index, &scene_width, &scene_height, 0);

				if (scene_width) width = scene_width;
				if (scene_height) height = scene_height;
			}

			ofs << std::format("width={}\r\n", width);
			ofs << std::format("height={}\r\n", height);
			ofs << std::format("rate={}\r\n", fi->video_rate);
			ofs << std::format("scale={}\r\n", fi->video_scale);
			ofs << std::format("length={}\r\n", *(int32_t*)(exedit + 0x14D3A0));
			ofs << std::format("audio_rate={}\r\n", fi->audio_rate);
			ofs << std::format("audio_ch={}\r\n", fi->audio_ch);
			if (is_scene_alpha_enabled(current_scene_index))
				ofs << "alpha=1\r\n";
			if (current_scene && current_scene->name)
				ofs << std::format("name={}\r\n", current_scene->name);

			// ソート済みオブジェクトを走査します。
			auto index = 0;
			auto c = magi.exin.get_sorted_object_count();
			for (auto i = 0; i < c; i++)
			{
				// ソート済みオブジェクトを取得します。
				auto object = magi.exin.get_sorted_object(i);
				auto object_index = magi.exin.get_object_index(object);

				// 対象ではない場合は除外します。
				if (!target_objects.contains(object_index)) continue;

				// 書き込み用のオブジェクトのインデックスです。
				auto write_object_index = index++;

				ofs << std::format("[{}]\r\n", write_object_index);
				ofs << std::format("start={}\r\n", object->frame_begin + 1 - frame_origin);
				ofs << std::format("end={}\r\n", object->frame_end + 1 - frame_origin);
				ofs << std::format("layer={}\r\n", object->layer_disp + 1 - layer_origin);

				if (object->group_belong)
					ofs << std::format("group={}\r\n", object->group_belong);

				auto flag = (uint32_t)object->flag;

				if (flag & 0x50000)
					ofs << std::format("overlay={}\r\n", 1);

				if (flag & 0x100)
					ofs << std::format("clipping={}\r\n", 1);

				if (flag & 0x200)
				{
					ofs << std::format("camera={}\r\n", 1);
				}
				else if (flag & 0x10000 && !(flag & 0x20000))
				{
					ofs << std::format("camera={}\r\n", 0);
				}

				if (flag & 0x20000)
					ofs << std::format("audio={}\r\n", 1);

				auto is_midpt = FALSE;

				if (object->index_midpt_leader >= 0 &&
					object->index_midpt_leader != object_index)
				{
					ofs << std::format("chain={}\r\n", 1);

					is_midpt = TRUE;
				}

				// すべてのフィルタを走査します。
				for (auto j = 0; j < object->MAX_FILTER; j++)
				{
					// フィルタを取得します。
					auto filter = magi.exin.get_filter(object, j);
					if (!filter) break;

					// 書き込み用のフィルタのインデックスです。
					auto write_filter_index = j;

					ofs << std::format("[{}.{}]\r\n", write_object_index, write_filter_index);

					const auto write_filter = [&]()
					{
						auto next_string_buffer = (*filter_to_string)(string_buffer, object, j, is_midpt);

						ofs << string_buffer;
					};

					// テキストを空にしてコピーする場合は
					if (hive.use_empty_text && !is_midpt && object->filter_param[j].id == 3)
					{
						// テキストオブジェクトの拡張データを取得します。
						auto exdata = (ExEdit::Exdata::efText*)magi.exin.get_exdata(object, j);

						// テキストを取得しておきます。
						decltype(exdata->text) text;
						memcpy(text, exdata->text, sizeof(text));

						// 一時的にテキストを空にします。
						memset(exdata->text, 0, sizeof(text));

						// フィルタを書き込みます。
						write_filter();

						// テキストを元に戻します。
						memcpy(exdata->text, text, sizeof(text));
					}
					// テキストを空にしない場合は
					else
					{
						// そのままフィルタを書き込みます。
						write_filter();
					}
				}
			}

			return TRUE;
		}

		//
		// 選択されているexoファイルをタイムラインに貼り付けます。
		//
		virtual BOOL paste_item() override
		{
			// 選択ファイルを取得します。
			auto selection = addin_dialog.get_selection();

			// 選択ファイルが空の場合は何もしません。
			if (selection.empty()) return FALSE;

			// 選択されている最初のexoファイルをタイムラインに適用します。
			return read_file(selection.front());
		}

		//
		// タイムラインの選択アイテムをexoファイルに書き込みます。
		//
		virtual BOOL copy_item() override
		{
			// exoファイルのパスを作成します。
			auto exo_path = create_exo_path();

			// タイムラインの選択アイテムを自動生成したexoファイルに書き込みます。
			if (!write_file(exo_path)) return FALSE;

			// 生成したファイルを選択状態にします。
			addin_dialog.set_selection(exo_path);

			return TRUE;
		}

		//
		// ドロップテキストをファイルから読み込みます。
		//
		virtual BOOL read_file() override
		{
			auto file_name = get_open_file_name(
				hive.main_window,
				L"exoファイルを選択",
				L"exoファイル (*.exo)\0*.exo\0"
				L"すべてのファイル (*.*)\0*.*\0",
				hive.exo_file_name.c_str());
			if (file_name.empty()) return FALSE;

			hive.exo_file_name = file_name;

			return read_file(file_name);
		}

		//
		// 選択アイテムををexoファイルに書き込みます。
		//
		virtual BOOL write_file() override
		{
			auto file_name = get_save_file_name(
				hive.main_window,
				L"exoファイルを選択",
				L"exoファイル (*.exo)\0*.exo\0"
				L"すべてのファイル (*.*)\0*.*\0",
				hive.exo_file_name.c_str(),
				L"exo");
			if (file_name.empty()) return FALSE;

			hive.exo_file_name = file_name;

			return write_file(file_name);
		}
	} app_impl;
}
