#pragma once

namespace apn::text_split
{
	//
	// このクラスはテキストオブジェクトの分解処理を実行します。
	//
	struct executor_t
	{
		//
		// exoファイル内でのテキストの長さです。
		//
		inline static constexpr auto c_exo_text_length = 4096;

		//
		// 使用するファイルパスを管理します。
		//
		path_t path;

		//
		// 使用するexoファイルを管理します。
		//
		exo_t exo;

		//
		// 使用するテキストオブジェクトを管理します。
		//
		text_object_t text_object;

		//
		// aviutlの変数です。
		//
		AviUtl::FilterPlugin* fp = nullptr;
		AviUtl::EditHandle* editp = nullptr;

		//
		// このクラスは改行を含まない文字列の寸法を保持します。
		//
		struct text_metrics_t
		{
			//
			// 文字列です。
			//
			std::wstring text;

			//
			// 文字幅の配列です。
			//
			std::vector<int32_t> widths;

			//
			// 文字列全体の幅です。
			//
			int32_t bounds = 0;

			//
			// コンストラクタです。
			//
			text_metrics_t(HDC dc, const std::wstring& text)
				: text(text)
			{
				// 文字毎に処理します。
				for (auto ch : text)
				{
					// ::GetGlyphOutlineW()用の行列です。
					// 拡張編集は手動で寸法を1/2にしていますが、
					// ここではこの行列を使用して1/2にしています。
					constexpr MAT2 mat = {
						{ 0xFFFF / 2, 0 }, { 0, 0 },
						{ 0, 0 }, { 0xFFFF / 2, 0 },
					};

					// 文字の幅を取得します。
					GLYPHMETRICS gm = {};
					::GetGlyphOutlineW(dc, ch, GGO_METRICS, &gm, 0, nullptr, &mat);

					// 文字の幅を配列に追加します。
					widths.emplace_back(gm.gmCellIncX);

					// 文字の幅を加算します。
					bounds += widths.back();
				}
			}
		};

		//
		// このクラスは寸法です。
		//
		struct metrics_t
		{
			//
			// フォント(一行)の高さです。
			//
			int32_t height;

			//
			// 全行及び全文字の寸法です。
			//
			std::vector<text_metrics_t> lines;

			//
			// テキスト全体の寸法です。
			//
			SIZE bounds = {};

			//
			// 行端揃えするときの幅です。
			//
			int32_t justification_width = 0;
		} metrics;

		//
		// 分解後オブジェクトの数です。
		//
		int32_t nb_part_objects = 0;

		//
		// 書き込んだ分解後オブジェクトの数です。
		//
		int32_t nb_written = 0;

		//
		// コンストラクタです。
		//
		executor_t()
			: path()
			, exo(path)
			, text_object(exo)
			, fp(magi.auin.get_filter_plugin(magi.fp, "拡張編集"))
			, editp(magi.exin.get_editp())
		{
			if (!fp || !editp) throw L"拡張編集を取得できませんでした";
		}

		//
		// exoファイルに分解後オブジェクトを書き込みます。
		//
		void write_part(const std::string& str, const pos_t::value_t& x, const pos_t::value_t& y)
		{
			// フレームオフセットを取得します。
			auto frame_offset = [&]() -> int32_t
			{
				// ずらしモードで分岐します。
				switch (hive.shift_mode)
				{
				// オフセットでずらす場合は
				case hive.c_shift_mode.c_offset:
					{
						// オフセットの分だけずらします。
						return nb_written * hive.shift_offset;
					}
				// 範囲内で均等にずらす場合は
				case hive.c_shift_mode.c_range:
					{
						// タイムライン上のアイテムの長さを算出します。
						auto item_range = text_object.timeline.end - text_object.timeline.start + 1;

						// 配置範囲を算出します。
						auto deploy_range = hive.shift_range - item_range;

						// 配置範囲内の均等な位置を返します。
						return ::MulDiv(deploy_range, nb_written, nb_part_objects - 1);
					}
				}

				return 0;
			}();

			// 出力レイヤーを取得します。
			auto output_layer = [&]() -> int32_t
			{
				// 出力レイヤーを算出します。
				auto output_layer = text_object.timeline.layer + nb_written;

				// 元のアイテムを削除しない場合は出力レイヤーを1つ下に下げます。
				if (!hive.erase_orig_item) output_layer++;

				// 出力レイヤーを返します。
				return output_layer;
			}();

			// 各属性を書き込みます。
			exo.write_int(text_object.object_section_name, "layer", output_layer);
			exo.write_int(text_object.object_section_name, "start", text_object.timeline.start + frame_offset);
			exo.write_int(text_object.object_section_name, "end", text_object.timeline.end + frame_offset);
			exo.write_pos(text_object.draw_filter_section_name, "X", x, text_object.x);
			exo.write_pos(text_object.draw_filter_section_name, "Y", y, text_object.y);
			exo.write_string(text_object.main_filter_section_name, "text",  str);

			// 分解後オブジェクトのセクション名を取得します。
			auto part_object_section_name = my::format("{/}", nb_written);

			// 分解後オブジェクトのセクションに
			// 元のオブジェクトのセクションをコピーします。
			exo.copy_section(text_object.object_section_name, part_object_section_name);

			// オブジェクトのフィルタを走査します。
			for (size_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// 元のオブジェクトのフィルタのセクション名を取得します。
				auto orig_filter_section_name = my::format("{/}.{/}", text_object.sorted_object_index, i);

				// 分解後オブジェクトのフィルタのセクション名を取得します。
				auto part_filter_section_name = my::format("{/}.{/}", nb_written, i);

				// 分解後オブジェクトのフィルタのセクションに
				// 元のオブジェクトのフィルタのセクションをコピーします。
				exo.copy_section(orig_filter_section_name, part_filter_section_name);
			}

			// 書き込んだ数を増やします。
			nb_written++;
		}

		//
		// テキストオブジェクトを文字毎に分解して書き込みます。
		//
		void write_as_ch(HDC dc, const text_metrics_t& line, const pos_t::value_t& y)
		{
			// 文字のX座標です。
			auto ch_x = 0.0;

			// 文字毎に処理します。
			for (size_t ch_index = 0; ch_index < line.text.length(); ch_index++)
			{
				// 文字の幅です。
				auto ch_width = line.widths[ch_index] + text_object.font.spacing.x;

				// 分解後テキストオブジェクトのX座標を算出します。
				pos_t::value_t x = {};
				{
					// 座標パラメータの数だけ処理します。
					for (size_t i = 0; i < text_object.x.get_nb_params(); i++)
					{
						// 左寄せの場合のX座標です。
						x[i] = text_object.x.params[i] + ch_x;

						// テキストオブジェクトのアラインで分岐します。
						switch (text_object.font.align)
						{
						// 中央揃えの場合の処理です。
						case 1: case 4: case 7:
							x[i] -= line.bounds / 2.0; // 分解後の左寄せ座標を算出します。
							x[i] += line.widths[ch_index] / 2.0; // 分解後の中央揃え座標に変換します。
							break;

						// 右寄せの場合の処理です。
						case 2: case 5: case 8:
							x[i] -= line.bounds; // 分解後の左寄せ座標を算出します。
							x[i] += line.widths[ch_index]; // 分解後の右寄せ座標に変換します。
							break;
						}
					}
				}

				// 16進数文字に変換されたテキストを取得します。
				std::string str;
				{
					auto hex = (BYTE*)&line.text[ch_index];
					str = my::format("{/02X}{/02X}", hex[0], hex[1]);
					str.resize(c_exo_text_length, '0');
				}

				// exoファイルに分解後のテキストオブジェクトを書き込みます。
				write_part(str, x, y);

				// 文字のX座標を更新します。
				ch_x += ch_width;
			}
		}

		//
		// テキストオブジェクトを行端揃えの文字毎に分解して書き込みます。
		//
		void write_as_fit_ch(HDC dc, const text_metrics_t& line, const pos_t::value_t& y)
		{
			// 二文字未満の場合は行端揃えができないので
			// 通常の文字毎の処理を実行します。
			if (line.widths.size() < 2) return write_as_ch(dc, line, y);

			auto front_offset = line.widths.front() / 2.0;
			auto back_offset = line.widths.back() / 2.0;

			// 行端揃え用の文字間隔を算出します。
			auto interval = (double)metrics.justification_width;
			{
				// 先頭と末尾の文字の幅の半分だけ縮小します。
				interval -= front_offset + back_offset;

				// 文字の間の数で除算します。
				interval /= line.widths.size() - 1;
			}

			// 文字のX座標です。
			auto ch_x = front_offset;

			// 文字毎に処理します。
			for (size_t ch_index = 0; ch_index < line.text.length(); ch_index++)
			{
				// 分解後テキストオブジェクトのX座標を算出します。
				pos_t::value_t x = {};
				{
					// 座標パラメータの数だけ処理します。
					for (size_t i = 0; i < text_object.x.get_nb_params(); i++)
					{
						// まず、X座標の基準値をセットします。
						x[i] = text_object.x.params[i] + ch_x;

						// テキストオブジェクトのアラインで分岐します。
						switch (text_object.font.align)
						{
						// 左寄せの場合の処理です。
						case 0: case 3: case 6:
							x[i] -= line.widths[ch_index] / 2.0; // 文字幅の半分だけ左にずらします。
							break;

						// 中央揃えの場合の処理です。
						case 1: case 4: case 7:
							x[i] -= metrics.justification_width / 2.0; // 全体の幅の半分だけ左にずらします。
							break;

						// 右寄せの場合の処理です。
						case 2: case 5: case 8:
							x[i] -= metrics.justification_width; // 全体の幅の分だけに左にずらします。
							x[i] += line.widths[ch_index] / 2.0; // 文字幅の半分だけ右にずらします。
							break;
						}
					}
				}

				// 16進数文字に変換されたテキストを取得します。
				std::string str;
				{
					auto hex = (BYTE*)&line.text[ch_index];
					str = my::format("{/02X}{/02X}", hex[0], hex[1]);
					str.resize(c_exo_text_length, '0');
				}

				// exoファイルに分解後のテキストオブジェクトを書き込みます。
				write_part(str, x, y);

				// 文字のX座標を更新します。
				ch_x += interval;
			}
		}

		//
		// テキストオブジェクトを行毎に分解して書き込みます。
		//
		void write_as_row(HDC dc, const text_metrics_t& line, const pos_t::value_t& y)
		{
			// 分解後テキストオブジェクトのX座標を算出します。
			// いずれのモードでも自然に揃うので
			// そのままコピーするだけで構いません。
			pos_t::value_t x = {};
			{
				for (size_t i = 0; i < text_object.x.get_nb_params(); i++)
					x[i] = text_object.x.params[i];
			}

			// 16進数文字に変換されたテキストを取得します。
			std::string str;
			{
				for (auto ch : line.text)
				{
					auto hex = (BYTE*)&ch;

					str += my::format("{/02X}{/02X}", hex[0], hex[1]);
				}

				str.resize(c_exo_text_length, '0');
			}

			// exoファイルに分解後のテキストオブジェクトを書き込みます。
			write_part(str, x, y);
		}

		//
		// 分解されたテキストオブジェクトをタイムラインに配置します。
		//
		void deploy(const std::string& exo_file_name, int32_t orig_object_index)
		{
			// 分解後ファイルを作成します。
			if (!exo.part_file.write_file(exo_file_name))
				throw L"exoファイルの作成に失敗しました";

			// 新しいアンドゥブロックを作成します。
			magi.exin.push_undo();

			// 元のアイテムを削除するように設定されている場合は
			if (hive.erase_orig_item)
			{
				// 元のアイテムを削除します。
				magi.exin.erase_object(orig_object_index);
			}

			// 分解後ファイルを読み込んでアイテムを配置します。
			hive.exclude_objects.insert(orig_object_index);
			auto frame = magi.exin.load_exo_internal(exo_file_name.c_str(), 0, 0);
			hive.exclude_objects.erase(orig_object_index);

			// 以下の処理はスコープ終了時に実行します。
			my::scope_exit scope_exit([&]()
			{
				// オブジェクトテーブルを更新して、タイムラインを再描画します。
				magi.exin.update_object_table();
				magi.exin.redraw_timeline();

				// aviutlを再描画します。
				magi.redraw();
			});

			// 最終フレームが有効の場合は
			if (frame >= 0)
			{
				// フレーム数を更新します。
				magi.exin.set_frame_number(frame, editp, fp);
			}
			// 最終フレームが無効の場合は
			else
			{
				// ユーザーに通知します。
				throw L"exoファイルの読み込みに失敗しました";
			}
		}

		//
		// テキストオブジェクトを文字毎または行毎に分解して配置します。
		//
		void operator()()
		{
			MY_TRACE_FUNC("");

			// 文字列矩形を取得するためのDCです。
			dc_t dc(fp->hwnd, text_object);

			// 文字の高さを取得します。
			{
				// フォントの寸法を取得します。
				TEXTMETRICW tm = {};
				::GetTextMetricsW(dc, &tm);

				// 文字の高さを取得します。
				metrics.height = tm.tmHeight;
			}

			// テキスト全体・行毎・文字毎の寸法を算出します。
			{
				// テキストを行毎に処理します。
				std::wstringstream stream(text_object.text);
				std::wstring line;
				while (std::getline(stream, line, L'\n'))
				{
					// 正規化された行を取得します。
					line = [&line]()
					{
						// 除外する文字です。
						constexpr wchar_t chars[] = L"\r\n";

						// 除外する文字を取り除いて返します。
						auto str = line;
						for (auto ch : chars)
							str.erase(std::remove(str.begin(), str.end(), ch), str.end());
						return str;
					}();

					// 二行目以降の場合は行間スペースを追加します。
					if (metrics.lines.size()) metrics.bounds.cy += text_object.font.spacing.y;

					// 行の寸法を配列に追加して取得します。
					auto& line_metrics = metrics.lines.emplace_back(dc, line);

					// 二文字以上の場合は文字間スペースを追加します。
					if (line.length() >= 2)
						line_metrics.bounds += (line.length() - 1) * text_object.font.spacing.x;

					// テキスト全体の寸法を拡張します。
					metrics.bounds.cx = std::max(metrics.bounds.cx, (LONG)line_metrics.bounds);
					metrics.bounds.cy += metrics.height;
				}
			}

			// 行端揃えの幅を取得します。
			{
				metrics.justification_width = metrics.bounds.cx;
				if (hive.fit_mode == hive.c_fit_mode.c_manual && hive.fit_width != 0)
					metrics.justification_width = hive.fit_width;
			}

			// 分解後オブジェクトの総数を取得します。
			{
				// 行毎に処理します。
				for (const auto& line : metrics.lines)
				{
					// 行毎に分解する場合は
					if (hive.each_mode == hive.c_each_mode.c_row)
					{
						nb_part_objects++; // 行数をカウントします。
					}
					// 文字毎に分解する場合は
					else
					{
						nb_part_objects += line.text.length(); // 文字数をカウントします。
					}
				}
			}

			// テキストオブジェクトのY座標(原点)を算出します。
			pos_t::value_t y = {};
			{
				// 座標パラメータの数だけ処理します。
				for (size_t i = 0; i < text_object.y.get_nb_params(); i++)
				{
					// [上]の場合の座標です。
					y[i] = text_object.y.params[i];

					// テキストオブジェクトのアラインで分岐します。
					switch (text_object.font.align)
					{
					// [中]の場合の処理です。
					case 3: case 4: case 5:
						y[i] -= metrics.bounds.cy / 2.0; // 分解後の[上]座標を算出します。
						y[i] += metrics.height / 2.0; // 分解後の[中]座標に変換します。
						break;

					// [下]の場合の処理です。
					case 6: case 7: case 8:
						y[i] -= metrics.bounds.cy; // 分解後の[上]座標を算出します。
						y[i] += metrics.height; // 分解後の[下]座標に変換します。
						break;
					}
				}
			}

			// テキストを解析して、分解後オブジェクトをexoファイルに書き込みます。
			for (const auto& line : metrics.lines)
			{
				// 行毎の分解が指定されている場合は
				// テキストオブジェクトを行毎に分割して書き込みます。
				if (hive.each_mode == hive.c_each_mode.c_row)
					write_as_row(dc, line, y);

				// 行端揃えが指定されている場合は
				// テキストオブジェクトを行端揃えの文字毎に分割して書き込みます。
				else if (hive.fit_mode != hive.c_fit_mode.c_none)
					write_as_fit_ch(dc, line, y);

				// それ以外の場合は
				// テキストオブジェクトを文字毎に分割して書き込みます。
				else
					write_as_ch(dc, line, y);

				// Y座標を更新します。
				for (size_t i = 0; i < text_object.y.get_nb_params(); i++)
					y[i] += metrics.height + text_object.font.spacing.y;
			}

			// 分解後のテキストオブジェクトをタイムラインに配置します。
			deploy(path.each_temp_file_name, text_object.orig_object_index);
		}
	};
}
