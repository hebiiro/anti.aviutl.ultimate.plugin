#pragma once

namespace apn
{
	//
	// このクラスは他クラスから共通して使用される変数を保持します。
	//
	inline struct Hive
	{
		inline static constexpr LPCWSTR c_name = L"ultimate.auf";
		inline static constexpr LPCWSTR c_display_name = L"アルティメットプラグイン";

		struct Addin
		{
			std::wstring size;
			std::wstring name;
			std::wstring display_name;
			std::vector<std::wstring> conflicts;
			BOOL active = FALSE;
			std::wstring args;

			Addin(const std::wstring& size, const std::wstring& name, const std::wstring& display_name, std::initializer_list<std::wstring> conflicts)
				: size(size)
				, name(name)
				, display_name(display_name)
				, conflicts(conflicts)
			{
			}
		};

		HINSTANCE instance = 0;
		std::vector<std::unique_ptr<Addin>> addins;

		int message_box(const std::wstring& text) {
			return ::MessageBoxW(0, text.c_str(), c_name, MB_OK | MB_ICONWARNING);
		}
	} hive;
}
