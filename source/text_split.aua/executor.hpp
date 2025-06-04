#pragma once

namespace apn::text_split
{
	//
	// このクラスはテキストアイテムの分解処理を実行します。
	//
	struct Executor
	{
		//
		// このクラスは改行を含まない文字列のサイズを保持します。
		//
		struct TextSize
		{
			SIZE size = {};
			POINT center = {};

			//
			// デフォルトコンストラクタです。
			//
			TextSize()
			{
				// 何もしません。
			}

			//
			// コンストラクタです。
			//
			TextSize(HDC dc, LPCWSTR text, int32_t c, const POINT& spacing, BOOL overhang = FALSE)
			{
				init(dc, text, c, spacing, overhang);
			}

			//
			// 初期化処理を実行します。
			//
			BOOL init(HDC dc, LPCWSTR text, int32_t c, const POINT& spacing, BOOL overhang = FALSE)
			{
				std::wstring str(text, c);
				if (str.empty()) return FALSE;

				::GetTextExtentPoint32W(dc, str.c_str(), str.length(), &size);

				if (overhang)
				{
					ABC abc = {};
					::GetCharABCWidthsW(dc, str.back(), str.back(), &abc);
					size.cx -= abc.abcC;
				}

				center.x = size.cx / 2;
				center.y = size.cy / 2;

				return TRUE;
			}
		};

		inline static constexpr struct PosMode {
			inline static constexpr int32_t c_solo = 0;
			inline static constexpr int32_t c_pair = 1;
			inline static constexpr int32_t c_animation = 2;
		} c_pos_mode;

		//
		// このクラスはアイテムの位置パラメータを管理します。
		//
		struct Pos {
			inline static constexpr int32_t c_begin = 0;
			inline static constexpr int32_t c_end = 1;

			int32_t mode = 0;
			double pos[2] = {};
			std::string animation;

			//
			// パラメータの数を返します。
			//
			size_t get_count() {
				switch (mode)
				{
				case c_pos_mode.c_solo: return 1;
				default: return 2;
				}
			}
		};

		std::string temp_file_name;
		std::string each_temp_file_name;

		int32_t sorted_object_index = -1;

		std::string object_app_name;
		std::string first_filter_app_name;

		int32_t item_layer = 0;
		int32_t item_start = 0;
		int32_t item_end = 0;

		//
		// このクラスはテキストアイテムの属性です。
		//
		struct Item {
			struct Font {
				int32_t size = 0;
				BOOL bold = FALSE;
				BOOL italic = FALSE;
				int32_t align = 0;
				POINT spacing = {};
				std::string name;
			} font;
			Pos x = {}; // テキストアイテムのXです。
			Pos y = {}; // テキストアイテムのYです。
			int32_t draw_filter_index = -1; // テキストアイテムの描画フィルタのインデックスです。
			std::wstring text; // テキストアイテムに設定されているテキストです。
		} item;

		std::string draw_filter_app_name;

		struct Base {
			TextSize size;
			int32_t each_object_count = 0;
			SIZE whole = {};
			POINT whole_center = {};
			int32_t justification_width = 0;
			double justification_y[2] = {};
		} base;

		int32_t current_line_index = 0;
		int32_t each_ojbect_index = 0;

		inline static BOOL copy_private_profile_section(
			const std::string& src_file_name, const std::string& src_app_name,
			const std::string& dst_file_name, const std::string& dst_app_name)
		{
			static std::vector<char> section_buffer(70000);

			::GetPrivateProfileSectionA(src_app_name.c_str(), section_buffer.data(), section_buffer.size(), src_file_name.c_str());
			if (::GetLastError() != 0) return FALSE;
			return ::WritePrivateProfileSectionA(dst_app_name.c_str(), section_buffer.data(), dst_file_name.c_str());
		}

		//
		// iniファイルから文字列を読み込みます。
		//
		inline static auto read_private_profile_string(
			const std::string& file_name,
			const std::string& app_name,
			const std::string& key_name,
			const std::string& default_value = "",
			size_t buffer_length = MAX_PATH)
		{
			std::string buffer(buffer_length, '\0');
			auto length = ::GetPrivateProfileStringA(app_name.c_str(), key_name.c_str(), default_value.c_str(), buffer.data(), buffer.size(), file_name.c_str());
			buffer.resize(length);
			return buffer;
		}

		//
		// iniファイルから整数を読み込みます。
		//
		inline static auto read_private_profile_int(
			const std::string& file_name,
			const std::string& app_name,
			const std::string& key_name,
			int32_t default_value = 0)
		{
			return ::GetPrivateProfileIntA(app_name.c_str(), key_name.c_str(), default_value, file_name.c_str());
		}
		//
		// iniファイルに文字列を書き込みます。
		//
		inline static BOOL write_private_profile_string(
			const std::string& file_name,
			const std::string& app_name,
			const std::string& key_name,
			const std::string& value)
		{
			return ::WritePrivateProfileStringA(app_name.c_str(), key_name.c_str(), value.c_str(), file_name.c_str());
		}

		//
		// iniファイルに整数を書き込みます。
		//
		inline static BOOL write_private_profile_int(
			const std::string& file_name,
			const std::string& app_name,
			const std::string& key_name,
			int32_t value)
		{
			return ::WritePrivateProfileStringA(app_name.c_str(), key_name.c_str(), my::format("{/}", value).c_str(), file_name.c_str());
		}

		//
		// ソート済みアイテムのインデックスを返します。
		//
		inline static int32_t get_sorted_object_index(auto object)
		{
			auto c = magi.exin.get_sorted_object_count();

			for (decltype(c) i = 0; i < c; i++)
			{
				if (object == magi.exin.get_sorted_object(i))
					return i;
			}

			return -1;
		}

		//
		// 一時ファイル名を初期化します。
		//
		BOOL init_temp_file_name()
		{
			MY_TRACE_FUNC("");

			// テンポラリフォルダのパスを取得します。
//			auto temp_dir = my::hs(my::get_temp_path());
			auto temp_dir = my::hs(magi.get_config_file_name(hive.c_name));
			MY_TRACE_STR(temp_dir);

			// カレントプロセスのIDを取得します。ファイル名の重複を防ぐのに使用します。
			auto pid = ::GetCurrentProcessId();
			MY_TRACE_INT(pid);

			// テンポラリファイル名を取得します。
			temp_file_name = my::format("{/}\\text_split_{/}.exo", temp_dir, pid);
			MY_TRACE_STR(temp_file_name);

			// テンポラリファイル名を取得します。(文字列分割後のexoファイル)
			each_temp_file_name = my::format("{/}\\text_split_{/}_each.exo", temp_dir, pid);
			MY_TRACE_STR(each_temp_file_name);

			return TRUE;
		}

		//
		// アイテムの情報を初期化します。
		//
		BOOL init_item_info()
		{
			MY_TRACE_FUNC("");

			// カレントアイテムのインデックスを取得します。
			auto object_index = magi.exin.get_current_object_index();
			MY_TRACE_INT(object_index);
			if (object_index < 0) return FALSE;

			// カレントアイテムを取得します。
			auto object = magi.exin.get_object(object_index);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			// アイテムの中の最初のフィルタを取得します。
			auto first_filter = magi.exin.get_filter(object, 0);
			MY_TRACE_HEX(first_filter);
			if (!first_filter) return FALSE;

			// 最初のフィルタがテキストではない場合は
			if (std::string(first_filter->name) != "テキスト")
				return FALSE; // 選択アイテムがテキストアイテムではなかったので終了します。

			init_temp_file_name();

			// 現在のシーンをexoファイルに保存します。
			magi.exin.save_exo(temp_file_name.c_str());

			// ソート済みアイテムのインデックスを取得します。
			// このインデックスがexoファイル内の番号になります。
			sorted_object_index = get_sorted_object_index(object);
			MY_TRACE_INT(sorted_object_index);

			// アイテムのセクション名を取得します。
			object_app_name = my::format("{/}", sorted_object_index);
			MY_TRACE_STR(object_app_name);

			// レイヤーを取得します。
			item_layer = read_private_profile_int(temp_file_name, object_app_name, "layer", 1);
			MY_TRACE_INT(item_layer);

			// 開始フレームを取得します。
			item_start = read_private_profile_int(temp_file_name, object_app_name, "start", 1);
			MY_TRACE_INT(item_start);

			// 終了フレームを取得します。
			item_end = read_private_profile_int(temp_file_name, object_app_name, "end", 1);
			MY_TRACE_INT(item_end);

			// 最初のフィルタのセクション名を取得します。
			first_filter_app_name = my::format("{/}.0", sorted_object_index);
			MY_TRACE_STR(first_filter_app_name);

			// _nameをチェックします。
			auto _name = read_private_profile_string(temp_file_name, first_filter_app_name, "_name");
			MY_TRACE_STR(_name);
			if (_name != "テキスト") return FALSE; // テキストアイテムではなかったので終了します。

			// テキストアイテムの属性を取得します。
			item.font.size = read_private_profile_int(temp_file_name, first_filter_app_name, "サイズ", 1);
			item.font.bold = read_private_profile_int(temp_file_name, first_filter_app_name, "B", FALSE);
			item.font.italic = read_private_profile_int(temp_file_name, first_filter_app_name, "I", FALSE);
			item.font.align = read_private_profile_int(temp_file_name, first_filter_app_name, "align", 0);
			item.font.spacing.x = (signed char)(BYTE)read_private_profile_int(temp_file_name, first_filter_app_name, "spacing_x", 0);
			item.font.spacing.y = (signed char)(BYTE)read_private_profile_int(temp_file_name, first_filter_app_name, "spacing_y", 0);

			// フォント名を取得します。
			item.font.name = read_private_profile_string(temp_file_name, first_filter_app_name, "font");
			MY_TRACE_STR(item.font.name);

			// アイテムのXとYを取得します。
			item.x = {};
			item.y = {};
			item.draw_filter_index = -1;

			for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// セクション名を作成します。
				auto app_name = my::format("{/}.{/}", sorted_object_index, i);
				MY_TRACE_STR(app_name);

				// _name を取得します。
				auto _name = read_private_profile_string(temp_file_name, app_name, "_name");
				MY_TRACE_STR(_name);

				// 標準描画でも拡張描画でもない場合は
				if (_name != "標準描画" && _name != "拡張描画")
					continue; // スキップします。

				// XとYを取得します。
				init_item_pos_info(&item.x, app_name, "X");
				init_item_pos_info(&item.y, app_name, "Y");

				// 描画フィルタのインデックスを保存しておきます。
				item.draw_filter_index = i;

				break;
			}

			return TRUE;
		}

		//
		// アイテムの位置情報を初期化します。
		//
		BOOL init_item_pos_info(Pos* pos, const std::string& app_name, const std::string& key_name)
		{
			auto separator = ',';

			auto value = read_private_profile_string(temp_file_name, app_name, key_name);
			MY_TRACE_STR(value);

			auto sep1 = value.find(separator);

			// valueが数字のみの場合は
			if (sep1 == value.npos)
			{
				pos->mode = c_pos_mode.c_solo;
				pos->pos[pos->c_begin] = stof(value);
				return TRUE;
			}

			auto sep2 = value.find(separator, sep1 + 1);

			// valueが数字2個のみの場合は
			if (sep2 == value.npos)
			{
				pos->mode = c_pos_mode.c_pair;
				pos->pos[pos->c_begin] = stof(value);
				pos->pos[pos->c_end] = stof(value.substr(sep1 + 1));
				return TRUE;
			}

			// valueが数字2個とアニメーションテキストの場合は
			{
				pos->mode = c_pos_mode.c_animation;
				pos->pos[pos->c_begin] = stof(value);
				pos->pos[pos->c_end] = stof(value.substr(sep1 + 1, sep2 - (sep1 + 1)));
				pos->animation = value.substr(sep2 + 1);
				return TRUE;
			}
		}

		//
		// アイテムのテキスト情報を初期化します。
		//
		BOOL init_item_text_info()
		{
			// テキストが入っているエディットボックスを取得します。
			auto edit = ::GetDlgItem(magi.exin.get_setting_dialog(), 0x5655);
			MY_TRACE_HEX(edit);

			// エディットボックスからテキストを取得します。
			item.text = my::get_window_text(edit);

			return TRUE;
		}

		//
		// 1行分の文字列をノーマライズして返します。
		//
		inline static std::wstring normalize_line(const std::wstring& line)
		{
			auto str = line;
			auto chars = L"\r\n"s;
 
			for (auto c : chars)
				str.erase(std::remove(str.begin(), str.end(), c), str.end());

			return str;
		}

		//
		// 各種計算に必要な基準サイズを初期化します。
		//
		BOOL init_base_size_info(HDC dc)
		{
			// 基準となる文字列矩形を取得します。
			base.size.init(dc, L"A", 1, item.font.spacing);

			// 分解後のアイテムの総数です。
			base.each_object_count = 0;

			// テキスト全体の大きさです。
			base.whole = {};

			{
				// ここで分解後のアイテムの総数とテキスト全体の大きさを取得します。

				std::wstringstream stream(item.text);
				std::wstring line;
				while (std::getline(stream, line, L'\n'))
				{
					line = normalize_line(line);

					TextSize line_size(dc, line.c_str(), line.length(), item.font.spacing, TRUE);

					if (line_size.size.cx == 0)
						continue;

					base.whole.cx = std::max(base.whole.cx, line_size.size.cx);
					base.whole.cy += base.size.size.cy + item.font.spacing.y;

					// 行毎に分解する場合は
					if (hive.each_mode == hive.c_each_mode.c_row)
					{
						base.each_object_count++; // 行数をカウントします。
					}
					else
					{
						for (size_t ch_index = 0; ch_index < line.length(); ch_index++)
						{
							auto ch = line[ch_index];

							base.each_object_count++; // 文字数をカウントします。
						}
					}
				}

				base.whole.cy -= item.font.spacing.y;

				MY_TRACE_INT(base.each_object_count);
				MY_TRACE_INT(base.whole.cx);
				MY_TRACE_INT(base.whole.cy);
			}

			// テキスト全体の中心座標です。
			base.whole_center.x = base.whole.cx / 2;
			base.whole_center.y = base.whole.cy / 2;

			// 行端揃えの幅です。
			base.justification_width = base.whole.cx;
			if (hive.fit_mode == hive.c_fit_mode.c_manual && hive.fit_width != 0)
				base.justification_width = hive.fit_width;

			for (size_t i = 0; i < 2; i++)
			{
				base.justification_y[i] = item.x.pos[i]; // [上]

				switch (item.font.align)
				{
				case 1:
				case 4:
				case 7:
					{
						// 中央揃え

						base.justification_y[i] -= base.justification_width / 2;

						break;
					}
				case 2:
				case 5:
				case 8:
					{
						// 右寄せ

						base.justification_y[i] -= base.justification_width;

						break;
					}
				}
			}

			return TRUE;
		}

		//
		// フレームオフセットを返します。
		//
		int32_t get_frame_offset()
		{
			switch (hive.shift_mode)
			{
			case hive.c_shift_mode.c_offset:
				{
					return each_ojbect_index * hive.shift_offset;
				}
			case hive.c_shift_mode.c_range:
				{
					auto item_range = item_end - item_start + 1;
					auto range = hive.shift_range - item_range;
					return ::MulDiv(range, each_ojbect_index, base.each_object_count - 1);
				}
			}

			return 0;
		}

		//
		// 出力レイヤーを返します。
		//
		int32_t get_output_layer()
		{
			auto output_layer = item_layer + each_ojbect_index;

			// 元のアイテムを削除しない場合は
			if (!hive.erase_orig_item)
				output_layer++; // 出力レイヤーを1つ下に下げます。

			return output_layer;
		}

		//
		// iniファイルにアイテムの位置情報を書き込みます。
		//
		BOOL write_private_profile_pos(const std::string& file_name, const std::string& app_name, const std::string& key_name, double value[2], const Pos& pos)
		{
			std::string str;

			switch (pos.mode)
			{
			case c_pos_mode.c_solo:
				{
					str = my::format("{/.1f}", value[0]);
					break;
				}
			case c_pos_mode.c_pair:
				{
					str = my::format("{/.1f},{/.1f}", value[0], value[1]);
					break;
				}
			case c_pos_mode.c_animation:
				{
					str = my::format("{/.1f},{/.1f},{/}", value[0], value[1], pos.animation);
					break;
				}
			}

			MY_TRACE_STR(str);

			write_private_profile_string(file_name, app_name, key_name, str);

			return TRUE;
		}

		//
		// 一時ファイルにテキストアイテム情報を書き込みます。
		//
		BOOL write_text_item_info(const std::string& str, double x[2], double y[2])
		{
			// フレームオフセットを決定します。
			auto frame_offset = get_frame_offset();

			// 出力レイヤーを決定します。
			auto output_layer = get_output_layer();

			write_private_profile_int(temp_file_name, object_app_name, "layer", output_layer);
			write_private_profile_int(temp_file_name, object_app_name, "start", item_start + frame_offset);
			write_private_profile_int(temp_file_name, object_app_name, "end", item_end + frame_offset);
			write_private_profile_pos(temp_file_name, draw_filter_app_name, "X", x, item.x);
			write_private_profile_pos(temp_file_name, draw_filter_app_name, "Y", y, item.y);
			write_private_profile_string(temp_file_name, first_filter_app_name, "text",  str);

			auto each_object_app_name = my::format("{/}", each_ojbect_index);
			MY_TRACE_STR(each_object_app_name);

			copy_private_profile_section(
				temp_file_name, object_app_name,
				each_temp_file_name, each_object_app_name);

			for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				auto filter_app_name = my::format("{/}.{/}", sorted_object_index, i);
//				MY_TRACE_STR(filter_app_name);

				auto each_filter_app_name = my::format("{/}.{/}", each_ojbect_index, i);
//				MY_TRACE_STR(each_filter_app_name);

				copy_private_profile_section(
					temp_file_name, filter_app_name,
					each_temp_file_name, each_filter_app_name);
			}

			return TRUE;
		}

		//
		// 一時ファイルに一文字分のアイテム情報を書き込みます。
		//
		BOOL write_ch_item_info(LPCWSTR ch, double x[2], double y[2])
		{
			// 16進数文字に変換されたテキストを取得します。
			std::string str;
			{
				auto hex = (BYTE*)ch;
				str = my::format("{/02X}{/02X}", hex[0], hex[1]);
				str.resize(4096, '0');
			}
			MY_TRACE_STR(str);

			return write_text_item_info(str, x, y);
		}

		BOOL split_to_ch(HDC dc, const std::wstring& line, double y[2])
		{
			TextSize line_size(dc, line.c_str(), line.length(), item.font.spacing, TRUE);

			auto extra_width = base.justification_width - line_size.size.cx;

			for (size_t ch_index = 0; ch_index < line.length(); ch_index++)
			{
				auto ch = line[ch_index];
				MY_TRACE_HEX(ch);

				// 文字列矩形を取得します。
				TextSize text_size(dc, line.c_str(), ch_index, item.font.spacing);

				// 文字の矩形を取得します。
				TextSize ch_size(dc, line.c_str() + ch_index, 1, item.font.spacing, TRUE);

				// アイテムのX座標を算出します。

				double x[2] = {};

				for (size_t i = 0; i < item.x.get_count(); i++)
				{
					// 行端揃えなら
					if (hive.fit_mode != hive.c_fit_mode.c_none)
					{
						x[i] = item.x.pos[i] + text_size.size.cx; // 左寄せ

						if (line.length() >= 2)
							x[i] += ::MulDiv(extra_width, ch_index, line.length() - 1);

						switch (item.font.align)
						{
						case 1:
						case 4:
						case 7:
							{
								// 中央揃え

								x[i] -= base.justification_width / 2;
								x[i] += ch_size.center.x;

								break;
							}
						case 2:
						case 5:
						case 8:
							{
								// 右寄せ

								x[i] -= base.justification_width;
								x[i] += ch_size.size.cx;

								break;
							}
						}
					}
					// 行端揃えではないなら
					else
					{
						x[i] = item.x.pos[i] + text_size.size.cx; // 左寄せ

						switch (item.font.align)
						{
						case 1:
						case 4:
						case 7:
							{
								// 中央揃え

								x[i] -= line_size.center.x;
								x[i] += ch_size.center.x;

								break;
							}
						case 2:
						case 5:
						case 8:
							{
								// 右寄せ

								x[i] -= line_size.size.cx;
								x[i] += ch_size.size.cx;

								break;
							}
						}
					}
				}

				MY_TRACE("line_index = {/}, ch_index = {/}, x = ({/}, {/}), y = ({/}, {/})\n", current_line_index, ch_index, x[0], x[1], y[0], y[1]);

				if (write_ch_item_info(&line[ch_index], x, y))
					each_ojbect_index++;
			}

			return TRUE;
		}

		BOOL split_to_row(HDC dc, const std::wstring& line, double y[2])
		{
			TextSize line_size(dc, line.c_str(), line.length(), item.font.spacing, TRUE);

			// アイテムのX座標を算出します。

			double x[2] = {};

			for (size_t i = 0; i < item.x.get_count(); i++)
				x[i] = item.x.pos[i];

			MY_TRACE("line_index = {/}, x = ({/}, {/}), y = ({/}, {/})\n", current_line_index, x[0], x[1], y[0], y[1]);

			// 16進数文字に変換されたテキストを取得します。
			std::string str;
			{
				for (auto ch : line)
				{
					auto hex = (BYTE*)&ch;

					str += my::format("{/02X}{/02X}", hex[0], hex[1]);
				}

				str.resize(4096, '0');
			}
			MY_TRACE_STR(str);

			if (write_text_item_info(str, x, y))
				each_ojbect_index++;

			return TRUE;
		}

		//
		// テキストアイテムを文字毎または行毎に分解します。
		//
		BOOL split_text()
		{
			MY_TRACE_FUNC("");

			auto fp = magi.auin.get_filter_plugin(magi.fp, "拡張編集");
			auto editp = magi.exin.get_editp();
			if (!fp || !editp) return FALSE;

			// アイテム情報を初期化します。
			if (!init_item_info()) return FALSE;

			// [exedit]をバッファにコピーします。
			copy_private_profile_section(
				temp_file_name, "exedit",
				each_temp_file_name, "exedit");

			// 描画フィルタのセクション名を作成します。
			draw_filter_app_name = my::format("{/}.{/}", sorted_object_index, item.draw_filter_index);
			MY_TRACE_STR(draw_filter_app_name);

			// アイテムのテキスト情報を初期化します。
			init_item_text_info();

			// 文字列矩形を取得するためのHDCを用意します。
			my::ClientDC dc(fp->hwnd);
			my::gdi::unique_ptr<HFONT> font(::CreateFontA(-item.font.size, 0, 0, 0,
				item.font.bold ? FW_BOLD : FW_NORMAL, item.font.italic, FALSE, FALSE,
				DEFAULT_CHARSET, 0, 0, 0, 0, item.font.name.c_str()));
			my::gdi::selector font_selector(dc, font.get());

			::SetTextCharacterExtra(dc, item.font.spacing.x);

			// 基準サイズを初期化します。
			init_base_size_info(dc);

			current_line_index = 0;
			each_ojbect_index = 0;

			std::wstringstream stream(item.text);
			std::wstring line;
			while (std::getline(stream, line, L'\n'))
			{
				line = normalize_line(line);
				MY_TRACE_STR(line);

				// アイテムのY座標を算出します。

				double y[2] = {};

				for (size_t i = 0; i < item.y.get_count(); i++)
				{
					y[i] = item.y.pos[i] + (base.size.size.cy + item.font.spacing.y) * current_line_index; // [上]

					switch (item.font.align)
					{
					case 3:
					case 4:
					case 5:
						{
							// [中]

							y[i] -= base.whole_center.y;
							y[i] += base.size.center.y;

							break;
						}
					case 6:
					case 7:
					case 8:
						{
							// [下]

							y[i] -= base.whole.cy;
							y[i] += base.size.size.cy;

							break;
						}
					}
				}

				if (hive.each_mode == hive.c_each_mode.c_row)
					split_to_row(dc, line, y);
				else
					split_to_ch(dc, line, y);

				current_line_index++;
			}

			// 元のアイテムを削除するように設定されている場合は
			if (hive.erase_orig_item)
			{
				// 元のアイテムを削除します。
				::SendMessage(magi.exin.get_exedit_window(), WM_COMMAND, 0x3E9, 0);
			}

			// exoファイルをフラッシュします。
			auto r4 = ::GetPrivateProfileIntA("exedit", "width", 123, each_temp_file_name.c_str());
			MY_TRACE_INT(r4);

			// exoファイルを読み込みます。
			if (!magi.exin.load_exo(each_temp_file_name.c_str(), 0, 0, fp, editp))
				hive.message_box(L"exoファイルの読み込みに失敗しました");

			// 不要になったテンポラリファイルを削除します。
			::DeleteFileA(temp_file_name.c_str());
			::DeleteFileA(each_temp_file_name.c_str());

			// 拡張編集を再描画します。
			magi.exin.invalidate();

			// aviutlを再描画します。
			magi.redraw();

			return TRUE;
		}
	};
}
