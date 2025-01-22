#pragma once

namespace apn
{
	inline void set_logger(LPCWSTR args)
	{
#if defined(_DEBUG) && 1
		if (my::contains(args, L"f1"))
		{
			// カスタムロガーを設定します。
			static struct Logger : my::Tracer::Logger {
				virtual void output(LPCTSTR raw, LPCTSTR text) override {
					// F1キーが押されているときだけログを出力します。
					if (::GetKeyState(VK_F1) < 0) ::OutputDebugString(text);
				}
			} logger;
			my::Tracer::logger = &logger;
		}
		else if (my::contains(args, L"shift"))
		{
			// カスタムロガーを設定します。
			static struct Logger : my::Tracer::Logger {
				virtual void output(LPCTSTR raw, LPCTSTR text) override {
					// Shiftキーが押されているときだけログを出力します。
					if (::GetKeyState(VK_SHIFT) < 0) ::OutputDebugString(text);
				}
			} logger;
			my::Tracer::logger = &logger;
		}
#endif
		if (!my::contains(args, L"debug")) my::reset(my::Tracer::logger);
	}
}
