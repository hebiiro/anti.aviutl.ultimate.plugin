#pragma once

namespace apn::exedit_tweaker
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	inline struct ConfigIO : StdConfigIO
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			return __super::init(hive.instance);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			return __super::exit(hive.instance);
		}

		inline void get_color(const ptree& node, COLORREF& value)
		{
			std::string str = node.get_value<std::string>("");
			size_t c = str.length();
			if (c == 0) return;

			if (str[0] == '#')
			{
				// 先頭の'#'を取り除いた残りの部分を16進数に変換します。
				auto temp = std::stoul(&str[1], 0, 16);

				if (c == 4)
				{
					// `#fc8`の形式です。

					auto r = (temp & 0x0F00) >> 8;
					auto g = (temp & 0x00F0) >> 4;
					auto b = (temp & 0x000F) >> 0;

					r |= r << 4;
					g |= g << 4;
					b |= b << 4;

					value = RGB(r, g, b);
				}
				else
				{
					// `#ffcc88`の形式です。

					auto r = (temp & 0x00FF0000) >> 16;
					auto g = (temp & 0x0000FF00) >> 8;
					auto b = (temp & 0x000000FF) >> 0;

					value = RGB(r, g, b);
				}
			}
			else
			{
				size_t sep1 = str.find(',');
				if (sep1 == str.npos) return;

				size_t sep2 = str.find(',', sep1 + 1);
				if (sep2 == str.npos) return;

				str[sep1] = '\0';
				str[sep2] = '\0';

				auto r = std::stoul(&str[0], 0, 0);
				auto g = std::stoul(&str[sep1 + 1], 0, 0);
				auto b = std::stoul(&str[sep2 + 1], 0, 0);

				value = RGB(r, g, b);
			}
		}

		//
		// コンフィグを読み込みます。
		//
		virtual BOOL on_load(ptree& root) override
		{
			using namespace my::json;

			get_bool(root.get_child("gradient_fill.enable"), hive.gradient_fill.enable);
			get_color(root.get_child("gradient_fill.edge.inner.color"), hive.gradient_fill.edge.inner.color);
			get_int(root.get_child("gradient_fill.edge.inner.size.cx"), hive.gradient_fill.edge.inner.size.cx);
			get_int(root.get_child("gradient_fill.edge.inner.size.cy"), hive.gradient_fill.edge.inner.size.cy);
			get_color(root.get_child("gradient_fill.edge.outer.color"), hive.gradient_fill.edge.outer.color);
			get_int(root.get_child("gradient_fill.edge.outer.size.cx"), hive.gradient_fill.edge.outer.size.cx);
			get_int(root.get_child("gradient_fill.edge.outer.size.cy"), hive.gradient_fill.edge.outer.size.cy);

			get_color(root.get_child("selection.fill.color"), hive.selection.fill.color);
			get_color(root.get_child("selection.stroke.color"), hive.selection.stroke.color);
			get_color(root.get_child("selection.background.color"), hive.selection.background.color);

			get_color(root.get_child("layer.bound.left.color"), hive.layer.bound.left.color);
			get_color(root.get_child("layer.bound.top.color"), hive.layer.bound.top.color);
			get_color(root.get_child("layer.bound.right.color"), hive.layer.bound.right.color);
			get_color(root.get_child("layer.bound.bottom.color"), hive.layer.bound.bottom.color);
			get_color(root.get_child("layer.separator.color"), hive.layer.separator.color);

			return TRUE;
		}

		static std::string from_color(const COLORREF& color)
		{
			if (color == CLR_NONE)
				return std::format(R"("none")");
			else
				return std::format(R"("{}, {}, {}")", GetRValue(color), GetGValue(color), GetBValue(color));
		}

		static std::string from_size(const SIZE& size)
		{
			return std::format(R"({{ "cx": {}, "cy": {} }})", size.cx, size.cy);
		}

		static std::string from_fill(const Hive::Fill& fill)
		{
			return std::format(R"({{ "color": {} }})", from_color(fill.color));
		}

		static std::string from_stroke(const Hive::Stroke& stroke)
		{
			return std::format(R"({{ "color": {} }})", from_color(stroke.color));
		}

		static std::string from_frame_rect(const Hive::FrameRect& frame_rect)
		{
			return std::format(R"({{ "color": {}, "size": {} }})", from_color(frame_rect.color), from_size(frame_rect.size));
		}

		//
		// コンフィグをファイルに保存します。
		//
		virtual BOOL on_save(std::ofstream& ofs) override
		{
			using namespace my::json;

			size_t indent = 0;

			ofs << with_eol(indent++, R"({)"s);
			{
				ofs << with_eol(indent++, R"("gradient_fill": {)"s);
				{
					ofs << with_eol(indent, std::format(R"("enable": {},)", from_bool(hive.gradient_fill.enable)));
					ofs << with_eol(indent++, R"("edge": {)"s);
					{
						ofs << with_eol(indent, std::format(R"("inner": {},)", from_frame_rect(hive.gradient_fill.edge.inner)));
						ofs << with_eol(indent, std::format(R"("outer": {})", from_frame_rect(hive.gradient_fill.edge.outer)));
					}
					ofs << with_eol(--indent, R"(})"s);
				}
				ofs << with_eol(--indent, R"(},)"s);

				ofs << with_eol(indent++, R"("selection": {)"s);
				{
					ofs << with_eol(indent, std::format(R"("fill": {},)", from_fill(hive.selection.fill)));
					ofs << with_eol(indent, std::format(R"("stroke": {},)", from_stroke(hive.selection.stroke)));
					ofs << with_eol(indent, std::format(R"("background": {})", from_fill(hive.selection.background)));
				}
				ofs << with_eol(--indent, R"(},)"s);

				ofs << with_eol(indent++, R"("layer": {)"s);
				{
					ofs << with_eol(indent++, R"("bound": {)"s);
					{
						ofs << with_eol(indent, std::format(R"("left": {},)", from_stroke(hive.layer.bound.left)));
						ofs << with_eol(indent, std::format(R"("top": {},)", from_stroke(hive.layer.bound.top)));
						ofs << with_eol(indent, std::format(R"("right": {},)", from_stroke(hive.layer.bound.right)));
						ofs << with_eol(indent, std::format(R"("bottom": {})", from_stroke(hive.layer.bound.bottom)));
					}
					ofs << with_eol(--indent, R"(},)"s);

					ofs << with_eol(indent, std::format(R"("separator": {})", from_stroke(hive.layer.separator)));
				}
				ofs << with_eol(--indent, R"(})"s);
			}
			ofs << with_eol(--indent, R"(})"s);

			return TRUE;
		}

		virtual int message_box(const std::wstring& text) override
		{
			return hive.message_box(text);
		}
	} config_io;
}
