#pragma once

namespace apn::text_split
{
	//
	// このクラスはテキストオブジェクトの情報を保持します。
	//
	struct text_object_t
	{
		struct track_t {
			inline static constexpr size_t c_size = 0;
			inline static constexpr size_t c_speed = 1;
		} c_track;

		struct check_t {
			inline static constexpr size_t c_parts = 0;
			inline static constexpr size_t c_ = 1;
			inline static constexpr size_t c_auto_scroll = 2;
			inline static constexpr size_t c_bold = 3;
			inline static constexpr size_t c_italic = 4;
		} c_check;

		//
		// テキストオブジェクトのインデックスです。
		//
		int32_t orig_object_index = -1;

		//
		// ソート済みオブジェクトのインデックスです。
		//
		int32_t sorted_object_index = -1;

		//
		// テキストオブジェクトの描画フィルタのインデックスです。
		//
		int32_t draw_filter_index = -1;

		//
		// exoファイル内のセクション名です。
		//
		std::string object_section_name;
		std::string main_filter_section_name;
		std::string draw_filter_section_name;

		//
		// このクラスはタイムライン上のオブジェクト情報です。
		//
		struct item_t {
			int32_t layer = 0;
			int32_t start = 0;
			int32_t end = 0;
		} timeline;

		//
		// このクラスはテキストオブジェクトのフォントです。
		//
		struct font_t {
			int32_t size = 0;
			BOOL bold = FALSE;
			BOOL italic = FALSE;
			int32_t align = 0;
			POINT spacing = {};
			std::string name;
		} font;

		//
		// テキストオブジェクトが持つ文字列です。
		//
		std::wstring text;

		//
		// テキストオブジェクトの座標です。
		//
		pos_t x, y;

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
		// コンストラクタです。
		//
		text_object_t(exo_t& exo)
		{
			MY_TRACE_FUNC("");

			// メインフィルタのインデックスです。
			constexpr auto c_main_filter_index = 0;

			// カレントオブジェクトのインデックスを取得します。
			orig_object_index = magi.exin.get_current_object_index();
			MY_TRACE_INT(orig_object_index);
			if (orig_object_index < 0) throw L"カレントオブジェクトを取得できませんでした";

			// カレントオブジェクトを取得します。
			auto object = magi.exin.get_object(orig_object_index);
			MY_TRACE_HEX(object);
			if (!object) throw L"カレントオブジェクトが無効です";

			// メインフィルタがテキストフィルタではない場合は
			if (object->filter_param[0].id != 3)
				throw L"カレントオブジェクトがテキストオブジェクトではありません";

			// タイムライン情報を取得します。
			{
				// レイヤーを取得します。
				timeline.layer = object->layer_set + 1;
				MY_TRACE_INT(timeline.layer);

				// 開始フレームを取得します。
				timeline.start = object->frame_begin + 1;
				MY_TRACE_INT(timeline.start);

				// 終了フレームを取得します。
				timeline.end = object->frame_end + 1;
				MY_TRACE_INT(timeline.end);
			}

			// ソート済みオブジェクトのインデックスを取得します。
			// このインデックスがexoファイル内の番号になります。
			sorted_object_index = get_sorted_object_index(object);
			MY_TRACE_INT(sorted_object_index);

			// オブジェクトのセクション名を取得します。
			object_section_name = my::format("{/}", sorted_object_index);
			MY_TRACE_STR(object_section_name);

			// 最初のフィルタのセクション名を取得します。
			main_filter_section_name = my::format("{/}.{/}", sorted_object_index, c_main_filter_index);
			MY_TRACE_STR(main_filter_section_name);

			{
				// テキストオブジェクトの拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efText*)
					magi.exin.get_exdata(object, c_main_filter_index);

				// テキストオブジェクトの文字列を取得します。
				text = exdata->text;
			}

			{
				// _nameを取得します。
				auto _name = exo.read_string(main_filter_section_name, "_name");
				MY_TRACE_STR(_name);
				if (_name != "テキスト") throw L"exoファイル内のオブジェクトがテキストオブジェクトではありません";
			}

			// テキストオブジェクトの属性を取得します。
			{
				font.size = exo.read_int(main_filter_section_name, "サイズ", 1);
				font.bold = exo.read_int(main_filter_section_name, "B", FALSE);
				font.italic = exo.read_int(main_filter_section_name, "I", FALSE);
				font.align = exo.read_int(main_filter_section_name, "align", 0);
				font.spacing.x = (signed char)(BYTE)exo.read_int(main_filter_section_name, "spacing_x", 0);
				font.spacing.y = (signed char)(BYTE)exo.read_int(main_filter_section_name, "spacing_y", 0);
				font.name = exo.read_string(main_filter_section_name, "font");
			}

			// 描画フィルタの情報を取得します。
			{
				// 描画フィルタのインデックスを取得します。
				draw_filter_index = object->countFilters() - 1;
				if (draw_filter_index <= 0) throw L"フィルタの数が無効です";

				// 描画フィルタのセクション名を取得します。
				draw_filter_section_name = my::format("{/}.{/}", sorted_object_index, draw_filter_index);
				MY_TRACE_STR(draw_filter_section_name);

				// _name を取得します。
				auto _name = exo.read_string(draw_filter_section_name, "_name");
				MY_TRACE_STR(_name);
				if (_name != "標準描画" && _name != "拡張描画") throw L"描画フィルタを取得できませんでした";

				// XとYを取得します。
				x.from_string(exo.read_string(draw_filter_section_name, "X"));
				y.from_string(exo.read_string(draw_filter_section_name, "Y"));
			}
		}
	};
}
