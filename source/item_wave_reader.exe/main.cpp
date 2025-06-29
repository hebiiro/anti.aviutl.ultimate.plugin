#include "pch.h"
#include "ffmpeg/common.hpp"
#include "ffmpeg/session.hpp"
#include "app.hpp"

namespace apn::item_wave::reader
{
	//
	// このクラスはエントリポイントです。
	//
	struct WinMain
	{
		//
		// コンストラクタです。
		//
		WinMain()
		{
			_tsetlocale(LC_CTYPE, _T(""));

			auto suffix = L"_" + std::to_wstring(::GetCurrentProcessId());

			my::tracer_to_file::init(nullptr, suffix.c_str());

			MY_TRACE_FUNC("");
		}

		//
		// デストラクタです。
		//
		~WinMain()
		{
			MY_TRACE_FUNC("");

			my::tracer_to_file::exit();
		}

		//
		// メイン処理です。
		//
		int main(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
		{
			MY_TRACE_FUNC("{/}", cmd_line);

			try
			{
				app.main();

				MY_TRACE("プロセスが正常終了しました\n");

				return 0;
			}
			catch (...)
			{
				MY_TRACE("プロセスが異常終了しました\n");

				return 20250512;
			}
		}
	};
}

int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int cmd_show)
{
	apn::item_wave::reader::WinMain win_main;

	return win_main.main(instance, prev_instance, cmd_line, cmd_show);
}
