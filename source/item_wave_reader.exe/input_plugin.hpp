#pragma once

namespace apn::item_wave::reader
{
	struct InputPlugin
	{
		HINSTANCE aui = nullptr;
		AviUtl::InputPluginDLL* dll = nullptr;

		//
		// コンストラクタです。
		//
		InputPlugin(LPCTSTR file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			// 入力プラグインをロードします。
			aui = ::LoadLibrary(file_name);
			MY_TRACE_HEX(aui);
			if (!aui) return;

			// 入力プラグインのエクスポート関数を取得します。
			auto GetInputPluginTable = (AviUtl::InputPluginDLL* (WINAPI*)())::GetProcAddress(aui, "GetInputPluginTable");
			MY_TRACE_HEX(GetInputPluginTable);
			if (!GetInputPluginTable) return;

			// 入力プラグインの構造体を取得します。
			dll = GetInputPluginTable();
			MY_TRACE_HEX(dll);
			if (!dll) return;

			// 入力プラグインを初期化します。
			if (dll->func_init)
				dll->func_init();
		}

		//
		// デストラクタです。
		//
		~InputPlugin()
		{
			MY_TRACE_FUNC("");

			if (!aui) return;

			// 入力プラグインの後始末をします。
			if (dll && dll->func_exit)
				dll->func_exit();

			// 入力プラグインをアンロードします。
			::FreeLibrary(aui), aui = nullptr;
		}
	};
}
