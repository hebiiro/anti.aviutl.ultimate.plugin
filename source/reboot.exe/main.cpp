#include "pch.h"

namespace apn::reboot
{
	//
	// スレッドプロシージャです。
	//
	LRESULT on_thread_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case share::c_message.c_reboot:
			{
				MY_TRACE_FUNC("share::c_message.c_reboot, {:#010x}, {:#010x}", wParam, lParam);

				// 引数から既存のaviutlプロセスのハンドルを取得します。
				my::handle::unique_ptr<> process_handle((HANDLE)wParam);

				// 引数から共有メモリを取得します。
				my::handle::unique_ptr<> shared_handle((HANDLE)lParam);
				my::SharedMemory<share::RebootSignal> shared(shared_handle.get());

				// 共有メモリが取得できた場合は
				if (auto buffer = shared.get())
				{
					MY_TRACE_STR(buffer->path);
					MY_TRACE_STR(buffer->args);

					// 既存のaviutlプロセスが終了するまで待機します。
					auto result1 = ::WaitForSingleObject(process_handle.get(), INFINITE);
					MY_TRACE_INT(result1);

					// 新しいaviutlプロセスを起動します。
					share::shell_execute(_T("open"), buffer->path, buffer->args);
				}

				// メッセージループを終了します。
				::PostQuitMessage(0);

				break;
			}
		}

		return 0;
	}

	//
	// エントリポイントです。
	//
	int win_main(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int show_cmd)
	{
		_tsetlocale(LC_CTYPE, _T(""));

		my::tracer_to_file::init(instance);

		::CoInitialize(nullptr);

		MY_TRACE_FUNC("");

		auto msg = MSG {};
		while (::GetMessage(&msg, nullptr, 0, 0) > 0)
		{
			if (!msg.hwnd)
				on_thread_proc(msg.hwnd, msg.message, msg.wParam, msg.lParam);
		}

		MY_TRACE("プロセスを終了します\n");

		::CoUninitialize();

		my::tracer_to_file::exit();

		return 0;
	}
}

//
// エントリポイントです。
//
int APIENTRY _tWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPTSTR cmd_line, int show_cmd)
{
	return apn::reboot::win_main(instance, prev_instance, cmd_line, show_cmd);
}
