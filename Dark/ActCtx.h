#pragma once

namespace fgo::dark
{
	//
	// このクラスはアクティブ化コンテキストを管理します。
	//
	inline struct ActCtx
	{
		HANDLE hac = INVALID_HANDLE_VALUE;

		//
		// アクティブ化コンテキストを作成します。
		//
		BOOL init(HINSTANCE instance)
		{
			MY_TRACE_FUNC("0x%08X", instance);

			// マニフェストのファイル名を取得します。
			WCHAR fileName[MAX_PATH] = {};
			::GetModuleFileNameW(instance, fileName, std::size(fileName));
			::PathRemoveFileSpecW(fileName);
			::PathAppendW(fileName, L"cc6.manifest");
			MY_TRACE_WSTR(fileName);

			// アクティブ化コンテキストを作成します。
			ACTCTXW ac = { sizeof(ac) };
			ac.lpSource = fileName;
			hac = ::CreateActCtxW(&ac);

			return hac != INVALID_HANDLE_VALUE;
		}

		//
		// アクティブ化コンテキストを削除します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			if (hac != INVALID_HANDLE_VALUE)
				::ReleaseActCtx(hac), hac = INVALID_HANDLE_VALUE;

			return TRUE;
		}

		struct Activator
		{
			ULONG_PTR cookie = 0;

			//
			// アクティブ化コンテキストをアクティブにします。
			//
			Activator()
			{
//				MY_TRACE_FUNC("");

				::ActivateActCtx(actctx.hac, &cookie);
			}

			//
			// アクティブ化コンテキストを非アクティブにします。
			//
			~Activator()
			{
//				MY_TRACE_FUNC("");

				::DeactivateActCtx(0, cookie), cookie = 0;
			}
		};
	} actctx;
}
