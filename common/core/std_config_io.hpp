#pragma once

namespace apn
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	struct StdConfigIO
	{
		//
		// 指定されたファイルからコンフィグを読み込みます。
		//
		BOOL read_file(const std::wstring& config_file_name, auto& error_handler)
		{
			if (!std::filesystem::exists(config_file_name)) return FALSE;

			try
			{
				std::ifstream ifs(config_file_name);

				return read_stream(ifs);
			}
			catch (const std::exception& error)
			{
				error_handler.message_box(std::format(
					L"{}を読込中にエラーが発生しました\n{}",
					config_file_name, my::cp_to_wide(error.what(), CP_ACP)));

				return FALSE;
			}

			return TRUE;
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL write_file(const std::wstring& config_file_name, auto& error_handler)
		{
			try
			{
				std::ofstream ofs(config_file_name);

				return write_stream(ofs);
			}
			catch (const std::exception& error)
			{
				error_handler.message_box(std::format(
					L"{}を保存中にエラーが発生しました\n{}",
					config_file_name, my::cp_to_wide(error.what(), CP_ACP)));

				return FALSE;
			}

			return TRUE;
		}

		//
		// 指定されたストリームから設定を読み込みます。
		//
		virtual BOOL read_stream(std::ifstream& ifs)
		{
			ptree root;
			read_json(ifs, root);
			return read_node(root);
		}

		//
		// 指定されたプロパティツリーから設定を読み込みます。
		//
		virtual BOOL read_node(ptree& root)
		{
			return TRUE;
		}

		//
		// 指定されたストリームに設定を保存します。
		//
		virtual BOOL write_stream(std::ofstream& ofs)
		{
			ptree root;
			if (!write_node(root)) return FALSE;
			write_json(ofs, root);
			return TRUE;
		}

		//
		// 指定されたプロパティツリーに設定を保存します。
		//
		virtual BOOL write_node(ptree& root)
		{
			return TRUE;
		}
	};
}
