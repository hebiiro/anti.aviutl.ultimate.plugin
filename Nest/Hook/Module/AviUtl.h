#pragma once

namespace fgo::nest::hook::aviutl
{
	struct WindowPlacement
	{
		HWND hwnd;
		WINDOWPLACEMENT wp = { sizeof(wp) };

		WindowPlacement(HWND hwnd)
			: hwnd(hwnd)
		{
			// ウィンドウの位置を取得します。
			::GetWindowPlacement(hwnd, &wp);

			// ウィンドウを最大化します。
			::ShowWindow(hwnd, SW_MAXIMIZE);

			// ウィンドウを一番手前に持ってきます。
			::SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}

		~WindowPlacement()
		{
			// ウィンドウの位置を元に戻します。
			::SetWindowPlacement(hwnd, &wp);
		}
	};

	inline BOOL g_movieplaymain = FALSE;

	namespace PlayMain
	{
		inline uintptr_t orig = 0;

		inline __declspec(naked) UINT __fastcall doDefault(UINT u1, UINT u2, UINT u3, UINT u4, UINT u5, UINT u6)
		{
			// origは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

			__asm {
				push ebp
				mov ebp, esp
				sub esp, __LOCAL_SIZE
				mov u1, ecx
				mov u2, edx
			}

			__asm {
				// __fastcallとして引数を積み直します。
				MOV EAX, u6
				PUSH EAX
				MOV EAX, u5
				PUSH EAX
				MOV EAX, u4
				PUSH EAX
				MOV EAX, u3
				PUSH EAX
				MOV EDX, u2
				MOV ECX, u1
				CALL orig
				ADD ESP, 0x10 // orig()は__cdeclで終わります。
			}

			__asm {
				mov esp, ebp
				pop ebp
				retn 0x10 // __fastcallで終わります。
			}
		}

		inline UINT __fastcall doHook(UINT u1, UINT u2, UINT u3, UINT u4, UINT u5, UINT u6)
		{
			MY_TRACE(_T("AviUtl::PlayMain::doHook()\n"));

			// フラグが立っていない場合はデフォルト処理のみ行います。
			if (!hive.showPlayer)
				return doDefault(u1, u2, u3, u4, u5, u6);

			// AviUtlウィンドウまたは再生ウィンドウを取得します。
			HWND hwnd = ::GetParent(hive.aviutlWindow);

			if (!g_movieplaymain)
			{
				auto shuttle = shuttleManager.get(L"再生ウィンドウ");
				if (shuttle) hwnd = ::GetParent(*shuttle);
			}

			// ウィンドウを最大化します。その後、スコープから抜けたときに元に戻します。
			WindowPlacement wp(hwnd);

			// デフォルト処理を行います。
			return doDefault(u1, u2, u3, u4, u5, u6);
		}

		inline __declspec(naked) UINT __fastcall hook(UINT u1, UINT u2, UINT u3, UINT u4, UINT u5, UINT u6)
		{
			// この関数は__fastcallで始まり、__cdeclで終わります。

			__asm {
				push ebp
				mov ebp, esp
				sub esp, __LOCAL_SIZE
				mov u1, ecx
				mov u2, edx
			}

			__asm {
				// 引数を積み直します。
				MOV EAX, u6
				PUSH EAX
				MOV EAX, u5
				PUSH EAX
				MOV EAX, u4
				PUSH EAX
				MOV EAX, u3
				PUSH EAX
				MOV EDX, u2
				MOV ECX, u1
				CALL doHook
			}

			__asm {
				mov esp, ebp
				pop ebp
				ret // __cdeclで終わります。
			}
		}

		inline LONG init(uintptr_t aviutl)
		{
			orig = aviutl + 0x00053320;
			return DetourAttach((PVOID*)&orig, hook);
		}
	}

	namespace PlaySub
	{
		inline uintptr_t orig = 0;

		inline __declspec(naked) UINT __fastcall doDefault(UINT u1, UINT u2, UINT u3, UINT u4, UINT u5)
		{
			// origは__fastcallで始まり、__cdeclで終わるので、それに合わせてアセンブラで調整します。

			__asm {
				push ebp
				mov ebp, esp
				sub esp, __LOCAL_SIZE
				mov u1, ecx
				mov u2, edx
			}

			__asm {
				// __fastcallとして引数を積み直します。
				MOV EAX, u5
				PUSH EAX
				MOV EAX, u4
				PUSH EAX
				MOV EAX, u3
				PUSH EAX
				MOV EDX, u2
				MOV ECX, u1
				CALL orig
				ADD ESP, 0x0C // orig()は__cdeclで終わります。
			}

			__asm {
				mov esp, ebp
				pop ebp
				retn 0x0C // __fastcallで終わります。
			}
		}

		inline UINT __fastcall doHook(UINT u1, UINT u2, UINT u3, UINT u4, UINT u5)
		{
			MY_TRACE(_T("AviUtl::PlayMain::doHook()\n"));

			// フラグが立っていない場合はデフォルト処理のみ行います。
			if (!hive.showPlayer)
				return doDefault(u1, u2, u3, u4, u5);

			// AviUtlウィンドウまたは再生ウィンドウを取得します。
			HWND hwnd = ::GetParent(hive.aviutlWindow);

			if (!g_movieplaymain)
			{
				auto shuttle = shuttleManager.get(L"再生ウィンドウ");
				if (shuttle) hwnd = ::GetParent(*shuttle);
			}

			// ウィンドウを最大化します。その後、スコープから抜けたときに元に戻します。
			WindowPlacement wp(hwnd);

			// デフォルト処理を行います。
			return doDefault(u1, u2, u3, u4, u5);
		}

		inline __declspec(naked) UINT __fastcall hook(UINT u1, UINT u2, UINT u3, UINT u4, UINT u5)
		{
			// この関数は__fastcallで始まり、__cdeclで終わります。

			__asm {
				push ebp
				mov ebp, esp
				sub esp, __LOCAL_SIZE
				mov u1, ecx
				mov u2, edx
			}

			__asm {
				// 引数を積み直します。
				MOV EAX, u5
				PUSH EAX
				MOV EAX, u4
				PUSH EAX
				MOV EAX, u3
				PUSH EAX
				MOV EDX, u2
				MOV ECX, u1
				CALL doHook
			}

			__asm {
				mov esp, ebp
				pop ebp
				ret // __cdeclで終わります。
			}
		}

		inline LONG init(uintptr_t aviutl)
		{
			orig = aviutl + 0x00051150;
			return DetourAttach((PVOID*)&orig, hook);
		}
	}

	//
	// AviUtl用フックの初期化処理です。
	//
	inline BOOL init()
	{
		MY_TRACE(_T("AviUtl::init()\n"));

		TCHAR fileName[MAX_PATH] = {};
		::GetModuleFileName(0, fileName, std::size(fileName));
		::PathRemoveFileSpec(fileName);
		::PathAppend(fileName, _T("aviutl.ini"));
		MY_TRACE_TSTR(fileName);

		g_movieplaymain = ::GetPrivateProfileInt(_T("system"), _T("movieplaymain"), 0, fileName);

		uintptr_t aviutl = (uintptr_t)::GetModuleHandle(0);
		MY_TRACE_HEX(aviutl);

		DetourTransactionBegin();
		DetourUpdateThread(::GetCurrentThread());

		PlayMain::init(aviutl);
		PlaySub::init(aviutl);

		return DetourTransactionCommit() == NO_ERROR;
	}

	//
	// AviUtl用フックの後始末処理です。
	//
	inline BOOL exit()
	{
		MY_TRACE(_T("AviUtl::exit()\n"));

		return TRUE;
	}
}
