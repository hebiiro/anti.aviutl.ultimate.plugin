#pragma once

namespace apn::text_split
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// テキスト分解を実行します。
		//
		virtual BOOL split_text() override
		{
			try
			{
				executor_t()();

				return TRUE;
			}
			catch (LPCWSTR error)
			{
				hive.message_box(error);
			}
			catch (...)
			{
				hive.message_box(L"未知の例外が発生しました");
			}

			return FALSE;
		}
	} app_impl;
}
