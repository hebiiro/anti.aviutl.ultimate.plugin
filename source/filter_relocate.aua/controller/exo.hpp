#pragma once

namespace apn::filter_relocate::controller
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
		// 加工前のexoファイルです。
		//
		ini_file_t before_file;

		//
		// 加工後のexoファイルです。
		//
		ini_file_t after_file;

		//
		// 加工前exoファイルから加工後exoファイルへセクションをコピーします。
		//
		BOOL copy_section(
			const std::string& src_section_name,
			const std::string& dst_section_name)
		{
			if (auto section = before_file.get_section(src_section_name))
				return after_file.add_section(dst_section_name, section->clone()), TRUE;
			return FALSE;
		}

		//
		// 加工前exoファイルから文字列を読み込みます。
		//
		auto read_string(
			const std::string& section_name,
			const std::string& key_name,
			const std::string& default_value = {})
		{
			return before_file.get_string(section_name, key_name, default_value);
		}

		//
		// 加工前exoファイルから整数を読み込みます。
		//
		auto read_int(
			const std::string& section_name,
			const std::string& key_name,
			int32_t default_value = {})
		{
			return before_file.get_int(section_name, key_name, default_value);
		}

		//
		// 加工後exoファイルにセクションを作成します。
		//
		auto create_section(const std::string& section_name)
		{
			return after_file.create_section(section_name);
		}

		//
		// 加工後exoファイルに文字列を書き込みます。
		//
		auto write_string(
			const std::string& section_name,
			const std::string& key_name,
			const std::string& value)
		{
			return after_file.set_string(section_name, key_name, value);
		}

		//
		// 加工後exoファイルに整数を書き込みます。
		//
		auto write_int(
			const std::string& section_name,
			const std::string& key_name,
			int32_t value)
		{
			return after_file.set_int(section_name, key_name, value);
		}

		//
		// コンストラクタです。
		//
		exo_t(const path_t& path)
			: path(path)
		{
			MY_TRACE_FUNC("");

			// 現在のシーンからexoファイルを作成します。
			magi.exin.save_exo(path.before_file_name.c_str());

			// 作成されたexoファイルを読み込みます。
			before_file.read_file(path.before_file_name);

			// [exedit]セクションを加工後exoファイルにコピーします。
			copy_section("exedit", "exedit");
		}

		//
		// デストラクタです。
		//
		~exo_t()
		{
			// 不要になったexoファイルを削除します。
			std::filesystem::remove(path.before_file_name);
			std::filesystem::remove(path.after_file_name);
		}
	};
}
