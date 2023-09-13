#pragma once
#include "SubProcess.h"

namespace fgo::nest
{
	//
	// このクラスは棒読みさん用のシャトルです。
	//
	struct Bouyomisan : SubProcess
	{
		HWND findWindow() override
		{
			HWND result = 0;
			::EnumWindows([](HWND hwnd, LPARAM lParam) -> BOOL {
				// ウィンドウ名をチェックします。
				TCHAR windowText[MAX_PATH] = {};
				::GetWindowText(hwnd, windowText, std::size(windowText));
				if (!_tcsstr(windowText, _T("棒読みさん"))) return TRUE;

				// クラス名をチェックします。
				TCHAR className[MAX_PATH] = {};
				::GetClassName(hwnd, className, std::size(className));
				if (!_tcsstr(className, _T("Bouyomisan"))) return TRUE;

				// 棒読みさんが見つかったので、FALSEを返して列挙を終了します。
				*(HWND*)lParam = hwnd;
				return FALSE;
			}, (LPARAM)&result);

			return result;
		}
	}; inline std::shared_ptr<Bouyomisan> bouyomisan;
}
