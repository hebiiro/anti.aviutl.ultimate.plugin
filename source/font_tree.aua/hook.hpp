#pragma once

namespace apn::font_tree
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
#pragma pack(push)
#pragma pack(1)
		template <typename T>
		inline static void mov_ebx(auto address, T value)
		{
			struct {
				BYTE mov;
				T value;
				BYTE nop;
			} code = {
				0xBB, // MOV EBX
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
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();
			if (!exedit) return FALSE;

			// まず、patch.aulをフックします。
			my::hook::attach_call(Unknown, exedit + 0x008B97F);

			// patch.aulをフックできなかった場合は
			if (!Unknown.orig_proc)
			{
				// 拡張編集をフックします。
				SendMessageA.orig_proc = ::SendMessageA;
				mov_ebx(exedit + 0x0008B956, SendMessageA.hook_proc);
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

		//
		// このクラスは拡張編集内の::SendMessageA()の呼び出しをフックします。
		//
		inline static struct {
			inline static LRESULT WINAPI hook_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

				// まず、デフォルト処理を実行します。
				auto result = orig_proc(hwnd, message, wParam, lParam);

				// フォント名が取得されようとしている場合は
				if (message == CB_GETLBTEXT)
				{
					// フォント名を取得します。
					auto font_name = (LPCSTR)lParam;

					// フォント名を使用履歴に追加します。
					recent_manager.insert(my::ws(font_name).c_str());
				}

				// デフォルト処理の結果を返します。
				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::SendMessageA;
		} SendMessageA;

		//
		// このクラスはpatch.aul内の関数をフックします。
		//
		inline static struct {
			inline static LRESULT WINAPI hook_proc(HWND hwnd, ExEdit::Filter* filter, int32_t font_index, LPCSTR font_name)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}, {}, {}", hwnd, filter, font_index, font_name);

				// まず、デフォルト処理を実行します。
				auto result = orig_proc(hwnd, filter, font_index, font_name);

				// フォント名を使用履歴に追加します。
				recent_manager.insert(my::ws(font_name).c_str());

				// デフォルト処理の結果を返します。
				return result;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} Unknown;
	} hook_manager;
}
