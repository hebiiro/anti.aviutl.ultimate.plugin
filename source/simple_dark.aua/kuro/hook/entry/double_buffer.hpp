#pragma once

namespace apn::dark::kuro::hook
{
	//
	// このクラスはダブルバッファ関係のフックを担当します。
	//
	inline struct double_buffer_t : common_entry_t
	{
		//
		// 先行して初期化処理を実行します。
		//
		virtual BOOL on_pre_init() override
		{
			MY_TRACE_FUNC("");

			// APIフックを開始します。
			{
				my::hook::detours detours;
				my::hook::attach(BeginPaint);
				my::hook::attach(EndPaint);
			}

			return TRUE;
		}

		//
		// 最後の後始末処理を実行します。
		//
		virtual BOOL on_post_exit() override
		{
			MY_TRACE_FUNC("");

			// APIフックを終了します。
			{
				my::hook::detours detours;
				my::hook::detach(BeginPaint);
				my::hook::detach(EndPaint);
			}

			return TRUE;
		}

		//
		// このクラスは::BeginPaint()をフックします。
		//
		struct {
			inline static HDC WINAPI hook_proc(HWND hwnd, LPPAINTSTRUCT ps)
			{
//				MY_TRACE_FUNC("{/hex}", hwnd);

				// デフォルト処理を実行してデフォルトのDCを取得します。
				auto dc = orig_proc(hwnd, ps);
//				MY_TRACE_HEX(dc);

				// バックバッファを作成します。
				auto back_buffer = dark::double_buffer.create_back_buffer(hwnd, dc);

				// バックバッファが作成された場合は
				if (back_buffer)
				{
					// バックバッファを返します。
					return back_buffer;
				}

				// デフォルトのDCを返します。
				return dc;
			}
			inline static decltype(&hook_proc) orig_proc = ::BeginPaint;
		} BeginPaint;

		//
		// このクラスは::EndPaint()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HWND hwnd, const PAINTSTRUCT* ps)
			{
//				MY_TRACE_FUNC("{/hex}", hwnd);

				// ダブルバッファが存在する場合はフロントバッファに転送します。
				dark::double_buffer.swap(hwnd);

				return orig_proc(hwnd, ps);
			}
			inline static decltype(&hook_proc) orig_proc = ::EndPaint;
		} EndPaint;
	} double_buffer;
}
