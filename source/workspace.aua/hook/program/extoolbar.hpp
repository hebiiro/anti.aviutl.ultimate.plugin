#pragma once

namespace apn::workspace::hook
{
	//
	// このクラスはextoolbar.aufをフックします。
	//
	inline struct {
		//
		// このクラスは::GetMenuState()をフックします。
		//
		inline static struct {
			inline static UINT WINAPI hook_proc(HMENU menu, UINT id, UINT flags)
			{
				MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}", menu, id, flags);

				auto result = orig_proc(menu, id, flags);
				if (result == -1) return 0;
				return result;
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} GetMenuState;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto extoolbar = ::GetModuleHandle(_T("extoolbar.auf"));
			MY_TRACE_HEX(extoolbar);
			if (!extoolbar) return FALSE;

			// extoolbar.auf内の::GetMenuState()をフックします。
			my::hook::attach_import(GetMenuState, extoolbar, "GetMenuState");
			MY_TRACE_HEX(GetMenuState.orig_proc);

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
	} extoolbar;
}
