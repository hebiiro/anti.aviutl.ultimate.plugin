#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはダブルバッファ関係のフックを担当します。
	//
	inline struct DoubleBuffer
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(BeginPaint);
			my::hook::attach(EndPaint);

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
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
