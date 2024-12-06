#pragma once

namespace apn::reboot::spreader
{
	//
	// naked関数を使用するのでクラスは使用できません。
	//
	inline namespace
	{
		//
		// 関数フックをセットします。
		//
		template <typename HookProc, typename OrigProc, typename A>
		LONG attach(HookProc& hook_proc, OrigProc& orig_proc, A address)
		{
			orig_proc = reinterpret_cast<decltype(orig_proc)>(address);
			return DetourAttach(&(PVOID&)orig_proc, hook_proc);
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
				player.show(hwnd);
			}

			//
			// デストラクタです。
			//
			~ShowPlayer()
			{
				player.hide();
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
		namespace main_play_start
		{
			//
			// オリジナルの関数です。
			//
			inline my::addr_t orig_proc = 0;

			//
			// オリジナルの関数を実行します。
			//
			inline __declspec(naked) uint32_t __fastcall call_orig_proc(
				HWND preview_window, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp, int32_t play_frame_begin, int32_t play_frame_end, uint32_t command_id)
			{
				// orig_procは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov preview_window, ecx
					mov editp, edx
				}

				__asm {
					// __fastcallとして引数を積み直します。
					MOV EAX, command_id
					PUSH EAX
					MOV EAX, play_frame_end
					PUSH EAX
					MOV EAX, play_frame_begin
					PUSH EAX
					MOV EAX, fp
					PUSH EAX
					MOV EDX, editp
					MOV ECX, preview_window
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
				HWND preview_window, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp,
				int32_t play_frame_begin, int32_t play_frame_end, uint32_t command_id)
			{
				MY_TRACE_FUNC("");

				auto orig_proc = [&](){ return call_orig_proc(preview_window, editp, fp, play_frame_begin, play_frame_end, command_id); };

				// 保存中の場合は
				if (fp->exfunc->is_saving(editp))
				{
					// デフォルト処理を実行します。
					return orig_proc();
				}

				// 選択範囲だけを再生する場合は
				if (agit.play_select_frame)
				{
					// 再生範囲を選択範囲に変更します。
					fp->exfunc->get_select_frame(editp, &play_frame_begin, &play_frame_end);
				}

				return play(orig_proc);
			}

			//
			// フック関数です。
			//
			inline __declspec(naked) uint32_t __fastcall hook_proc(
				HWND preview_window, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp,
				int32_t play_frame_begin, int32_t play_frame_end, uint32_t command_id)
			{
				// この関数は__fastcallで始まり、__cdeclで終わります。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov preview_window, ecx
					mov editp, edx
				}

				__asm {
					// 引数を積み直します。
					MOV EAX, command_id
					PUSH EAX
					MOV EAX, play_frame_end
					PUSH EAX
					MOV EAX, play_frame_begin
					PUSH EAX
					MOV EAX, fp
					PUSH EAX
					MOV EDX, editp
					MOV ECX, preview_window
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

				attach(hook_proc, orig_proc, aviutl + 0x00053320);

				return TRUE;
			}
		}

		//
		// 再生ウィンドウで再生する場合の処理です。
		//
		namespace sub_play_start
		{
			//
			// オリジナルの関数です。
			//
			inline my::addr_t orig_proc = 0;

			//
			// オリジナルの関数を実行します。
			//
			inline __declspec(naked) uint32_t __fastcall call_orig_proc(
				HWND preview_window, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp,
				int32_t play_frame_begin, int32_t play_frame_end)
			{
				// orig_procは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov preview_window, ecx
					mov editp, edx
				}

				__asm {
					// __fastcallとして引数を積み直します。
					MOV EAX, play_frame_end
					PUSH EAX
					MOV EAX, play_frame_begin
					PUSH EAX
					MOV EAX, fp
					PUSH EAX
					MOV EDX, editp
					MOV ECX, preview_window
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
				HWND preview_window, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp,
				int32_t play_frame_begin, int32_t play_frame_end)
			{
				MY_TRACE_FUNC("");

				auto orig_proc = [&](){ return call_orig_proc(preview_window, editp, fp, play_frame_begin, play_frame_end); };

				// 保存中の場合は
				if (fp->exfunc->is_saving(editp))
				{
					// デフォルト処理を実行します。
					return orig_proc();
				}

				return play(orig_proc);
			}

			//
			// フック関数です。
			//
			inline __declspec(naked) uint32_t __fastcall hook_proc(
				HWND preview_window, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp,
				int32_t play_frame_begin, int32_t play_frame_end)
			{
				// この関数は__fastcallで始まり、__cdeclで終わります。

				__asm {
					push ebp
					mov ebp, esp
					sub esp, __LOCAL_SIZE
					mov preview_window, ecx
					mov editp, edx
				}

				__asm {
					// 引数を積み直します。
					MOV EAX, play_frame_end
					PUSH EAX
					MOV EAX, play_frame_begin
					PUSH EAX
					MOV EAX, fp
					PUSH EAX
					MOV EDX, editp
					MOV ECX, preview_window
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

				attach(hook_proc, orig_proc, aviutl + 0x00051150);

				return TRUE;
			}
		}

		//
		// aviutlウィンドウで再生を停止する関数をフックします。
		//
		namespace main_play_stop
		{
			inline struct {
				inline static uint32_t __fastcall hook_proc(HWND preview_window)
				{
					MY_TRACE_FUNC("{:#010x}", preview_window);

					auto is_playing = magi.auin.is_playing();
					auto is_playing_main = magi.auin.is_playing_main();

					// 再生が中止されていない場合は
					if (magi.auin.is_playing())
					{
						// ループ再生する場合は
						if (agit.play_loop)
						{
							// 再生ウィンドウに再生コマンドをポストします。
							::PostMessage(preview_window, WM_COMMAND, 100, 0);
						}
					}

					return orig_proc(preview_window);
				}
				inline static decltype(&hook_proc) orig_proc = nullptr;
			} hook;

			//
			// フックを初期化します。
			//
			inline BOOL init(my::addr_t aviutl)
			{
				MY_TRACE_FUNC("{:#010x}", aviutl);

				my::hook::attach_call(hook, aviutl + 0x00053629);

				return TRUE;
			}
		}
	}
}
