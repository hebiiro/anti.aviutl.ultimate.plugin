#pragma once

namespace apn::dark
{
	//
	// このクラスはアクティブ化コンテキストを管理します。
	//
	inline struct act_ctx_t : entry_t::node_t
	{
        std::unique_ptr<std::remove_pointer<HANDLE>::type,
            decltype([](HANDLE handle){ ::ReleaseActCtx(handle); })> handle;

		virtual BOOL on_init() { return init(); }
		virtual BOOL on_exit() { return exit(); }

		//
		// アクティブ化コンテキストを作成します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// マニフェストのファイル名を取得します。
			auto file_name = magi.get_assets_file_name(hive.c_name + L"\\aviutl_dark.exe.manifest"s);
			MY_TRACE_STR(file_name);

			// アクティブ化コンテキストを作成します。
			ACTCTXW ac = { sizeof(ac) };
			ac.lpSource = file_name.c_str();
			handle.reset(::CreateActCtxW(&ac));

			return handle.get() != INVALID_HANDLE_VALUE;
		}

		//
		// アクティブ化コンテキストを削除します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			handle.reset();

			return TRUE;
		}

		struct activator_t
		{
			ULONG_PTR cookie = 0;

			//
			// アクティブ化コンテキストをアクティブにします。
			//
			activator_t()
			{
//				MY_TRACE_FUNC("");

				::ActivateActCtx(act_ctx.handle.get(), &cookie);
			}

			//
			// アクティブ化コンテキストを非アクティブにします。
			//
			~activator_t()
			{
//				MY_TRACE_FUNC("");

				::DeactivateActCtx(0, cookie), cookie = 0;
			}
		};
	} act_ctx;
}
