#pragma once

//
// このクラスは他クラスから共通して使用される変数を保持します。
//
inline struct Hive
{
	//
	// モジュールフォルダパスです。
	//
	std::filesystem::path module_folder_name;

	//
	// コンフィグのファイル名です。
	//
	std::wstring config_file_name;

	//
	// 定義のファイル名です。
	//
	std::wstring spec_file_name;

	//
	// インストール先のAviUtlのフォルダパスです。
	//
	std::filesystem::path aviutl_dir;

	//
	// インストール先のAviUtlのファイルパスです。
	//
	std::filesystem::path aviutl_file_name;

	//
	// インストール先の拡張編集のファイルパスです。
	//
	std::filesystem::path exedit_file_name;

	//
	// インストール元のディレクトリです。
	//
	std::filesystem::path from;

	//
	// インストール先のディレクトリです。
	//
	std::filesystem::path to;

	//
	// インストール結果です。
	//
	struct Result
	{
		BOOL succeeded;
		std::wstring from, to;

		Result(BOOL succeeded, const std::wstring& from, const std::wstring& to)
			: succeeded(succeeded), from(from), to(to)
		{
		}
	};
	std::vector<Result> results;

	//
	// TRUEの場合は新バージョンをインストールします。
	//
	BOOL install_new_version = TRUE;

	//
	// TRUEの場合は旧バージョンを削除します。
	//
	BOOL uninstall_old_version = TRUE;

	//
	// TRUEの場合はランタイムDLLを配置します。
	//
	BOOL deploy_runtime = FALSE;

	//
	// TRUEの場合はaviutl_dark.exeを作成します。
	//
	BOOL create_dark_exe = TRUE;
} hive;
