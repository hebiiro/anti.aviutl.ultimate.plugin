#pragma once

namespace apn::image_export::executor
{
	//
	// このクラスは対象レイヤーのアイテムを画像ファイルとしてエクスポートします。
	//
	struct export_layer_t : export_t
	{
		//
		// このクラスは対象アイテム情報です。
		//
		struct target_t {
			ExEdit::Object* object;
			ExEdit::Object* frame_object;
			ExEdit::Object* name_object;
		};

		//
		// 対象レイヤーを返します。
		//
		inline static int32_t get_target_layer()
		{
			auto layer = wcstol(hive.target_layer.c_str(), nullptr, 0) - 1;
			if (layer >= 0) return layer;

			auto object_index = magi.exin.get_current_object_index();
			if (object_index < 0) throw L"選択アイテムが存在しません";

			auto object = magi.exin.get_object(object_index);

			return object->layer_set;
		}

		//
		// フレームレイヤーを返します。
		//
		inline static int32_t get_frame_layer(int32_t target_layer)
		{
			auto layer = wcstol(hive.frame_layer.c_str(), nullptr, 0) - 1;
			if (layer >= 0) return layer;

			return target_layer;
		}

		//
		// 名前レイヤーを返します。
		//
		inline static int32_t get_name_layer(int32_t target_layer)
		{
			auto layer = wcstol(hive.name_layer.c_str(), nullptr, 0) - 1;
			if (layer >= 0) return layer;

			return target_layer;
		}

		//
		// フレームオブジェクトに対応するアイテムを返します。
		//
		inline static ExEdit::Object* get_object(ExEdit::Object* frame_object, int32_t layer)
		{
			// アイテムの数を取得します。
			auto c = magi.exin.get_sorted_object_count();

			// アイテムを走査します。
			for (decltype(c) i = 0; i < c; i++)
			{
				// アイテムを取得します。
				auto object = magi.exin.get_sorted_object(i);

				// アイテムが対象レイヤーに存在しない場合は除外します。
				if (object->layer_set != layer) continue;

				// アイテムがフレームアイテムの開始位置に存在する場合は
				if (frame_object->frame_begin >= object->frame_begin &&
					frame_object->frame_begin <= object->frame_end)
				{
					// アイテムを返します。
					return object;
				}
			}

			// アイテムが見つからなかった場合はnullptrを返します。
			return nullptr;
		}

		//
		// オブジェクトからステムを取得して返します。
		//
		inline static std::wstring get_stem(const target_t& target, int32_t index)
		{
			// 結果を格納する変数です。
			auto result = std::wstring {};

			// アイテム名を取得します。
			auto name = my::ws(target.name_object->dispname);

			//
			// この関数は正規表現を使用して
			// 文字列から不要な部分を削除して返します。
			//
			const auto erase = [](const std::wstring& name, const std::wstring& pattern)
			{
				return std::regex_replace(name, std::wregex(pattern), L"",  std::regex_constants::format_first_only);
			};

			name = erase(name, LR"***(\<\?s=\[==\[\s*)***");
			name = erase(name, LR"***(\s*\]==\];.*)***");

			// アイテム名を走査します。
			for (auto ch : name)
			{
				switch (::PathGetCharTypeW(ch))
				{
				case GCT_INVALID:
				case GCT_SEPARATOR:
				case GCT_WILD:
					continue;
				}

				result += ch;
			}

			if (result.length())
				return my::format(L"{/}_{/}", index, result);
			else
				return my::format(L"{/}", index);
		}

		//
		// exoファイルのヘッダー部分をストリームに書き込みます。
		//
		inline static BOOL write_exo_header(std::ofstream& ofs)
		{
			// 必要な変数を取得しておきます。
			auto fp = magi.fp;
			auto editp = magi.exin.get_editp();

			// ファイル情報を取得します。
			auto fi = AviUtl::FileInfo {};
			fp->exfunc->get_file_info(editp, &fi);

			// フレームの総数を取得します。
			auto nb_frames = fp->exfunc->get_frame_n(editp);

			ofs << "[exedit]\r\n";

			auto width = editp->w1;
			auto height = editp->h1;

			ofs << my::format("width={/}\r\n", width);
			ofs << my::format("height={/}\r\n", height);
			ofs << my::format("rate={/}\r\n", fi.video_rate);
			ofs << my::format("scale={/}\r\n", fi.video_scale);
			ofs << my::format("length={/}\r\n", nb_frames);
			ofs << my::format("audio_rate={/}\r\n", fi.audio_rate);
			ofs << my::format("audio_ch={/}\r\n", fi.audio_ch);

			return TRUE;
		}

		//
		// 指定されたアイテムをストリームに書き込みます。
		//
		inline static BOOL write_object(std::ofstream& ofs,
			ExEdit::Object* object, int32_t object_index, int32_t write_object_index,
			const std::wstring& file_name, int32_t frame_begin, int32_t frame_end, double x, double y)
		{
			ofs << my::format("[{/}]\r\n", write_object_index);
			ofs << my::format("start={/}\r\n", frame_begin + 1);
			ofs << my::format("end={/}\r\n", frame_end + 1);
			ofs << my::format("layer={/}\r\n", 1);
			ofs << "overlay=1\r\n";
			ofs << "camera=0\r\n";

			ofs << my::format("[{/}.{/}]\r\n", write_object_index, 0);
			ofs << "_name=画像ファイル\r\n";
			ofs << my::format("file={/}\r\n", file_name);

			ofs << my::format("[{/}.{/}]\r\n", write_object_index, 1);
			ofs << "_name=標準描画\r\n";
			ofs << my::format("X={/.1f}\r\n", x);
			ofs << my::format("Y={/.1f}\r\n", y);
			ofs << "Z=0.0\r\n";
			ofs << "拡大率=100.00\r\n";
			ofs << "透明度=0.0\r\n";
			ofs << "回転=0.00\r\n";
			ofs << "blend=0\r\n";

			return TRUE;
		}

		//
		// 対象レイヤーのアイテムを画像ファイルとしてエクスポートします。
		//
		BOOL export_image(const std::wstring& file_name, BOOL has_alpha)
		{
			MY_TRACE_FUNC("{/}, {/}, {/}", file_name, has_alpha);

			// 必要な変数を取得しておきます。
			auto fp = magi.fp;
			auto editp = magi.exin.get_editp();

			// 現在フレームを取得します。
			auto current_frame = fp->exfunc->get_frame(editp);

			// スコープ終了時の処理です。
			my::scope_exit scope_exit([&]()
			{
				// 現在フレームを元に戻します。
				fp->exfunc->set_frame(editp, current_frame);
			});

			try
			{
				// 出力先パスを取得します。
				auto path = std::filesystem::path(file_name);

				// 出力先パスが無効の場合は何もしません。
				if (path.empty()) return FALSE;

				// 出力先フォルダのパスを取得します。
				auto directory = path.parent_path();

				// 出力先フォルダを作成します。
				std::filesystem::create_directories(directory);

				// 出力ステムを取得します。
				auto stem = path.stem().wstring();

				// 出力拡張子を取得します。
				auto extension = my::to_lower(path.extension());

				if (extension.empty() && stem.starts_with(L'.'))
					extension = stem, stem.clear();

				// 出力拡張子からエンコーダを取得します。
				auto encoder = get_encoder(extension);

				// エンコーダを取得できなかった場合は
				if (encoder == GUID_NULL)
				{
					// メッセージボックスを表示します。
					hive.message_box(L"拡張子が無効です");

					return FALSE;
				}

				// エンコードパラメータを作成します。
				auto jpg_quality = (ULONG)hive.quality;
				auto jpg_encoder_parameters = EncoderParameters {};
				jpg_encoder_parameters.Count = 1;
				jpg_encoder_parameters.Parameter[0].Guid = EncoderQuality;
				jpg_encoder_parameters.Parameter[0].Type = EncoderParameterValueTypeLong;
				jpg_encoder_parameters.Parameter[0].NumberOfValues = 1;
				jpg_encoder_parameters.Parameter[0].Value = &jpg_quality;

				auto encoder_parameters = &jpg_encoder_parameters;
				if (extension != L".jpg") encoder_parameters = nullptr;

				// 対象レイヤーを取得します。
				auto target_layer = get_target_layer();
				if (target_layer < 0) throw L"対象レイヤーが無効です";

				// フレームレイヤーを取得します。
				auto frame_layer = get_frame_layer(target_layer);
				if (frame_layer < 0) throw L"フレームレイヤーが無効です";

				// 名前レイヤーを取得します。
				auto name_layer = get_name_layer(target_layer);
				if (name_layer < 0) throw L"名前レイヤーが無効です";

				// 選択フレーム番号を取得します。
				auto start_frame = int32_t {}, end_frame = int32_t {};
				fp->exfunc->get_select_frame(editp, &start_frame, &end_frame);

				// 対象アイテム情報のコレクションです。
				std::vector<target_t> targets;

				// まずフレームアイテムを列挙します。
				std::vector<ExEdit::Object*> frame_objects;
				{
					// アイテムの数を取得します。
					auto c = magi.exin.get_sorted_object_count();

					// アイテムを走査します。
					for (decltype(c) i = 0; i < c; i++)
					{
						// アイテムを取得します。
						auto object = magi.exin.get_sorted_object(i);

						// アイテムが選択範囲外の場合は除外します。
						if (object->frame_begin < start_frame ||
							object->frame_begin > end_frame) continue;

						// アイテムがフレームレイヤーに存在しない場合は除外します。
						if (object->layer_set != frame_layer) continue;

						// コレクションに追加します。
						frame_objects.emplace_back(object);
					}
				}

				// フレームアイテムを走査します。
				for (auto frame_object : frame_objects)
				{
					// 対象アイテムを取得します。
					auto object = get_object(frame_object, target_layer);
					if (!object) continue;

					// 名前アイテムを取得します。
					auto name_object = get_object(frame_object, name_layer);
					if (!name_object) name_object = object;

					// コレクションに追加します。
					targets.emplace_back(object, frame_object, name_object);
				}

				{
					// メッセージボックスに表示する文字列です。
					auto s = my::format(
						L"\"{/}\"\n"
						L"フォルダ内に\"{/}.exo\"と{/}個の画像ファイルを作成します\n"
						L"続行しますか？"
						, directory, stem, targets.size());

					// 処理を実行するかユーザーに確認します。
					if (IDYES != hive.message_box(s, hive.main_window, MB_YESNO))
						return FALSE;
				}

				// exoファイルのパスを取得します。
				auto exo_path = directory / (stem + L".exo");

				// バイナリファイル出力ストリームを開きます。
				std::ofstream ofs(exo_path, std::ios::binary);

				// exoヘッダーをストリームに書き込みます。
				write_exo_header(ofs);

				// 対象アイテム情報を走査します。
				auto c = targets.size();
				for (decltype(c) i = 0; i < c; i++)
				{
					// 対象アイテム情報を取得します。
					const auto& target = targets[i];

					// 対象アイテムを取得します。
					auto object = target.object;

					// 対象アイテムのインデックスを取得します。
					auto object_index = magi.exin.get_object_index(object);

					// フレームアイテムを取得します。
					auto frame_object = target.frame_object;

					// 現在フレームを変更します。
					fp->exfunc->set_frame(editp, frame_object->frame_begin);

					// 選択アイテムを変更します。
					magi.exin.set_current_object(object_index);

					// アイテムの画像データを作成します。
					create_image_internal(has_alpha, object_index);

					// 出力ファイル名を作成します。
					auto file_name = directory / (stem + L"_" + get_stem(target, i) + extension);

					// ビットマップを出力先パスに保存します。
					auto status = bitmap->Save(file_name.c_str(), &encoder, encoder_parameters);
					if (status != S_OK) throw L"ファイルの保存に失敗しました";

					// アイテムをストリームに書き込みます。
					write_object(ofs, object, object_index, i, file_name,
						frame_object->frame_begin, frame_object->frame_end,
						x + (width - editp->w1) / 2.0, y + (height - editp->h1) / 2.0);

					// アドインウィンドウのウィンドウ名を使用して進捗を出力します。
					::SetWindowTextW(hive.main_window,
						my::format(L"{/} ({/}/{/})", hive.c_display_name, i + 1, c).c_str());
				}
			}
			// 例外が発生した場合は
			catch (const std::exception& error)
			{
				// メッセージボックスを表示します。
				hive.message_box(my::ws(error.what()));

				return FALSE;
			}
			// 例外が発生した場合は
			catch (LPCWSTR what)
			{
				// メッセージボックスを表示します。
				hive.message_box(what);

				return FALSE;
			}

			return TRUE;
		}
	};
}
