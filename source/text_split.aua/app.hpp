#pragma once

namespace apn::text_split
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		virtual BOOL split_text() override
		{
			return Executor().split_text();
		}
	} app_impl;
}
