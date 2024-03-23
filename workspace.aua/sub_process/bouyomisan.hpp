#pragma once

namespace apn::workspace
{
	//
	// このクラスは棒読みさん用のシャトルです。
	//
	struct Bouyomisan : SubProcess
	{
		virtual HWND find_window() override
		{
			HWND result = 0;
			::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
				// ウィンドウ名をチェックします。
				auto window_text = my::get_window_text(hwnd);
				if (window_text.find( _T("棒読みさん")) == window_text.npos) return TRUE;

				// クラス名をチェックします。
				auto class_name = my::get_class_name(hwnd);
				if (class_name.find(_T("Bouyomisan")) == class_name.npos) return TRUE;

				// 棒読みさんが見つかったので、FALSEを返して列挙を終了します。
				return *(HWND*)lParam = hwnd, FALSE;
			}, (LPARAM)&result);

			return result;
		}
	}; inline std::shared_ptr<Bouyomisan> bouyomisan;
}
