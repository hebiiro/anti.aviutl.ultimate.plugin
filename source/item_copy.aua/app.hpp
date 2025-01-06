#pragma once

namespace apn::item_copy
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// オブジェクト名を正規化して返します。
		//
		inline static std::wstring normalize_object_name(ExEdit::Object* object)
		{
			switch (object->filter_param[0].id)
			{
			case 0: return L"動画ファイル";
			case 1: return L"画像ファイル";
			case 2: return L"音声ファイル";
			case 3: return L"テキスト";
			case 4: return L"図形";
			case 5: return L"フレームバッファ";
			case 6: return L"音声波形表示";
			case 7: return L"シーン";
			case 8: return L"シーン(音声)";
			case 9: return L"直前オブジェクト";
			case 10: return L"標準描画";
			case 11: return L"拡張描画";
			case 12: return L"標準再生";
			case 13: return L"パーティクル出力";
			case 14: return L"シーンチェンジ";
			case 15: return L"色調補正";
			case 16: return L"色調補正";
			case 17: return L"クリッピング";
			case 18: return L"ぼかし";
			case 19: return L"境界ぼかし";
			case 20: return L"ぼかし";
			case 21: return L"モザイク";
			case 22: return L"モザイク";
			case 23: return L"発光";
			case 24: return L"発光";
			case 25: return L"閃光";
			case 26: return L"拡散光";
			case 27: return L"拡散光";
			case 28: return L"グロー";
			case 29: return L"グロー";
			case 30: return L"クロマキー";
			case 31: return L"カラーキー";
			case 32: return L"ルミナンスキー";
			case 33: return L"ライト";
			case 34: return L"シャドー";
			case 35: return L"縁取り";
			case 36: return L"凸エッジ";
			case 37: return L"エッジ抽出";
			case 38: return L"シャープ";
			case 39: return L"フェード";
			case 40: return L"ワイプ";
			case 41: return L"マスク";
			case 42: return L"斜めクリッピング";
			case 43: return L"放射ブラー";
			case 44: return L"放射ブラー";
			case 45: return L"方向ブラー";
			case 46: return L"方向ブラー";
			case 47: return L"レンズブラー";
			case 48: return L"レンズブラー";
			case 49: return L"モーションブラー";
			case 50: return L"モーションブラー";
			case 51: return L"座標";
			case 52: return L"拡大率";
			case 53: return L"透明度";
			case 54: return L"回転";
			case 55: return L"領域拡張";
			case 56: return L"リサイズ";
			case 57: return L"ローテーション";
			case 58: return L"振動";
			case 59: return L"振動";
			case 60: return L"反転";
			case 61: return L"反転";
			case 62: return L"ミラー";
			case 63: return L"ラスター";
			case 64: return L"ラスター";
			case 65: return L"波紋";
			case 66: return L"画像ループ";
			case 67: return L"画像ループ";
			case 68: return L"極座標変換";
			case 69: return L"ディスプレイスメントマップ";
			case 70: return L"ノイズ";
			case 71: return L"色ずれ";
			case 72: return L"色ずれ";
			case 73: return L"単色化";
			case 74: return L"単色化";
			case 75: return L"グラデーション";
			case 76: return L"拡張色設定";
			case 77: return L"拡張色設定";
			case 78: return L"特定色域変換";
			case 79: return L"アニメーション効果";
			case 80: return L"カスタムオブジェクト";
			case 81: return L"スクリプト制御";
			case 82: return L"動画ファイル合成";
			case 83: return L"画像ファイル合成";
			case 84: return L"インターレース解除";
			case 85: return L"カメラ制御オプション";
			case 86: return L"オフスクリーン描画";
			case 87: return L"オブジェクト分割";
			case 88: return L"部分フィルタ";
			case 89: return L"音量フェード";
			case 90: return L"音声ディレイ";
			case 91: return L"音声ディレイ";
			case 92: return L"モノラル化";
			case 93: return L"時間制御";
			case 94: return L"グループ制御";
			case 95: return L"カメラ制御";
			case 96: return L"カメラ制御(拡張描画)";
			case 97: return L"カメラ効果";
			case 98: return L"シャドー(カメラ制御)";
			case 99: return L"スクリプト(カメラ制御)";
			}

			return L"不明";
		}

		//
		// 出力ファイルのパスを作成して返します。
		//
		inline static std::wstring create_output_path(const std::wstring& extension)
		{
			SYSTEMTIME local_time = {};
			::GetLocalTime(&local_time);

			auto file_spec = std::format(
				L"{}{:04d}年{:02d}月{:02d}日{:02d}時{:02d}分{:02d}.{:03d}秒.{}",
				hive.prefix,
				local_time.wYear,
				local_time.wMonth,
				local_time.wDay,
				local_time.wHour,
				local_time.wMinute,
				local_time.wSecond,
				local_time.wMilliseconds,
				extension);

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
		// 対象アイテムのコレクションを返します。
		//
		std::unordered_set<int32_t> get_target_objects()
		{
			// 対象アイテムのコレクションです。
			std::unordered_set<int32_t> target_objects;

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

			return target_objects;
		}

		//
		// exoファイルのヘッダー部分をストリームに書き込みます。
		//
		BOOL write_exo_header(std::ofstream& ofs)
		{
			auto exedit = magi.exin.get_exedit();
			auto get_scene_size = (void (*)(int32_t scene_index, int32_t* w, int32_t* h, int32_t u4))(exedit + 0x2B980);
			auto is_scene_alpha_enabled = (BOOL (*)(int32_t scene_index))(exedit + 0x2BA00);
			auto fi = (AviUtl::FileInfo*)(exedit + 0x178E78);

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

			return TRUE;
		}

		//
		// 指定されたオブジェクトをストリームに書き込みます。
		//
		BOOL write_object(std::ofstream& ofs, ExEdit::Object* object, int32_t object_index,
			int32_t write_object_index, int32_t frame_origin, int32_t layer_origin)
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

			return TRUE;
		}

		//
		// タイムラインの選択アイテムをアイテム毎に
		// exa形式でファイルに書き込みます。
		//
		BOOL write_item_exa(const std::filesystem::path& base_file_name)
		{
			auto exedit = magi.exin.get_exedit();
			auto write_exa = (BOOL (*)(int32_t object_index, int32_t filter_index, LPCSTR file_name))(exedit + 0x28CA0);

			// 対象アイテムを取得します。
			auto target_objects = get_target_objects();

			// 対象が存在しない場合は何もしません。
			if (target_objects.empty()) return FALSE;

			auto dir = base_file_name.parent_path();
			auto stem = base_file_name.stem().wstring();
			auto extension = base_file_name.extension().wstring();

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

				// 中間点の場合は除外します。
				if (object->index_midpt_leader >= 0 &&
					object->index_midpt_leader != object_index)
				{
					continue;
				}

				// 書き込み先ファイル名を取得します。
				auto file_name = dir / std::format(L"{}_{}_{}{}",
					stem, ++index, normalize_object_name(object), extension);

				// アイテムをファイルに書き込みます。
				write_exa(object_index, -2, file_name.string().c_str());
			}

			return TRUE;
		}

		//
		// タイムラインの選択アイテムをアイテム毎に
		// exo形式でファイルに書き込みます。
		//
		BOOL write_item_exo(const std::filesystem::path& base_file_name)
		{
			// 対象アイテムを取得します。
			auto target_objects = get_target_objects();

			// 対象が存在しない場合は何もしません。
			if (target_objects.empty()) return FALSE;

			auto dir = base_file_name.parent_path();
			auto stem = base_file_name.stem().wstring();
			auto extension = base_file_name.extension().wstring();

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

				// 中間点リーダーが無効の場合は
				if (object->index_midpt_leader < 0)
				{
					// 書き込み先ファイル名を取得します。
					auto file_name = dir / std::format(L"{}_{}_{}{}",
						stem, ++index, normalize_object_name(object), extension);

					// バイナリファイル出力ストリームを開きます。
					std::ofstream ofs(file_name, std::ios::binary);

					// ヘッダーをストリームに書き込みます。
					write_exo_header(ofs);

					// オブジェクトをストリームに書き込みます。
					write_object(ofs, object, object_index, 0, object->frame_begin, object->layer_disp);
				}
				// 中間点リーダーの場合は
				else if (object->index_midpt_leader == object_index)
				{
					// 書き込み先ファイル名を取得します。
					auto file_name = dir / std::format(L"{}_{}_{}{}",
						stem, ++index, normalize_object_name(object), extension);

					// バイナリファイル出力ストリームを開きます。
					std::ofstream ofs(file_name, std::ios::binary);

					// ヘッダーをストリームに書き込みます。
					write_exo_header(ofs);

					auto frame_origin = object->frame_begin;
					auto layer_origin = object->layer_disp;
					auto write_object_index = 0;

					// 中間点を走査します。
					for (auto midpt = object->index_midpt_leader;
						midpt >= 0; midpt = magi.exin.get_next_object_index(midpt))
					{
						auto midpt_object = magi.exin.get_object(midpt);

						// 中間点オブジェクトをストリームに書き込みます。
						write_object(ofs, midpt_object, midpt,
							write_object_index++, frame_origin, layer_origin);
					}
				}
			}

			return TRUE;
		}

		//
		// タイムラインの選択アイテムを指定されたexoファイルに書き込みます。
		//
		BOOL write_file(const std::wstring& exo_file_name)
		{
			// 対象アイテムを取得します。
			auto target_objects = get_target_objects();

			// 対象が存在しない場合は何もしません。
			if (target_objects.empty()) return FALSE;

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

			// ヘッダーをストリームに書き込みます。
			write_exo_header(ofs);

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

				// オブジェクトをストリームに書き込みます。
				write_object(ofs, object, object_index, write_object_index, frame_origin, layer_origin);
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
			switch (hive.file_split_mode)
			{
			case hive.c_file_split_mode.c_none:
				{
					// 出力ファイルのパスを作成します。
					auto path = create_output_path(L"exo");

					// タイムラインの選択アイテムを出力ファイルに書き込みます。
					if (!write_file(path)) return FALSE;

					// 生成したファイルを選択状態にします。
					addin_dialog.set_selection(path);

					return TRUE;
				}
			case hive.c_file_split_mode.c_item_exo:
				{
					// 出力ファイルのパスを作成します。
					auto path = create_output_path(L"exo");

					// タイムラインの選択アイテムを出力ファイルに書き込みます。
					if (!write_item_exo(path)) return FALSE;

					return TRUE;
				}
			case hive.c_file_split_mode.c_item_exa:
				{
					// 出力ファイルのパスを作成します。
					auto path = create_output_path(L"exa");

					// タイムラインの選択アイテムを出力ファイルに書き込みます。
					if (!write_item_exa(path)) return FALSE;

					return TRUE;
				}
			}

			return FALSE;
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
