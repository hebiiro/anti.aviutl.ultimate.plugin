#pragma once

namespace apn
{
	inline struct Initializer
	{
		BOOL exited = FALSE;

		//
		// このクラスは::LoadLibraryA()をフックします。
		//
		inline static struct
		{
			inline static HMODULE WINAPI hook_proc(LPCSTR file_name)
			{
				auto result = orig_proc(file_name);

				if (initializer.exited) return result;

				MY_TRACE("::LoadLibraryA({}) => {:#010x}\n", file_name, result);

				if (::StrStrIA(file_name, "exedit.auf"))
				{
					MY_TRACE("拡張編集が読み込まれたのでアドレスを取得します\n");

					// 拡張編集関連のアドレス情報を初期化します。
					magi.exin.init();

					// これ以上フックする必要はないのでフックを解除します。
					initializer.exit();
				}

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadLibraryA;
		} LoadLibraryA;

		BOOL init()
		{
			MY_TRACE_FUNC("");

			// AviUtl関連のアドレス情報を初期化します。
			magi.auin.init();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::attach(LoadLibraryA);

			return DetourTransactionCommit() == NO_ERROR;
		}

		BOOL exit()
		{
			MY_TRACE_FUNC("");
#if 1
			// フックをデタッチすると他のプログラムのフックもデタッチされてしまいます。
			// そのため、フックはデタッチせずにフラグを立てるだけにします。
			exited = TRUE;
			return TRUE;
#else
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			my::hook::detach(LoadLibraryA);

			return DetourTransactionCommit() == NO_ERROR;
#endif
		}
	} initializer;
}
