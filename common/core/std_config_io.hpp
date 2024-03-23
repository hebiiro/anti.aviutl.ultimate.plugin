#pragma once

namespace apn
{
	//
	// このクラスはコンフィグの入出力を担当します。
	//
	struct StdConfigIO
	{
		virtual BOOL on_load(ptree& root) = 0;
		virtual BOOL on_save(std::ofstream& ofs) = 0;
		virtual int message_box(const std::wstring& text) = 0;

		//
		// コンフィグファイルのフルパスを返します。
		//
		std::wstring get_config_file_name(HINSTANCE instance)
		{
			return magi.get_config_file_name(instance);
		}

		//
		// 初期化処理を実行します。
		//
		virtual BOOL init(HINSTANCE instance)
		{
			// コンフィグファイル名を取得し、設定を読み込みます。
			load(get_config_file_name(instance));

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL exit(HINSTANCE instance)
		{
			// コンフィグファイル名を取得し、設定を保存します。
			save(get_config_file_name(instance));

			return TRUE;
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		virtual BOOL load(const std::wstring& config_file_name)
		{
			if (!std::filesystem::exists(config_file_name)) return FALSE;

			using namespace my::json;

			ptree root;
			try
			{
				std::ifstream ifs(config_file_name);
				read_json(ifs, root);

				on_load(root);
			}
			catch (const std::exception& error)
			{
				message_box(std::format(
					L"{}を読込中にエラーが発生しました\n{}",
					config_file_name, my::cp_to_wide(error.what(), CP_ACP)));

				return FALSE;
			}

			return TRUE;
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		virtual BOOL save(const std::wstring& config_file_name)
		{
			try
			{
				std::ofstream ofs(config_file_name);

				on_save(ofs);
			}
			catch (const std::exception& error)
			{
				message_box(std::format(
					L"{}を保存中にエラーが発生しました\n{}",
					config_file_name, my::cp_to_wide(error.what(), CP_ACP)));

				return FALSE;
			}

			return TRUE;
		}
	};
}
