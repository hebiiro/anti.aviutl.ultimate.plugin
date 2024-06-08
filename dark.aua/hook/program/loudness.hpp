#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはloudness.aufをフックします。
	//
	inline struct loudness
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			auto loudness = ::GetModuleHandle(_T("loudness.auf"));
			MY_TRACE_HEX(loudness);

			if (loudness)
			{
				my::hook::attach_import(TextOutA, loudness, "TextOutA");
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
		// このクラスはloudness.auf内の::TextOutA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, LPCSTR text, int c)
			{
				if (auto theme = skin::theme::manager.get_theme(VSCLASS_STATIC))
				{
					auto str = my::ws(std::string(text, c));
					return python.call_text_out(gdi::manager.current_state.hwnd, theme, dc, STAT_TEXT, 0, x, y, 0, 0, str.c_str(), str.length(), 0);
				}

				return orig_proc(dc, x, y, text, c);
			}
			inline static decltype(&hook_proc) orig_proc = ::TextOutA;
		} TextOutA;
	} loudness;
}
