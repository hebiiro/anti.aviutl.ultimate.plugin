#pragma once

namespace apn
{
	//
	// このクラスは標準的なアドインダイアログです。
	//
	template <UINT dialog_id, typename T = my::layout::Window<my::NoCloseDialog>>
	struct StdAddinDialog : T
	{
		//
		// このフラグがTRUEのときはWM_COMMANDを無視します。
		//
		BOOL locked = FALSE;

		virtual void on_update_controls() {}
		virtual void on_update_config() {}
		virtual void on_init_dialog() {}
		virtual void on_command(UINT code, UINT id, HWND control) {}

		//
		// ロックされている場合はTRUEを返します。
		//
		BOOL is_locked() const { return locked; }

		//
		// ロックします。
		//
		void lock() { locked = TRUE; }

		//
		// ロックを解除します。
		//
		void unlock() { locked = FALSE; }

		//
		// 初期化処理を実行します。
		//
		BOOL init(HINSTANCE instance, HWND parent)
		{
			MY_TRACE_FUNC("{:#010x}, {:#010x}", instance, parent);

			return __super::create(instance, MAKEINTRESOURCE(dialog_id), parent);
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return __super::destroy();
		}

		//
		// コントロールを更新します。
		//
		BOOL update_controls()
		{
			MY_TRACE_FUNC("");

			if (is_locked()) return FALSE;

			lock();
			on_update_controls();
			unlock();

			return TRUE;
		}

		//
		// コンフィグを更新します。
		//
		BOOL update_config()
		{
			MY_TRACE_FUNC("");

			if (is_locked()) return FALSE;

			on_update_config();

			return TRUE;
		}

		//
		// ダイアログプロシージャです。
		//
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

					on_init_dialog();

					break;
				}
			case WM_COMMAND:
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("WM_COMMAND, {:#010x}, {:#010x}", wParam, lParam);

					auto code = HIWORD(wParam);
					auto id = LOWORD(wParam);
					auto control = (HWND)lParam;

					on_command(code, id, control);

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, wParam, lParam);
		}
	};
}
