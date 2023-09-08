#pragma once

namespace fgo
{
	struct MysticCode
	{
		//
		// このクラスは::LoadLibraryA()をフックします。
		//
		inline static struct
		{
			inline static HMODULE WINAPI hook(LPCSTR fileName)
			{
				auto result = orig(fileName);

				MY_TRACE(_T("::LoadLibraryA(%hs) => 0x%08X\n"), fileName, result);

				if (::StrStrIA(fileName, "exedit.auf"))
				{
					MY_TRACE(_T("拡張編集が読み込まれたのでアドレスを取得します\n"));

					// 拡張編集関連のアドレス情報を初期化します。
					magi.auin.initExEditAddress();

					// これ以上フックする必要はないのでフックを解除します。
					mystic_code.exit();
				}

				return result;
			}
			inline static decltype(&hook) orig = ::LoadLibraryA;
		} LoadLibraryA;

		BOOL init()
		{
			MY_TRACE(_T("MysticCode::init()\n"));

			// AviUtl関連のアドレス情報を初期化します。
			magi.auin.initAviUtlAddress();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(LoadLibraryA);

			return DetourTransactionCommit() == NO_ERROR;
		}

		BOOL exit()
		{
			MY_TRACE(_T("MysticCode::exit()\n"));

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::detach(LoadLibraryA);

			return DetourTransactionCommit() == NO_ERROR;
		}
	} mystic_code;
}
