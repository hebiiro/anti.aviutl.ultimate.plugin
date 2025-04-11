#pragma once

namespace apn::editbox_tweaker
{
	//
	// このクラスはフック処理を担当します。
	//
	inline struct HookManager
	{
		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		// 拡張編集の定数を書き換えます。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();

			my::hook::attach(GetMessageA);

			if (hive.unicode_input)
			{
				my::hook::attach(DispatchMessageA);
				// ::PeekMessageA()を::PeekMessageW()に置き換えてしまうと
				// フックしていないエディットボックスで日本語入力ができなくなってしまうので
				// ::PeekMessageA()は置き換えないようにします。
//				my::hook::attach(PeekMessageA);
			}

			// 拡張編集内の::CreateWindowExW()の呼び出しをフックします。
			// 定数も書き換えます。
			if (hive.delta || hive.font.handle)
			{
				my::hook::attach_abs_call(CreateWindowExW, exedit + 0x0008C46E);
				my::hook::attach_abs_call(CreateWindowExW, exedit + 0x00087658);
				my::tools::add_int32(exedit + 0x0008CC56 + 1, hive.delta);
				my::tools::add_int32(exedit + 0x000876DE + 1, hive.delta);
			}

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::GetMesageA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(LPMSG msg, HWND hwnd, UINT msgFilterMin, UINT msgFilterMax)
			{
				auto result = FALSE;
				if (hive.unicode_input)
				{
					// ::GetMessageA()の代わりに::GetMessageW()を呼び出します。
					result = ::GetMessageW(msg, hwnd, msgFilterMin, msgFilterMax);
				}
				else
				{
					// ::GetMessageA()を呼び出します。
					result = orig_proc(msg, hwnd, msgFilterMin, msgFilterMax);
				}

				// Ctrl+Aが押された場合は
				if (hive.ctrl_a &&
					msg->message == WM_KEYDOWN &&
					msg->wParam == 'A' &&
					::GetKeyState(VK_CONTROL) < 0 &&
					my::get_class_name(msg->hwnd) == WC_EDITW)
				{
					// テキスト全選択します。
					::SendMessageW(msg->hwnd, EM_SETSEL, 0, -1);

					// このメッセージはディスパッチしません。その代わり、
					// ::GetMessageA()を呼び出して次のメッセージまで待機します。
					return orig_proc(msg, hwnd, msgFilterMin, msgFilterMax);
				}

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::GetMessageA;
		} GetMessageA;

		//
		// このクラスは::CreateWindowExW()をフックします。
		//
		struct {
			inline static LRESULT CALLBACK subclass_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR id, DWORD_PTR)
			{
				switch (message)
				{
				case WM_SETFONT:
					{
						MY_TRACE("WM_SETFONT, {/hex}, {/hex}\n", wParam, lParam);

						wParam = reinterpret_cast<WPARAM>(hive.font.handle.get());

						[[fallthrough]]; // breakせずにこのまま処理を続けます。
					}
				case WM_DESTROY:
					{
						::RemoveWindowSubclass(hwnd, subclass_proc, id);

						break;
					}
				}

				return ::DefSubclassProc(hwnd, message, wParam, lParam);
			}

			//
			// 拡張編集内の複数行エディットボックスを作成する関数(::CreateWindowExW())と置き換えられます。
			//
			inline static HWND WINAPI hook_proc(DWORD ex_style, LPCWSTR class_name, LPCWSTR window_name, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				MY_TRACE_FUNC("{/}, {/}, {/}", class_name, w, h);

				auto hwnd = orig_proc(ex_style, class_name, window_name,
					style, x, y, w, h + hive.delta, parent, menu, instance, param);

				if (hive.zoomable)
					::SendMessage(hwnd, EM_SETEXTENDEDSTYLE, ES_EX_ZOOMABLE, ES_EX_ZOOMABLE);

				if (hive.font.handle)
					::SetWindowSubclass(hwnd, subclass_proc, 0, 0);

				return hwnd;
			}
			inline static decltype(&hook_proc) orig_proc = ::CreateWindowExW;
		} CreateWindowExW;

		//
		// この構造体は::DispatchMesageA()を::DispatchMesageW()に置き換えるために使用されます。
		//
		struct {
			constexpr static auto& hook_proc = ::DispatchMessageW;
			inline static decltype(&hook_proc) orig_proc = ::DispatchMessageA;
		} DispatchMessageA;

		//
		// この構造体は::PeekMesageA()を::PeekMesageW()に置き換えるために使用されます。
		//
		struct {
			constexpr static auto& hook_proc = ::PeekMessageW;
			inline static decltype(&hook_proc) orig_proc = ::PeekMessageA;
		} PeekMessageA;
	} hook_manager;
}
