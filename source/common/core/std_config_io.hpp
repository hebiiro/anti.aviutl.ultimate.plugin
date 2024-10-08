﻿#pragma once

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
			struct Updater {
				StdConfigIO* config_io;
				Updater(StdConfigIO* config_io) : config_io(config_io) {}
				~Updater() { config_io->update(); }
			} updater(this);

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
					config_file_name, my::ws(error.what())));

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
					config_file_name, my::ws(error.what())));

				return FALSE;
			}

			return TRUE;
		}

		//
		// この仮想関数はコンフィグが更新されたときに呼び出されます。
		//
		virtual BOOL update() { return FALSE; }

		//
		// 指定されたストリームから設定を読み込みます。
		//
		virtual BOOL read_stream(std::ifstream& ifs)
		{
			nlohmann::json root;
			ifs >> root;
			return read_node(root);
		}

		//
		// 指定されたプロパティツリーから設定を読み込みます。
		//
		virtual BOOL read_node(nlohmann::json& root)
		{
			return TRUE;
		}

		//
		// 指定されたストリームに設定を保存します。
		//
		virtual BOOL write_stream(std::ofstream& ofs)
		{
			nlohmann::json root;
			write_node(root);
			ofs << root.dump(1, '\t');
			return TRUE;
		}

		//
		// 指定されたプロパティツリーに設定を保存します。
		//
		virtual BOOL write_node(nlohmann::json& root)
		{
			return TRUE;
		}
	};
}
