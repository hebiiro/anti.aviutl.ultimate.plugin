#pragma once

namespace fgo::dark::hook
{
	//
	// このクラスはレンダラを一時的にロックします。
	//
	struct RendererLocker {
		RendererLocker() {
			if (skin::config.getOmitFileDialog() == skin::Config::OmitFileDialog::On) {
				hive.lockRenderer = TRUE;
//				skin::manager.redraw();
			}
		}
		~RendererLocker() {
			if (skin::config.getOmitFileDialog() == skin::Config::OmitFileDialog::On) {
				hive.lockRenderer = FALSE;
				skin::manager.redraw();
			}
		}
	};

	//
	// このクラスは::GetOpenFileNameA()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(LPOPENFILENAMEA ofn)
		{
			MY_TRACE_FUNC("0x%08X", ofn);

			RendererLocker locker;

			return orig(ofn);
		}
		inline static decltype(&hook) orig = ::GetOpenFileNameA;
	} GetOpenFileNameA;

	//
	// このクラスは::GetOpenFileNameW()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(LPOPENFILENAMEW ofn)
		{
			MY_TRACE_FUNC("0x%08X", ofn);

			RendererLocker locker;

			return orig(ofn);
		}
		inline static decltype(&hook) orig = ::GetOpenFileNameW;
	} GetOpenFileNameW;

	//
	// このクラスは::GetSaveFileNameA()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(LPOPENFILENAMEA ofn)
		{
			MY_TRACE_FUNC("0x%08X", ofn);

			RendererLocker locker;

			return orig(ofn);
		}
		inline static decltype(&hook) orig = ::GetSaveFileNameA;
	} GetSaveFileNameA;

	//
	// このクラスは::GetSaveFileNameW()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(LPOPENFILENAMEW ofn)
		{
			MY_TRACE_FUNC("0x%08X", ofn);

			RendererLocker locker;

			return orig(ofn);
		}
		inline static decltype(&hook) orig = ::GetSaveFileNameW;
	} GetSaveFileNameW;
}
