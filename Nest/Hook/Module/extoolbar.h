#pragma once

namespace fgo::nest::hook
{
	//
	// このクラスはextoolbar.aufをフックします。
	//
	struct extoolbar
	{
		//
		// このクラスは::GetMenuState()をフックします。
		//
		inline static struct
		{
			inline static UINT WINAPI hook(HMENU menu, UINT id, UINT flags)
			{
				MY_TRACE(_T("extoolbar::GetMenuState(0x%08X, 0x%08X, 0x%08X)\n"), menu, id, flags);

				UINT result = orig(menu, id, flags);
				if (result == -1) return 0;
				return result;
			}
			inline static decltype(&hook) orig = 0;
		} GetMenuState;

		//
		// extoolbar.auf用のフックを初期化します。
		//
		static BOOL init()
		{
			HMODULE extoolbar = ::GetModuleHandle(_T("extoolbar.auf"));
			MY_TRACE_HEX(extoolbar);
			if (!extoolbar) return FALSE;

			// extoolbar.auf内の::GetMenuState()をフックします。
			Tools::attach_import(GetMenuState, extoolbar, "GetMenuState");
			MY_TRACE_HEX(GetMenuState.orig);

			return TRUE;
		}
	};
}
