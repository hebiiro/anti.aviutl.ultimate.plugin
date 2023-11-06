#pragma once

namespace fgo::dark::hook
{
	//
	// このクラスはloudness.aufをフックします。
	//
	inline struct loudness
	{
		//
		// loudness.aufのインポート関数をフックします。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			HMODULE loudness = ::GetModuleHandle(_T("loudness.auf"));
			MY_TRACE_HEX(loudness);

			if (loudness)
			{
				Tools::attach_import(TextOutA, loudness, "TextOutA");
			}

			return TRUE;
		}

		//
		// このクラスはloudness.auf内の::TextOutA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook(HDC dc, int x, int y, LPCSTR text, int c)
			{
				HTHEME theme = skin::theme::manager.getTheme(skin::theme::THEME_STATIC);
				return skin::theme::manager.onExtTextOut(theme, dc, STAT_TEXT, 0, x, y, 0, 0, (_bstr_t)text, c, 0);
			}
			inline static decltype(&hook) orig = ::TextOutA;
		} TextOutA;
	} loudness;
}
