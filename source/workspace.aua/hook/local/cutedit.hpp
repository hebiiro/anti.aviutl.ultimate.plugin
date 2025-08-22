#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはcutedit.aufをフックします。
	//
	inline struct : Entity {
		//
		// このクラスは::EnumThreadWindows()をフックします。
		//
		inline static struct {
			inline static BOOL WINAPI hook_proc(DWORD thread_id, WNDENUMPROC enum_proc, LPARAM lParam)
			{
				MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", thread_id, enum_proc, lParam);

				// ジャンプウィンドウが存在する場合は
				if (auto shuttle = shuttle_manager.get(L"ジャンプウィンドウ"))
				{
					// ジャンプウィンドウを渡します。
					if (!enum_proc(*shuttle, lParam))
						return TRUE;
				}

				return orig_proc(thread_id, enum_proc, lParam);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} EnumThreadWindows;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto cutedit = ::GetModuleHandle(_T("cutedit.auf"));
			MY_TRACE_HEX(cutedit);
			if (!cutedit) return FALSE;

			// cutedit.auf内の::EnumThreadWindows()をフックします。
			my::hook::attach_import(EnumThreadWindows, cutedit, "EnumThreadWindows");
			MY_TRACE_HEX(EnumThreadWindows.orig_proc);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} cutedit;
}
