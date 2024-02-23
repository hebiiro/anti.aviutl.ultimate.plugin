#pragma once
#include "Hive.h"
#include "DoubleBuffer.h"

namespace fgo::dark::hook
{
	//
	// このクラスは::BeginPaint()をフックします。
	//
	inline struct
	{
		inline static HDC WINAPI hook(HWND hwnd, LPPAINTSTRUCT ps)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			// デフォルト処理を実行してデフォルトのDCを取得します。
			HDC dc = orig(hwnd, ps);
			MY_TRACE_HEX(dc);

			// バックバッファを作成します。
			auto back_buffer = double_buffer.createBackBuffer(hwnd, dc);

			// バックバッファが作成された場合は
			if (back_buffer)
			{
				// バックバッファを返します。
				return back_buffer;
			}

			// デフォルトのDCを返します。
			return dc;
		}
		inline static decltype(&hook) orig = ::BeginPaint;
	} BeginPaint;

	//
	// このクラスは::EndPaint()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HWND hwnd, const PAINTSTRUCT* ps)
		{
			MY_TRACE_FUNC("0x%08X", hwnd);

			// ダブルバッファが存在する場合はフロントバッファに転送します。
			double_buffer.swap(hwnd);

			return orig(hwnd, ps);
		}
		inline static decltype(&hook) orig = ::EndPaint;
	} EndPaint;
}
