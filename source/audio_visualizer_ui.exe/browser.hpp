#pragma once

namespace apn::audio_visualizer::ui
{
	//
	// このクラスはブラウザです。
	//
	struct Browser : apn::Browser
	{
		//
		// この仮想関数は、エラーが発生したときに呼ばれます。
		//
		virtual BOOL on_error(const std::wstring& what) override
		{
			return hive.message_box(what, *this);
		}
	};
}
