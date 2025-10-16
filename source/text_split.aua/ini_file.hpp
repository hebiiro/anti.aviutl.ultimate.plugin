#pragma once

namespace apn::text_split
{
	//
	// このクラスはiniファイルを管理します。
	//
	struct IniFile
	{
		//
		// このクラスはiniファイルのセクションです。
		//
		struct Section
		{
			std::vector<std::string> keys;
			std::unordered_map<std::string, std::string> entries;

			//
			// エントリを追加します。
			//
			void add_entry(const std::string& key, const std::string& value)
			{
				auto it = entries.find(key);
				if (it == entries.end()) keys.emplace_back(key);
				entries[key] = value;
			}

			//
			// エントリを削除します。
			//
			void erase_entry(const std::string& key)
			{
				auto it = std::find(keys.begin(), keys.end(), key);
				if (it != keys.end()) keys.erase(it);
				entries.erase(key);
			}

			//
			// 指定されたキーに対応するエントリの値を返します。
			//
			std::string get_value(const std::string& key)
			{
				auto it = entries.find(key);
				if (it == entries.end()) return {};
				return it->second;
			}

			//
			// 指定されたキーに対応するエントリの値を設定します。
			//
			void set_value(const std::string& key, const std::string& value)
			{
				return add_entry(key, value);
			}

			//
			// このセクションのクローンを返します。
			//
			std::shared_ptr<Section> clone()
			{
				return std::make_shared<Section>(*this);
			}
		};

		std::vector<std::string> keys;
		std::unordered_map<std::string, std::shared_ptr<Section>> sections;

		//
		// セクションを作成して追加します。
		//
		std::shared_ptr<Section> create_section(const std::string& name)
		{
			return add_section(name, std::make_shared<Section>());
		}

		//
		// セクションを追加します。
		//
		std::shared_ptr<Section> add_section(const std::string& name, const std::shared_ptr<Section>& section)
		{
			auto it = sections.find(name);
			if (it == sections.end()) keys.emplace_back(name);
			return sections[name] = section;
		}

		//
		// セクションを削除します。
		//
		void erase_section(const std::string& name)
		{
			auto it = std::find(keys.begin(), keys.end(), name);
			if (it != keys.end()) keys.erase(it);
			sections.erase(name);
		}

		//
		// 指定されたセクション名に対応するセクションを返します。
		//
		std::shared_ptr<Section> get_section(const std::string& name)
		{
			auto it = sections.find(name);
			if (it == sections.end()) return {};
			return it->second;
		}

		//
		// 指定されたセクション名に対応するセクションを設定します。
		//
		std::shared_ptr<Section> set_section(const std::string& name, const std::shared_ptr<Section>& section)
		{
			return add_section(name, section);
		}

		//
		// 指定されたキーに対応するエントリの値を返します。
		//
		std::string get_value(const std::string& name, const std::string& key)
		{
			if (auto section = get_section(name))
				return section->get_value(key);
			return {};
		}

		//
		// 指定されたキーに対応するエントリの値を設定します。
		//
		void set_value(const std::string& name, const std::string& key, const std::string& value)
		{
			if (auto section = get_section(name))
				return section->set_value(key, value);
		}

		//
		// 指定されたキーに対応するエントリの値を返します。
		//
		std::string get_string(const std::string& name, const std::string& key, const std::string& default_value = {})
		{
			auto value = get_value(name, key);
			if (value.empty()) return default_value;
			return value;
		}

		//
		// 指定されたキーに対応するエントリの値を設定します。
		//
		void set_string(const std::string& name, const std::string& key, const std::string& value)
		{
			return set_value(name, key, value);
		}

		//
		// 指定されたキーに対応するエントリの値を返します。
		//
		int get_int(const std::string& name, const std::string& key, int default_value = {})
		{
			auto value = get_value(name, key);
			if (value.empty()) return default_value;
			return atoi(value.c_str());
		}

		//
		// 指定されたキーに対応するエントリの値を設定します。
		//
		void set_int(const std::string& name, const std::string& key, int value)
		{
			return set_value(name, key, my::format("{/}", value));
		}

		//
		// iniファイルを読み込みます。
		//
		BOOL read_file(const std::string& ini_file_name)
		{
			MY_TRACE_FUNC("{/}", ini_file_name);

			try
			{
				// 現在読込中のセクションです。
				std::shared_ptr<Section> current_section;

				// 入力ストリームを作成します。
				std::ifstream stream(ini_file_name);

				// iniファイルを読み込めなかった場合は
				if (!stream)
				{
					// メッセージボックスでユーザーに通知します。
					hive.message_box(my::format(
						L"{/}を読み込めませんでした", ini_file_name));

					return FALSE;
				}

				// 一行ずつ読み込みます。
				std::string line;
				while (std::getline(stream, line))
				{
					// 末尾にキャリッジリターンが存在する場合は取り除きます。
					if (line.ends_with('\r')) line.pop_back();

					// 先頭がセクションの開始文字の場合は
					if (line.starts_with('['))
					{
						// セクションの終了文字が存在する場合は
						auto end_pos = line.find_first_of(']', 1);
						if (end_pos != line.npos)
						{
							// セクション名を取得します。
							auto key = line.substr(1, end_pos - 1);

							// セクションを新規作成します。
							// そして、現在のセクションに設定します。
							current_section = create_section(key);
						}
					}
					// 先頭がセクションの開始文字ではない場合は
					else if (current_section)
					{
						// エントリの区切り文字が存在する場合は
						auto sep_pos = line.find_first_of('=');
						if (sep_pos != line.npos)
						{
							// エントリのキーを取得します。
							auto key = line.substr(0, sep_pos);

							// エントリの値を取得します。
							auto value = line.substr(sep_pos + 1);

							// 現在のセクションにエントリを追加します。
							current_section->add_entry(key, value);
						}
					}
				}

				return TRUE;
			}
			// 例外が発生した場合は
			catch (const std::exception& error)
			{
				// メッセージボックスでユーザーに通知します。
				hive.message_box(my::ws(error.what()));
			}

			return FALSE;
		}

		//
		// iniファイルに書き込みます。
		//
		BOOL write_file(const std::string& ini_file_name)
		{
			MY_TRACE_FUNC("{/}", ini_file_name);

			try
			{
#if 1
				my::handle::unique_ptr<> file(
					my::create_file_for_write(my::ts(ini_file_name).c_str()));

				const auto write = [&](const std::string& s)
				{
					return ::WriteFile(file.get(), s.data(), s.length(), nullptr, nullptr);
				};

				for (const auto& name : keys)
				{
					auto section = get_section(name);
					if (!section) continue;

					write(my::format("[{/}]" "\r\n", name));

					for (const auto& key : section->keys)
					{
						auto value = section->get_value(key);

						write(my::format("{/}={/}" "\r\n", key, value));
					}
				}

				::FlushFileBuffers(file.get());
#else
				// 出力ストリームを作成します。
				std::ofstream stream(ini_file_name);

				for (const auto& name : keys)
				{
					auto section = get_section(name);
					if (!section) continue;

					stream << my::format("[{/}]", name) << std::endl;

					for (const auto& key : section->keys)
					{
						auto value = section->get_value(key);

						stream << my::format("{/}={/}", key, value) << std::endl;
					}
				}

				// これではフラッシュできないようです。
				stream.flush();

				// fd()は存在しないので、このコードはビルドできません。
				::FlushFileBuffers((HANDLE)_get_osfhandle(stream.rdbuf()->fd()));
#endif
				return TRUE;
			}
			// 例外が発生した場合は
			catch (const std::exception& error)
			{
				// メッセージボックスでユーザーに通知します。
				hive.message_box(my::ws(error.what()));
			}

			return FALSE;
		}
	};
}
