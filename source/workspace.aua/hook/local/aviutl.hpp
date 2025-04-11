#pragma once

namespace apn::workspace::hook::local
{
	inline namespace
	{
		struct WindowPlacement
		{
			HWND hwnd = nullptr;
			WINDOWPLACEMENT wp = { sizeof(wp) };

			WindowPlacement(HWND hwnd)
				: hwnd(hwnd)
			{
				// ウィンドウの位置を取得します。
				::GetWindowPlacement(hwnd, &wp);

				// ウィンドウを最大化します。
				::ShowWindow(hwnd, SW_MAXIMIZE);

				// ウィンドウを一番手前に持ってきます。
				::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}

			~WindowPlacement()
			{
				// ウィンドウの位置を元に戻します。
				::SetWindowPlacement(hwnd, &wp);
			}
		};

		inline BOOL movieplaymain = FALSE;

		namespace play_main
		{
			inline my::addr_t orig = 0;

			inline __declspec(naked) uint32_t __fastcall do_default(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5, uint32_t u6)
			{
				// origは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

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
					CALL orig
					ADD ESP, 0x10 // orig_proc()は__cdeclで終わります。
				}

				__asm {
					mov esp, ebp
					pop ebp
					retn 0x10 // __fastcallで終わります。
				}
			}

			inline uint32_t __fastcall do_hook(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5, uint32_t u6)
			{
				MY_TRACE_FUNC("");

				// フラグが立っていない場合はデフォルト処理のみ行います。
				if (!hive.fullscreen_preview)
					return do_default(u1, u2, u3, u4, u5, u6);

				// AviUtlウィンドウまたは再生ウィンドウを取得します。
				auto hwnd = ::GetParent(hive.aviutl_window);

				if (!movieplaymain)
				{
					auto shuttle = shuttle_manager.get(L"再生ウィンドウ");
					if (shuttle) hwnd = ::GetParent(*shuttle);
				}

				// ウィンドウを最大化します。その後、スコープから抜けたときに元に戻します。
				WindowPlacement wp(hwnd);

				// デフォルト処理を行います。
				return do_default(u1, u2, u3, u4, u5, u6);
			}

			inline __declspec(naked) uint32_t __fastcall hook(
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

			inline BOOL init(my::addr_t aviutl)
			{
				MY_TRACE_FUNC("{/hex}", aviutl);

				orig = aviutl + 0x00053320;
				MY_TRACE_HEX(orig);

				return TRUE;
			}

			inline LONG enable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourAttach(&(PVOID&)orig, hook);
			}

			inline LONG disable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourDetach(&(PVOID&)orig, hook);
			}
		}

		namespace play_sub
		{
			inline my::addr_t orig = 0;

			inline __declspec(naked) uint32_t __fastcall do_default(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5)
			{
				// origは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

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
					CALL orig
					ADD ESP, 0x0C // orig_proc()は__cdeclで終わります。
				}

				__asm {
					mov esp, ebp
					pop ebp
					retn 0x0C // __fastcallで終わります。
				}
			}

			inline uint32_t __fastcall do_hook(
				uint32_t u1, uint32_t u2, uint32_t u3, uint32_t u4, uint32_t u5)
			{
				MY_TRACE_FUNC("");

				// フラグが立っていない場合はデフォルト処理のみ行います。
				if (!hive.fullscreen_preview)
					return do_default(u1, u2, u3, u4, u5);

				// AviUtlウィンドウまたは再生ウィンドウを取得します。
				auto hwnd = ::GetParent(hive.aviutl_window);

				if (!movieplaymain)
				{
					auto shuttle = shuttle_manager.get(L"再生ウィンドウ");
					if (shuttle) hwnd = ::GetParent(*shuttle);
				}

				// ウィンドウを最大化します。その後、スコープから抜けたときに元に戻します。
				WindowPlacement wp(hwnd);

				// デフォルト処理を行います。
				return do_default(u1, u2, u3, u4, u5);
			}

			inline __declspec(naked) uint32_t __fastcall hook(
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

			inline BOOL init(my::addr_t aviutl)
			{
				MY_TRACE_FUNC("{/hex}", aviutl);

				orig = aviutl + 0x00051150;
				MY_TRACE_HEX(orig);

				return TRUE;
			}

			inline LONG enable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourAttach(&(PVOID&)orig, hook);
			}

			inline LONG disable_hook()
			{
				MY_TRACE_FUNC("");

				return DetourDetach(&(PVOID&)orig, hook);
			}
		}
	}

	inline struct AviUtl : Entity
	{
		//
		// このクラスは::AdjustWindowRectEx()をフックします。
		// NC領域が確保されないようにします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPRECT rc, DWORD style, BOOL menu, DWORD ex_style)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}", style, menu, ex_style);

				return TRUE;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} AdjustWindowRectEx;

		//
		// このクラスはdraw_aviutl_button_as_play()をフックします。
		// スペースキーで再生を開始したときに呼ばれます。
		//
		inline static struct {
			inline static void __fastcall hook_proc(HWND hwnd, HICON icon)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}", hwnd, icon);

				// ボタンにアイコンをセットします。
				::SetWindowLongA(hwnd, 0, (LONG)icon);

				// デフォルト処理で上書き描画するのではなく、ボタンを再描画します。
				my::invalidate(hwnd);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_aviutl_button_as_play;

		//
		// このクラスはdraw_aviutl_button_as_stop()をフックします。
		// スペースキーで再生を停止したときに呼ばれます。
		//
		inline static struct {
			inline static void __fastcall hook_proc(HWND hwnd, HICON icon)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}", hwnd, icon);

				// ボタンにアイコンをセットします。
				::SetWindowLongA(hwnd, 0, (LONG)icon);

				// デフォルト処理で上書き描画するのではなく、ボタンを再描画します。
				my::invalidate(hwnd);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} draw_aviutl_button_as_stop;

		//
		// この関数は::ShowWindow()の処理を省略します。
		//
		inline static BOOL WINAPI omit_ShowWindow(HWND hwnd, int cmd_show)
		{
//			MY_TRACE_FUNC("{/hex}, {/}", hwnd, cmd_show);
//			MY_TRACE_HWND(hwnd);

			return TRUE;
		}

		//
		// この関数は::UpdateWindow()の処理を省略します。
		//
		inline static BOOL WINAPI omit_UpdateWindow(HWND hwnd)
		{
//			MY_TRACE_FUNC("{/hex}", hwnd);
//			MY_TRACE_HWND(hwnd);

			return TRUE;
		}

		//
		// この関数は::SetWindowPos()の処理を省略します。
		//
		inline static BOOL WINAPI omit_SetWindowPos(HWND hwnd, HWND insert_after, int x, int y, int w, int h, UINT flags)
		{
//			MY_TRACE_FUNC("{/hex}", hwnd);
//			MY_TRACE_HWND(hwnd);

			return TRUE;
		}

#pragma pack(push)
#pragma pack(1)
		template <typename T>
		inline static void mov_ecx(auto address, T value)
		{
			struct {
				BYTE mov;
				T value;
				BYTE nop;
			} code = {
				0xB9, // MOV ECX
				value,
				0x90, // NOP
			};
			static_assert(sizeof(code) == 6);

			auto process = ::GetCurrentProcess();
			auto result1 = ::WriteProcessMemory(process, (LPVOID)address, &code, sizeof(code), nullptr);
			auto result2 = ::FlushInstructionCache(process, (LPVOID)address, sizeof(code));
		}
#pragma pack(pop)

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_dll_init() override
		{
			MY_TRACE_FUNC("");

			// AviUtlのモジュールハンドルを取得します。
			auto aviutl = (my::addr_t)::GetModuleHandle(nullptr);
			MY_TRACE_HEX(aviutl);

			// AviUtl内の::AdjustWindowRectEx()のコールをフックします。
			my::hook::attach_abs_call(AdjustWindowRectEx, aviutl + 0x3281D); // ヒストグラム
			my::hook::attach_abs_call(AdjustWindowRectEx, aviutl + 0x5CAA9); // ツールウィンドウ
			MY_TRACE_HEX(AdjustWindowRectEx.orig_proc);

			// AviUtl内のdraw_aviutl_button_as_play()のコールをフックします。
			my::hook::attach_call(draw_aviutl_button_as_play, aviutl + 0x53383);
			MY_TRACE_HEX(draw_aviutl_button_as_play.orig_proc);

			// AviUtl内のdraw_aviutl_button_as_stop()のコールをフックします。
			my::hook::attach_call(draw_aviutl_button_as_stop, aviutl + 0x536A1);
			MY_TRACE_HEX(draw_aviutl_button_as_stop.orig_proc);

			// AviUtlのiniファイルのパスを取得します。
			auto file_name = my::get_module_file_name(nullptr).parent_path() / _T("aviutl.ini");
			MY_TRACE_STR(file_name);

			// AviUtlの設定を取得します。
			movieplaymain = ::GetPrivateProfileInt(_T("system"), _T("movieplaymain"), 0, file_name.c_str());

			// フック用のアドレスを初期化します。
			play_main::init(aviutl);
			play_sub::init(aviutl);

			if (hive.omit_window_initialize)
			{
				// AviUtl内のウィンドウ初期化処理をフックします。
				mov_ecx(aviutl + 0x2CBE8, omit_ShowWindow);
				mov_ecx(aviutl + 0x2CBDC, omit_UpdateWindow);
				mov_ecx(aviutl + 0x2CC3C, omit_SetWindowPos);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_dll_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// フックを有効/無効化します。
		//
		BOOL enable_hook(BOOL enable)
		{
			MY_TRACE_FUNC("{/}", enable);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			if (enable)
			{
				play_main::enable_hook();
				play_sub::enable_hook();
			}
			else
			{
				play_main::disable_hook();
				play_sub::disable_hook();
			}

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("APIフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}
		}
	} aviutl;
}
