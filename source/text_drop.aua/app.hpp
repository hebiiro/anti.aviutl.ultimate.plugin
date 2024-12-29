#pragma once

namespace apn::text_drop
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// 指定された文字列を16進数文字列にして返します。
		//
		inline static std::string to_hex_string(const std::wstring& text)
		{
			uint8_t code[2048] = {};
			memcpy(code, text.data(), std::min(sizeof(code), text.size() * sizeof(WCHAR)));

			std::string hex_string;
			for (size_t i = 0; i < std::size(code); i++)
				hex_string += std::format("{:02x}", code[i]);
			return hex_string;
		}

		//
		// 文字を文字列に変換して返します。
		//
		inline static std::wstring to_char_string(WCHAR ch)
		{
#if 0
			// 無効な文字を判別します。
			switch (ch)
			{
			case L'\r':
			case L'\n':
				return {};
			}
#endif
			return { 1, ch };
		}

		//
		// exo形式で書き込みます。
		//
		inline static void write_as_exo(
			std::ofstream& ofs,
			int32_t index,
			int32_t frame_begin,
			int32_t frame_end,
			int32_t layer_set,
			const std::wstring& text)
		{
			ofs << std::format("[{}]\r\n", index);
			ofs << std::format("start={}\r\n", frame_begin + 1);
			ofs << std::format("end={}\r\n", frame_end);
			ofs << std::format("layer={}\r\n", layer_set + 1);
			ofs << "overlay=1\r\n";
			ofs << "camera=0\r\n";
			ofs << std::format("[{}.0]\r\n", index);
			ofs << "_name=テキスト\r\n";
			ofs << std::format("text={}\r\n", to_hex_string(text));
			ofs << std::format("[{}.1]\r\n", index);
			ofs << "_name=標準描画\r\n";
		}

		//
		// ドロップテキストをまとめて書き込みます。
		//
		BOOL write_exo_whole(std::ofstream& ofs)
		{
			MY_TRACE_FUNC("");

			write_as_exo(ofs, 0, 0, hive.item_range, 0, hive.drop_text);

			return TRUE;
		}

		//
		// ドロップテキストを行別に書き込みます。
		//
		BOOL write_exo_lines(std::ofstream& ofs)
		{
			MY_TRACE_FUNC("");

			auto is_horz = hive.item_direction == hive.c_direction.c_horz;

			auto frame_begin = 0;
			auto layer_set = 0;

			auto frame_offset = hive.item_range + hive.space_range;
			auto layer_offset = 1;

			auto index = 0;

			// 文字列ストリームを開きます。
			std::wstringstream stream(hive.drop_text);

			// ドロップテキストを行毎に走査します。
			std::wstring line;
			while (std::getline(stream, line))
			{
				// キャリッジリターンを除去します。
				if (line.ends_with(L"\r")) line.pop_back();

				auto frame_end = frame_begin + hive.item_range;
				auto text = line;

				write_as_exo(ofs, index++, frame_begin, frame_end, layer_set, text);

				if (is_horz)
					frame_begin += frame_offset;
				else
					layer_set += layer_offset;
			}

			return TRUE;
		}

		//
		// ドロップテキストを文字別に書き込みます。
		//
		BOOL write_exo_chars(std::ofstream& ofs)
		{
			MY_TRACE_FUNC("");

			auto is_horz = hive.item_direction == hive.c_direction.c_horz;

			auto frame_begin = 0;
			auto layer_set = 0;

			auto frame_offset = hive.item_range + hive.space_range;
			auto layer_offset = 1;

			auto index = 0;

			// ドロップテキストを文字毎に走査します。
			for (size_t i = 0; i < hive.drop_text.length(); i++)
			{
				// キャリッジリターンは無視します。
				if (hive.drop_text[i] == L'\r') continue;

				// ラインフィードは改行として処理します。
				if (hive.drop_text[i] == L'\n')
				{
					if (is_horz)
						frame_begin = 0, layer_set += layer_offset;
					else
						layer_set = 0, frame_begin += frame_offset;

					continue;
				}

				auto frame_end = frame_begin + hive.item_range;
				auto text = to_char_string(hive.drop_text[i]);

				write_as_exo(ofs, index++, frame_begin, frame_end, layer_set, text);

				if (is_horz)
					frame_begin += frame_offset;
				else
					layer_set += layer_offset;

			}

			return TRUE;
		}

		//
		// ドロップテキストを段落別に書き込みます。
		//
		BOOL write_exo_blocks(std::ofstream& ofs)
		{
			MY_TRACE_FUNC("");

			auto is_horz = hive.item_direction == hive.c_direction.c_horz;

			auto frame_begin = 0;
			auto layer_set = 0;

			auto frame_offset = hive.item_range + hive.space_range;
			auto layer_offset = 1;

			auto index = 0;
			std::wstring text;

			// 文字列ストリームを開きます。
			std::wstringstream stream(hive.drop_text);

			// ドロップテキストを行毎に走査します。
			std::wstring line;
			while (std::getline(stream, line))
			{
				// キャリッジリターンを除去します。
				if (line.ends_with(L"\r")) line.pop_back();

				// 空行ではない場合は
				if (line.length())
				{
					// テキストを蓄積します。
					text += line + L"\r\n";
				}
				// 空行の場合は段落の終わりとみなします。
				else
				{
					auto frame_end = frame_begin + hive.item_range;

					write_as_exo(ofs, index++, frame_begin, frame_end, layer_set, text);

					if (is_horz)
						frame_begin += frame_offset;
					else
						layer_set += layer_offset;

					text.clear();
				}
			}

			// テキストが残っている場合は
			if (text.length())
			{
				auto frame_end = frame_begin + hive.item_range;

				write_as_exo(ofs, index++, frame_begin, frame_end, layer_set, text);
			}

			return TRUE;
		}

		//
		// exoファイルに書き込みます。
		//
		virtual BOOL write_exo(int32_t mode, const std::wstring& exo_path) override
		{
			MY_TRACE_FUNC("");

			// バイナリファイル出力ストリームを開きます。
			std::ofstream ofs(exo_path, std::ios::binary);

			switch (mode)
			{
			case hive.c_mode.c_whole: return write_exo_whole(ofs);
			case hive.c_mode.c_lines: return write_exo_lines(ofs);
			case hive.c_mode.c_chars: return write_exo_chars(ofs);
			case hive.c_mode.c_blocks: return write_exo_blocks(ofs);
			}

			return FALSE;
		}
	} app_impl;
}
