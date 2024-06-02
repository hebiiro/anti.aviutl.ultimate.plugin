#pragma once

namespace apn::workspace::hook
{
	inline namespace
	{
		struct WindowPlacement
		{
			HWND hwnd = 0;
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
				if (!hive.fullscreen_player)
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

			inline LONG init(my::addr_t aviutl)
			{
				orig = aviutl + 0x00053320;

				return DetourAttach((PVOID*)&orig, hook);
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
				if (!hive.fullscreen_player)
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

			inline LONG init(my::addr_t aviutl)
			{
				orig = aviutl + 0x00051150;

				return DetourAttach((PVOID*)&orig, hook);
			}
		}
	}

	inline struct AviUtl
	{
		//
		// このクラスは::AdjustWindowRectEx()をフックします。
		// カラーパレットがドッキングしているマイパレットを表示するのを防ぎます。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(LPRECT rc, DWORD style, BOOL menu, DWORD ex_style)
			{
				MY_TRACE_FUNC("{:#010x}, {}, {:#010x}", style, menu, ex_style);

				return TRUE;
			}
			inline static decltype(&hook_proc) orig_proc = 0;
		} AdjustWindowRectEx;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// AviUtlのモジュールハンドルを取得します。
			auto aviutl = (my::addr_t)::GetModuleHandle(nullptr);
			MY_TRACE_HEX(aviutl);

			// AviUtl内の::AdjustWindowRectEx()をフックします。
			my::hook::attach_import(AdjustWindowRectEx, (HMODULE)aviutl, "AdjustWindowRectEx");
			MY_TRACE_HEX(AdjustWindowRectEx.orig_proc);

			// 最大化再生を使用する場合は
			if (hive.use_fullscreen_player)
			{
				// AviUtl内の再生関係の関数をフックします。

				auto file_name = my::get_module_file_name(nullptr).parent_path() / _T("aviutl.ini");
				MY_TRACE_STR(file_name);

				movieplaymain = ::GetPrivateProfileInt(_T("system"), _T("movieplaymain"), 0, file_name.c_str());

				play_main::init(aviutl);
				play_sub::init(aviutl);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} aviutl;
}
