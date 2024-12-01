#pragma once

namespace apn::reboot::spreader
{
	inline namespace
	{
		//
		// ウィンドウが表示されているスクリーンの矩形を返します。
		//
		inline auto get_screen_rect(HWND hwnd)
		{
			auto monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
			MONITORINFOEX monitor_info = {};
			monitor_info.cbSize = sizeof(monitor_info);
			::GetMonitorInfo(monitor, &monitor_info);
			return monitor_info.rcMonitor;
		}

		//
		// 指定されたメッセージをメッセージキューから取り除きます。
		//
		inline void remove_message(HWND hwnd, uint32_t message)
		{
			auto msg = MSG {};
			while (::PeekMessage(&msg, hwnd, message, message, PM_REMOVE)) {}
		}

		//
		// このクラスはプレイヤーを一時的に表示します。
		//
		struct ShowPlayer
		{
			//
			// コンストラクタです。
			//
			ShowPlayer(HWND hwnd)
			{
				// スクリーン矩形を取得します。
				auto rc = get_screen_rect(hwnd);

				remove_message(player, agit.c_message.c_show_window);
				my::set_window_pos(player, HWND_TOP, &rc, SWP_SHOWWINDOW);
			}

			//
			// デストラクタです。
			//
			~ShowPlayer()
			{
				::PostMessage(player, agit.c_message.c_show_window, SW_HIDE, 0);
			}
		};

		inline uint32_t play(const auto& call_orig_proc)
		{
			MY_TRACE_FUNC("");

			// フラグが立っていない場合はデフォルト処理のみ行います。
			if (!agit.enabled)
				return call_orig_proc();

			// プレイヤーを表示します。その後、
			// スコープから抜けたときに非表示にします。
			ShowPlayer show_player(agit.preview_window);

			// デフォルト処理を行います。
			return call_orig_proc();
		}

		//
		// aviutlウィンドウで再生する場合の処理です。
		//
		namespace play_main
		{
			//
			// オリジナルの関数です。
			//
			inline my::addr_t orig_proc = 0;

			//
			// オリジナルの関数を実行します。
			//
			inline __declspec(naked) uint32_t __fastcall call_orig_proc(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5, uint32_t u6)
			{
				// orig_procは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov u1, ecx
					mov u2, edx
				}

				__asm {
					// __fastcallとして引数を積み直します。
					MOV EAX, u6
					PUSH EAX
					MOV EAX, u5
					PUSH EAX
					MOV EAX, u4
					PUSH EAX
					MOV EAX, u3
					PUSH EAX
					MOV EDX, u2
					MOV ECX, u1
					CALL orig_proc
					ADD ESP, 0x10 // orig_proc()は__cdeclで終わります。
				}

				__asm {
					mov esp, ebp
					pop ebp
					retn 0x10 // __fastcallで終わります。
				}
			}

			//
			// フック処理を実行します。
			//
			inline uint32_t __fastcall do_hook(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5, uint32_t u6)
			{
				MY_TRACE_FUNC("");

				return play([&](){ return call_orig_proc(u1, u2, u3, u4, u5, u6); });
			}

			//
			// フック関数です。
			//
			inline __declspec(naked) uint32_t __fastcall hook_proc(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5, uint32_t u6)
			{
				// この関数は__fastcallで始まり、__cdeclで終わります。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov u1, ecx
					mov u2, edx
				}

				__asm {
					// 引数を積み直します。
					MOV EAX, u6
					PUSH EAX
					MOV EAX, u5
					PUSH EAX
					MOV EAX, u4
					PUSH EAX
					MOV EAX, u3
					PUSH EAX
					MOV EDX, u2
					MOV ECX, u1
					CALL do_hook
				}

				__asm {
					mov esp, ebp
					pop ebp
					ret // __cdeclで終わります。
				}
			}

			//
			// フックを初期化します。
			//
			inline BOOL init(my::addr_t aviutl)
			{
				MY_TRACE_FUNC("{:#010x}", aviutl);

				orig_proc = aviutl + 0x00053320;
				MY_TRACE_HEX(orig_proc);

				return TRUE;
			}

			//
			// フックを有効化します。
			//
			inline LONG enable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourAttach(&(PVOID&)orig_proc, hook_proc);
			}

			//
			// フックを無効化します。
			//
			inline LONG disable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourDetach(&(PVOID&)orig_proc, hook_proc);
			}
		}

		//
		// 再生ウィンドウで再生する場合の処理です。
		//
		namespace play_sub
		{
			//
			// オリジナルの関数です。
			//
			inline my::addr_t orig_proc = 0;

			//
			// オリジナルの関数を実行します。
			//
			inline __declspec(naked) uint32_t __fastcall call_orig_proc(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5)
			{
				// orig_procは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov u1, ecx
					mov u2, edx
				}

				__asm {
					// __fastcallとして引数を積み直します。
					MOV EAX, u5
					PUSH EAX
					MOV EAX, u4
					PUSH EAX
					MOV EAX, u3
					PUSH EAX
					MOV EDX, u2
					MOV ECX, u1
					CALL orig_proc
					ADD ESP, 0x0C // orig_proc()は__cdeclで終わります。
				}

				__asm {
					mov esp, ebp
					pop ebp
					retn 0x0C // __fastcallで終わります。
				}
			}

			//
			// フック処理を実行します。
			//
			inline uint32_t __fastcall do_hook(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5)
			{
				MY_TRACE_FUNC("");

				return play([&](){ return call_orig_proc(u1, u2, u3, u4, u5); });
			}

			//
			// フック関数です。
			//
			inline __declspec(naked) uint32_t __fastcall hook_proc(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5)
			{
				// この関数は__fastcallで始まり、__cdeclで終わります。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov u1, ecx
					mov u2, edx
				}

				__asm {
					// 引数を積み直します。
					MOV EAX, u5
					PUSH EAX
					MOV EAX, u4
					PUSH EAX
					MOV EAX, u3
					PUSH EAX
					MOV EDX, u2
					MOV ECX, u1
					CALL do_hook
				}

				__asm {
					mov esp, ebp
					pop ebp
					ret // __cdeclで終わります。
				}
			}

			//
			// フックを初期化します。
			//
			inline BOOL init(my::addr_t aviutl)
			{
				MY_TRACE_FUNC("{:#010x}", aviutl);

				orig_proc = aviutl + 0x00051150;
				MY_TRACE_HEX(orig_proc);

				return TRUE;
			}

			//
			// フックを有効化します。
			//
			inline LONG enable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourAttach(&(PVOID&)orig_proc, hook_proc);
			}

			//
			// フックを無効化します。
			//
			inline LONG disable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourDetach(&(PVOID&)orig_proc, hook_proc);
			}
		}
	}
}
