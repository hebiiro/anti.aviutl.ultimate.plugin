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
				MY_TRACE_FUNC("share::c_message.c_reboot, {/hex}, {/hex}", wParam, lParam);

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
					auto wait_result = ::WaitForSingleObject(process_handle.get(), INFINITE);
					MY_TRACE_INT(wait_result);

					// aviutlのパスを取得します。
					auto path = std::filesystem::path(buffer->path);
					MY_TRACE_STR(path);

					// aviutlの引数を作成します。
					// 引数に空白が含まれている場合があるので
					// 二重引用符で括って対処します。
					auto args = my::format(_T("\"{/}\""), buffer->args);
					MY_TRACE_STR(args);

					// aviutlフォルダを取得します。
					auto dir = path.parent_path();
					MY_TRACE_STR(dir);

					// 新しいaviutlプロセスを起動します。
					share::shell_execute(_T("open"), path.c_str(), args.c_str(), dir.c_str());
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
