#pragma once

namespace apn::dark::kuro
{
	//
	// 指定されたカラーエントリを結合します。
	//
	inline void operator+=(color_entry_t& lhs, const color_entry_t* rhs)
	{
		// 右辺のカラーエントリが無効の場合は何もしません。
		if (!rhs) return;

		// 右辺のカラーパーツを走査します。
		for (size_t i = 0; i < rhs->c_max_size; i++)
		{
			// 右辺のカラーパーツが有効の場合は
			if (rhs->parts[i].is_valid())
			{
				// 左辺のカラーパーツを上書きします。
				lhs.parts[i] = rhs->parts[i];
			}
		}
	}

	//
	// このクラスはcustom_color.confの変数を保持します。
	//
	inline struct custom_style_t : entry_t::node_t
	{
		virtual BOOL on_init() { return init(); }
		virtual BOOL on_exit() { return exit(); }

		//
		// カラーエントリのコレクションです。
		//
		std::unordered_map<std::pair<std::wstring, std::wstring>, color_entry_t> color_entries;

		//
		// 指定されたセクションとキーに対応するカラーエントリを返します。
		//
		const color_entry_t* get_color_entry(const std::wstring& section, const std::wstring& key) const
		{
			auto it = color_entries.find({ section, key });
			if (it == color_entries.end()) return {};
			return &it->second;
		}

		//
		// 指定された条件のカラーエントリを返します。
		//
		inline color_entry_t get_color_entry(const std::wstring& section, const std::wstring& key, const color_entry_t& default_color_entry)
		{
			// 一旦デフォルト値をセットします。
			auto out_color_entry = default_color_entry;

			// カラーエントリを結合します
			out_color_entry += get_color_entry(section, key);

			// 出力カラーエントリを返します。
			return out_color_entry;
		}

		//
		// バックグランドのカラーエントリを返します。
		//
		inline color_entry_t get_background_color_entry(const std::wstring& section, const std::wstring& sub_key, const color_entry_t& default_color_entry)
		{
			return get_color_entry(section, sub_key + L"Fill", default_color_entry);
		}

		//
		// ボーダーのカラーエントリを返します。
		//
		inline color_entry_t get_border_color_entry(const std::wstring& section, const std::wstring& sub_key, const color_entry_t& default_color_entry)
		{
			return get_color_entry(section, sub_key + L"Edge", default_color_entry);
		}

		//
		// テキストのカラーエントリを返します。
		//
		inline color_entry_t get_text_color_entry(const std::wstring& section, const std::wstring& sub_key, const color_entry_t& default_color_entry)
		{
			return get_color_entry(section, sub_key + L"Text", default_color_entry);
		}

		//
		// テキストの影のカラーエントリを返します。
		//
		inline color_entry_t get_text_shadow_color_entry(const std::wstring& section, const std::wstring& sub_key)
		{
			// 出力カラーエントリです。
			auto out_color_entry = color_entry_t {};

			// 各カラーエントリを出力カラーエントリに結合します。
			out_color_entry += get_color_entry(L"Common", L"TextShadow");
			out_color_entry += get_color_entry(section, sub_key + L"TextShadow");

			// 出力カラーエントリを返します。
			return out_color_entry;
		}

		//
		// 値をカラーパーツに変換してコレクションに追加します。
		//
		void add_color_parts(const std::wstring& section, const std::wstring& key, const std::wstring& value)
		{
			// 引数が無効の場合は何もしません。
			if (section.empty() || key.empty() || value.empty()) return;

			// 値を配列に分割します。
			auto vec = split(value, L',');

			// 最大数を取得します。
			auto c = std::min(color_entry_t::c_max_size, vec.size());

			// コレクションに追加予定のカラーエントリです。
			auto color_entry = color_entry_t {};

			// 配列を走査します。
			for (size_t i = 0; i < c; i++)
			{
				// 文字列を取得します。
				const auto& str = vec[i];

				// 文字列が空の場合は何もしません。
				if (str.empty()) continue;

				// 取得予定のRGBAです。
				auto rgba = rgba_t {};

				// 文字列の長さで分岐します。
				switch (str.length())
				{
				// rrggbb形式の場合は
				case 6:
					{
						// 文字列をRGBAに変換します。
						rgba.value = wcstoul(str.data(), nullptr, 16) << 8;
						rgba.a = 0xff;

						break;
					}
				// rrggbbaa形式の場合は
				case 8:
					{
						// 文字列をRGBAに変換します。
						rgba.value = wcstoul(str.data(), nullptr, 16);

						break;
					}
				// それ以外の場合は
				default:
					{
						// 何もしません。
						continue;
					}
				}

				// RGBAをカラーエントリに追加します。
				color_entry.parts[i] = { rgba };
			}

			// カラーエントリをコレクションに追加します。
			color_entries[{ section, key }] = color_entry;
		}

		//
		// カスタムカラーファイルを読み込みます。
		//
		BOOL read_custom_color_file(const std::wstring& custom_color_path)
		{
			MY_TRACE_FUNC("{/}", custom_color_path);

			// 既存のカラーエントリを消去します。
			color_entries.clear();

			// 現在のセクションです。
			auto section = std::wstring {};

			// ファイルストリームを開きます。UTF-8です。
			std::ifstream stream(custom_color_path);

			// 一行ずつ読み込みます。
			auto utf8_line = std::string {};
			while (std::getline(stream, utf8_line))
			{
				// ワイド文字列に変換します。
				auto line = my::cp_to_wide(utf8_line, CP_UTF8);

				// 前後の空白を削除します。
				line = trim(line);

				// 空行は無視します。
				if (line.empty()) continue;

				// コメント行の場合は無視します。
				if (line.starts_with(L';')) continue;

				// セクション行の場合は
				if (line.starts_with(L'[') && line.ends_with(L']'))
				{
					// セクションを更新します。
					section = line.substr(1, line.length() - 2);

					// ループを続けます。
					continue;
				}

				// セパレータの位置を取得します。
				auto separator_pos = line.find_first_of(L'=');

				// セパレータの位置が無効の場合は無視します。
				if (separator_pos == line.npos) continue;

				// キーを取得します。
				auto key = trim(line.substr(0, separator_pos));

				// 値を取得します。
				auto value = trim(line.substr(separator_pos + 1));

				// コレクションに追加します。
				add_color_parts(section, key, value);
			}

			return TRUE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			try
			{
				// カスタムカラーファイルのサブパスを作成します。
				auto sub_path = std::filesystem::path(hive.c_name) / L"custom_color.conf";

				// カスタムカラーファイルのパスを取得します。
				auto assets_path = magi.get_assets_file_name(sub_path);
				auto config_path = magi.get_config_file_name(sub_path);

				// コンフィグファイルが存在しない場合は
				if (!std::filesystem::exists(config_path))
				{
					// アセットファイルををコピーします。
					std::filesystem::copy(assets_path, config_path);
				}

				// カスタムカラーファイルを読み込みます。
				read_custom_color_file(config_path);

				// ファイルパスをハイブにセットします。
				hive.dark.custom_color_file_name = config_path;
			}
			// 例外が発生した場合は
			catch (const std::exception& error)
			{
				// メッセージボックスでユーザーに通知します。
				hive.message_box(my::ws(error.what()));
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} custom_style;
}
