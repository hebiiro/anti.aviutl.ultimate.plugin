#pragma once

namespace apn::text_drop
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// exoファイルのパスを返します。
		//
		inline static std::wstring get_exo_path()
		{
			return hive.exo_file_name.length() ?
				magi.get_assets_file_name(hive.exo_file_name) :
				magi.get_assets_file_name(hive.c_display_name) + L".exo";
		}

		//
		// ドロップテキストを返します。
		//
		inline static std::wstring get_drop_text()
		{
			// 選択範囲を使用しない場合は全体を返します。
			if (!hive.selected_text_only) return hive.drop_text;

			// ドロップテキストの選択範囲を取得します。
			int32_t start = 0, end = 0;
			::SendDlgItemMessage(addin_dialog, IDC_DROP_TEXT, EM_GETSEL, (WPARAM)&start, (LPARAM)&end);

			// 選択範囲を正規化します。
			if (end < start) std::swap(start, end);

			// 選択範囲が存在しない場合は全体を返します。
			if (start < 0 || end < 0 || start == end) return hive.drop_text;

			// ドロップテキストの選択範囲のみを返します。
			return hive.drop_text.substr(start, end - start);
		}

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
		// アイテムの長さを返します。
		//
		inline static int32_t get_item_range(int32_t length)
		{
			return hive.item_range + hive.char_range * length;
		}

		//
		// 制御文字を除いた文字数を返します。
		//
		inline static size_t get_length(const std::wstring& line)
		{
			// 文字毎の長さが無効の場合は
			// 文字数を返しても無意味なので0を返します。
			if (hive.char_range <= 0) return 0;

			try
			{
				const std::wregex re(L"<#.*?>|<s.*?>|<r.*?>|<w.*?>|<c.*?>|<p.*?>|");
				const std::wstring fmt = L"";

				return std::regex_replace(line, re, fmt).length();
			}
			catch (...)
			{
			}

			return line.length();
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
		BOOL write_exo_whole(std::ofstream& ofs, const std::wstring& drop_text)
		{
			MY_TRACE_FUNC("");

			// 最大文字数を取得します。
			size_t max_length = 0;
			{
				// 文字列ストリームを開きます。
				std::wstringstream stream(drop_text);

				// ドロップテキストを行毎に走査します。
				std::wstring line;
				while (std::getline(stream, line))
				{
					// キャリッジリターンを除去します。
					if (line.ends_with(L"\r")) line.pop_back();

					max_length = std::max(max_length, get_length(line));
				}
			}

			write_as_exo(ofs, 0, 0, get_item_range(max_length), 0, drop_text);

			return TRUE;
		}

		//
		// ドロップテキストを行別に書き込みます。
		//
		BOOL write_exo_lines(std::ofstream& ofs, const std::wstring& drop_text)
		{
			MY_TRACE_FUNC("");

			auto is_horz = hive.item_direction == hive.c_direction.c_horz;

			auto frame_begin = 0;
			auto layer_set = 0;
			auto index = 0;

			// 文字列ストリームを開きます。
			std::wstringstream stream(drop_text);

			// ドロップテキストを行毎に走査します。
			std::wstring line;
			while (std::getline(stream, line))
			{
				// キャリッジリターンを除去します。
				if (line.ends_with(L"\r")) line.pop_back();

				auto item_range = get_item_range(get_length(line));
				auto frame_offset = item_range + hive.space_range;
				auto layer_offset = 1;

				auto frame_end = frame_begin + item_range;
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
		BOOL write_exo_chars(std::ofstream& ofs, const std::wstring& drop_text)
		{
			MY_TRACE_FUNC("");

			auto is_horz = hive.item_direction == hive.c_direction.c_horz;

			auto frame_begin = 0;
			auto layer_set = 0;
			auto index = 0;

			// ドロップテキストを文字毎に走査します。
			for (size_t i = 0; i < drop_text.length(); i++)
			{
				// キャリッジリターンは無視します。
				if (drop_text[i] == L'\r') continue;

				auto item_range = get_item_range(1);
				auto frame_offset = item_range + hive.space_range;
				auto layer_offset = 1;

				// ラインフィードは改行として処理します。
				if (drop_text[i] == L'\n')
				{
					if (is_horz)
						frame_begin = 0, layer_set += layer_offset;
					else
						layer_set = 0, frame_begin += frame_offset;

					continue;
				}

				auto frame_end = frame_begin + item_range;
				auto text = to_char_string(drop_text[i]);

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
		BOOL write_exo_blocks(std::ofstream& ofs, const std::wstring& drop_text)
		{
			MY_TRACE_FUNC("");

			auto is_horz = hive.item_direction == hive.c_direction.c_horz;

			auto frame_begin = 0;
			auto layer_set = 0;
			auto index = 0;
			std::wstring text;
			size_t max_length = 0;

			// 文字列ストリームを開きます。
			std::wstringstream stream(drop_text);

			// ドロップテキストを行毎に走査します。
			std::wstring line;
			while (std::getline(stream, line))
			{
				// キャリッジリターンを除去します。
				if (line.ends_with(L"\r")) line.pop_back();

				// 空行ではない場合は
				if (line.length())
				{
					max_length = std::max(max_length, get_length(line));

					// テキストを蓄積します。
					text += line + L"\r\n";
				}
				// 空行の場合は段落の終わりとみなします。
				else
				{
					auto item_range = get_item_range(max_length);
					auto frame_offset = item_range + hive.space_range;
					auto layer_offset = 1;

					auto frame_end = frame_begin + item_range;

					write_as_exo(ofs, index++, frame_begin, frame_end, layer_set, text);

					if (is_horz)
						frame_begin += frame_offset;
					else
						layer_set += layer_offset;

					max_length = 0;
					text.clear();
				}
			}

			// テキストが残っている場合は
			if (text.length())
			{
				auto item_range = get_item_range(max_length);

				auto frame_end = frame_begin + item_range;

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

			// ドロップテキストを取得します。
			auto drop_text = get_drop_text();

			// バイナリファイル出力ストリームを開きます。
			std::ofstream ofs(exo_path, std::ios::binary);

			switch (mode)
			{
			case hive.c_mode.c_whole: return write_exo_whole(ofs, drop_text);
			case hive.c_mode.c_lines: return write_exo_lines(ofs, drop_text);
			case hive.c_mode.c_chars: return write_exo_chars(ofs, drop_text);
			case hive.c_mode.c_blocks: return write_exo_blocks(ofs, drop_text);
			}

			return FALSE;
		}

		//
		// exoファイルのドラッグを開始入します。
		//
		virtual BOOL drag_exo(int32_t mode) override
		{
			// exoファイルのパスを取得します。
			auto exo_path = get_exo_path();

			{
#if 1
				// 空のファイルを作成します。
				std::ofstream ofs(exo_path, std::ios::binary);
#else
				// ドロップソースを使用せずに
				// ここでexoファイルを作成することも可能です。
				write_exo(mode, exo_path);
#endif
			}

			// シェルアイテムを作成します。
			ComPtr<IShellItem> shell_item;
			auto hr = ::SHCreateItemFromParsingName(exo_path.c_str(), nullptr, IID_PPV_ARGS(&shell_item));
			MY_TRACE_STR(my::get_error_message(hr));
			if (!shell_item) return FALSE;

			// データオブジェクトを作成します。
			ComPtr<IDataObject> data_object;
			hr = shell_item->BindToHandler(nullptr, BHID_DataObject, IID_PPV_ARGS(&data_object));
			MY_TRACE_STR(my::get_error_message(hr));
			if (!data_object) return FALSE;

			// ドラッグアンドドロップを開始します。
			// ::SHDoDragDrop()の場合はドロップソースをnullptrにすることもできますが、
			// 今回はドロップした瞬間に処理したかったので自前のドロップソースを渡しています。
			auto effect = DWORD {};
			hr = ::SHDoDragDrop(
				hive.main_window,
				data_object.Get(),
				DropSource::Allocator::create(mode, exo_path),
				DROPEFFECT_MOVE | DROPEFFECT_COPY,
				&effect);
			MY_TRACE_STR(my::get_error_message(hr));

			return TRUE;
		}

		//
		// exoファイルをタイムラインに挿入します。
		//
		virtual BOOL insert_exo(int32_t mode) override
		{
			// exoファイルのパスを取得します。
			auto exo_path = get_exo_path();

			// exoファイルを作成します。
			write_exo(mode, exo_path);
#if 1
			auto fp = magi.auin.get_filter_plugin(magi.fp, "拡張編集");
#else
			auto exedit = magi.exin.get_exedit();
			auto fp = *(AviUtl::FilterPlugin**)(exedit + 0x0014D4B4);
#endif
			auto editp = magi.exin.get_editp();
			auto frame = magi.fp->exfunc->get_frame(editp);
			auto layer = std::max(0, hive.insert_layer - 1);

			// 作成したexoファイルをタイムラインに適用します。
			magi.exin.load_exo(my::hs(exo_path).c_str(), frame, layer, fp, editp);

			return TRUE;
		}

		//
		// ドロップテキストをファイルから読み込みます。
		//
		virtual BOOL read_file() override
		{
			// 読み込み先テキストファイルのパスを取得します。
			auto file_name = get_open_file_name(
				hive.main_window,
				L"テキストファイルを選択",
				L"テキストファイル (*.txt)\0*.txt\0"
				L"すべてのファイル (*.*)\0*.*\0",
				hive.text_file_name.c_str());
			if (file_name.empty()) return FALSE;

			hive.text_file_name = file_name;

			// ファイルサイズを取得します。
			auto file_size = (size_t)std::filesystem::file_size(file_name);

			// バイナリファイルストリームを開きます。
			std::ifstream ifs(file_name, std::ios::binary);

			// バッファに読み込みます。
			auto buffer = std::string(
				std::istreambuf_iterator<char>(ifs),
				std::istreambuf_iterator<char>());

			// BOMがUTF-16LEの場合は
			if (buffer.length() >= 2 && buffer[0] == '\xff' && buffer[1] == '\xfe')
			{
				auto data = buffer.data() + 2; // BOMを除外します。
				auto size = buffer.size() - 2; // BOMを除外した分減らします。

				// UTF-16として読み込みます。
				hive.drop_text.assign((LPCWSTR)data, size / sizeof(WCHAR));
			}
			// BOMがUTF-8の場合は
			if (buffer.length() >= 3 && buffer[0] == '\xef' && buffer[1] == '\xbb' && buffer[2] == '\xbf')
			{
				// UTF-8として読み込みます。
				hive.drop_text = my::cp_to_wide(buffer.substr(3), CP_UTF8);
			}
			// それ以外の場合は
			else
			{
				// UTF-8として読み込みます。
				hive.drop_text = my::cp_to_wide(buffer, CP_UTF8);
			}

			// コンフィグが変更されたので
			// ダイアログのコントロールを更新します。
			addin_dialog.update_controls();

			return TRUE;
		}

		//
		// ドロップテキストをファイルに書き込みます。
		//
		virtual BOOL write_file() override
		{
			auto file_name = get_save_file_name(
				hive.main_window,
				L"テキストファイルを選択",
				L"テキストファイル (*.txt)\0*.txt\0"
				L"すべてのファイル (*.*)\0*.*\0",
				hive.text_file_name.c_str(),
				L"txt");
			if (file_name.empty()) return FALSE;

			hive.text_file_name = file_name;

			// ドロップテキストにBOMを付与します。
			auto text = L"\xfeff" + hive.drop_text;

			// バイナリファイルストリームを開きます。
			std::ofstream ofs(file_name, std::ios::binary);

			// ドロップテキストをUTF-16LE(BOMあり)として書き込みます。
			ofs.write((LPCSTR)text.data(), text.size() * sizeof(WCHAR));

			return TRUE;
		}
	} app_impl;
}
