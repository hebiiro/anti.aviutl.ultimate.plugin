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
		// ロックカウントです。
		//
		int lock_count = 0;

		virtual void on_update_controls() {}
		virtual void on_update_config() {}
		virtual void on_init_dialog() {}
		virtual void on_command(UINT code, UINT id, HWND control) {}
		virtual void on_notify(NMHDR* nmhdr) {}

		//
		// ロックされている場合はTRUEを返します。
		// ロックされているときはWM_COMMANDを無視します。
		//
		BOOL is_locked() const { return lock_count != 0; }

		//
		// ロックします。
		//
		void lock() { lock_count++; }

		//
		// ロックを解除します。
		//
		void unlock() { lock_count--; }

		//
		// このクラスはダイアログをロックします。
		//
		struct Locker {
			StdAddinDialog* dialog;
			Locker(StdAddinDialog* dialog) : dialog(dialog) { dialog->lock(); }
			~Locker() { dialog->unlock(); }
		};

		//
		// 初期化処理を実行します。
		//
		BOOL init(HINSTANCE instance, HWND parent)
		{
			MY_TRACE_FUNC("{/hex}, {/hex}", instance, parent);

			// 初期化中にエディットボックスがコマンドを発行してしまうので、
			// それを防ぐためにロックしておきます。
			Locker locker(this);

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

			Locker locker(this);

			on_update_controls();

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
		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {/hex}, {/hex}", w_param, l_param);

					// 派生クラスにWM_INITDIALOGを処理させます。
					on_init_dialog();

					break;
				}
			case WM_COMMAND:
			case AviUtl::FilterPlugin::WindowMessage::Command:
				{
					MY_TRACE_FUNC("WM_COMMAND, {/hex}, {/hex}", w_param, l_param);

					if (is_locked()) break;

					auto code = HIWORD(w_param);
					auto id = LOWORD(w_param);
					auto control = (HWND)l_param;

					// 派生クラスにWM_COMMANDを処理させます。
					on_command(code, id, control);

					break;
				}
			case WM_NOTIFY:
				{
					auto nmhdr = (NMHDR*)l_param;

					// 派生クラスにWM_NOTIFYを処理させます。
					on_notify(nmhdr);

					break;
				}
			}

			return __super::on_dlg_proc(hwnd, message, w_param, l_param);
		}
	};
}
