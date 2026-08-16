#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはloudness.aufをフックします。
	//
	inline struct loudness_t : entry_t::node_t
	{
		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");

			auto loudness = ::GetModuleHandle(_T("loudness.auf"));
			MY_TRACE_HEX(loudness);
			if (!loudness) return TRUE;

			my::hook::attach_import(TextOutA, loudness, "TextOutA");

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

		//
		// このクラスはloudness.auf内の::TextOutA()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, LPCSTR text, int c)
			{
				// ダイアログのパレットを使用します。
				const auto& palette = paint::dialog_material.palette;

				auto part_id = WP_DIALOG;
				auto state_id = ::IsWindowEnabled(gdi::renderer_t::current_message_state.hwnd) ? ETS_NORMAL : ETS_DISABLED;
				auto str = my::ws(std::string(text, c));

				return paint::stylus.ext_text_out(dc, x, y, 0, nullptr, str.c_str(), (UINT)str.length(), nullptr, palette, part_id, state_id, FALSE);
//				return orig_proc(dc, x, y, text, c);
			}
			inline static decltype(&hook_proc) orig_proc = ::TextOutA;
		} TextOutA;
	} loudness;
}
