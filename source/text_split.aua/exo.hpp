#pragma once

namespace apn::text_split
{
	//
	// このクラスはexoファイルを管理します。
	//
	struct exo_t
	{
		//
		// ファイルパスです。
		//
		const path_t& path;

		//
		// 元のexoファイルです。
		//
		IniFile orig_file;

		//
		// アイテム分解後のexoファイルです。
		//
		IniFile part_file;

		BOOL copy_section(
			const std::string& src_section_name,
			const std::string& dst_section_name)
		{
			if (auto section = orig_file.get_section(src_section_name))
				return part_file.add_section(dst_section_name, section->clone()), TRUE;
			return FALSE;
		}

		//
		// exoファイルから文字列を読み込みます。
		//
		auto read_string(
			const std::string& section_name,
			const std::string& key_name,
			const std::string& default_value = {})
		{
			return orig_file.get_string(section_name, key_name, default_value);
		}

		//
		// exoファイルから整数を読み込みます。
		//
		auto read_int(
			const std::string& section_name,
			const std::string& key_name,
			int32_t default_value = {})
		{
			return orig_file.get_int(section_name, key_name, default_value);
		}

		//
		// exoファイルに文字列を書き込みます。
		//
		auto write_string(
			const std::string& section_name,
			const std::string& key_name,
			const std::string& value)
		{
			return orig_file.set_string(section_name, key_name, value);
		}

		//
		// exoファイルに整数を書き込みます。
		//
		auto write_int(
			const std::string& section_name,
			const std::string& key_name,
			int32_t value)
		{
			return orig_file.set_int(section_name, key_name, value);
		}

		//
		// exoファイルにアイテムの位置情報を書き込みます。
		//
		auto write_pos(
			const std::string& section_name,
			const std::string& key_name,
			const pos_t::value_t& value, const pos_t& pos)
		{
			// posを文字列に変換します。
			auto str = pos.to_string(value);
			MY_TRACE_STR(str);

			return write_string(section_name, key_name, str);
		}

		//
		// コンストラクタです。
		//
		exo_t(const path_t& path)
			: path(path)
		{
			MY_TRACE_FUNC("");

			// 現在のシーンからexoファイルを作成します。
			magi.exin.save_exo(path.temp_file_name.c_str());

			// 作成されたexoファイルを読み込みます。
			orig_file.read_file(path.temp_file_name);

			// [exedit]セクションを分解後ファイルにコピーします。
			copy_section("exedit", "exedit");
		}

		//
		// デストラクタです。
		//
		~exo_t()
		{
			// 不要になったテンポラリファイルを削除します。
			std::filesystem::remove(path.temp_file_name);
			std::filesystem::remove(path.each_temp_file_name);
		}
	};
}
