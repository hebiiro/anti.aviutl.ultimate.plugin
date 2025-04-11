#pragma once

namespace apn::local_web_app
{
	//
	// このクラスはhtmlを表示するブラウザウィンドウです。
	//
	inline struct Browser : apn::Browser
	{
		//
		// 指定されたhtmlファイルをブラウズします。
		//
		BOOL navigate(const std::wstring& file_name)
		{
			MY_TRACE_FUNC("{/}", file_name);

			contents_file_name = file_name;

			return __super::navigate();
		}

		//
		// この仮想関数は、エラーが発生したときに呼ばれます。
		//
		virtual BOOL on_error(const std::wstring& what) override
		{
			return hive.message_box(what, *this);
		}
	} browser;
}
