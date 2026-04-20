#pragma once

namespace apn::item_affine
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_interface_t
	{
		//
		// 選択アイテムの位置を変更します。
		//
		virtual BOOL pos_execute() override
		{
			return execute([](){ executor_t().pos_execute(); });
		}

		//
		// 選択アイテムの長さを変更します。
		//
		virtual BOOL len_execute() override
		{
			return execute([](){ executor_t().len_execute(); });
		}

		//
		// 選択アイテムの位置または長さを変更します。
		//
		BOOL execute(auto func)
		{
			try
			{
				return func(), TRUE;
			}
			catch (const std::wstring& error)
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
