#pragma once

namespace apn
{
	//
	// このクラスは指定されたキーが押されているときだけログを出力するようにします。
	//
	template <int vk>
	struct logger_width_key_t : my::Tracer::Logger
	{
		//
		// オリジナルのロガーです。
		//
		my::Tracer::Logger* orig = {};

		//
		// コンストラクタです。
		//
		logger_width_key_t()
		{
			// オリジナルロガーを取得しておきます。
			orig = my::Tracer::logger;

			// このロガーをセットします。
			my::Tracer::logger = this;
		}

		//
		// この仮想関数をはログを出力するときに呼び出されます。
		//
		virtual void output(LPCTSTR raw, LPCTSTR text) override
		{
			// 指定されたキーが押されているときだけログを出力します。
			if (::GetKeyState(vk) < 0) orig->output(raw, text);
		}
	};

	inline void set_logger(LPCWSTR args)
	{
#if defined(_DEBUG) && 1
		if (my::contains(args, L"f1"))
		{
			// カスタムロガーをセットします。
			static logger_width_key_t<VK_F1> f1_logger;
		}
		else if (my::contains(args, L"shift"))
		{
			// カスタムロガーをセットします。
			static logger_width_key_t<VK_SHIFT> shift_logger;
		}
#endif
		// デバッグが設定されていない場合はロガーをリセットします。
		if (!my::contains(args, L"debug")) my::reset(my::Tracer::logger);
	}
}
