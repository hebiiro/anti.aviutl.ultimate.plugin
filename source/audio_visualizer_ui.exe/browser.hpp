#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはブラウザです。
	//
	struct Browser : apn::Browser
	{
		inline static constexpr struct Message {
			inline static constexpr uint32_t c_run_async = WM_APP + 1;
		} c_message;

		//
		// 非同期でコールバック関数を実行します。
		//
		void run_async(std::function<void()> callback)
		{
			auto* task = new std::function<void()>(std::move(callback));

			::PostMessage(*this, c_message.c_run_async, reinterpret_cast<WPARAM>(task), 0);
		}

		//
		// 非同期でメッセージボックスを表示します。
		//
		void message_box_async(std::wstring message, std::wstring title)
		{
			run_async([this, message = std::move(message), title = std::move(title)] {
				::MessageBox(*this, message.c_str(), title.c_str(), MB_OK);
			});
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case c_message.c_run_async:
				{
					MY_TRACE_FUNC("c_run_async, {:#010x}, {:#010x}", wParam, lParam);

					auto* task = reinterpret_cast<std::function<void()>*>(wParam);
					(*task)();
					delete task;

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}

		//
		// この仮想関数は、エラーが発生したときに呼ばれます。
		//
		virtual BOOL on_error(const std::wstring& what) override
		{
			return hive.message_box(what, *this);
		}
	};
}
