#pragma once
#include "Api/CreateWindow.h"
#include "Api/GetModuleHandle.h"
#include "Api/LoadIcon.h"
#include "Api/GetOpenFileName.h"
#include "Api/CallWindowProc.h"
#include "Api/Gdi.h"
#include "Api/Theme.h"

namespace fgo::dark::hook
{
	//
	// リターン先のアドレスを返します。
	//
	__declspec(naked) inline void getRetAddr()
	{
		__asm {
			MOV EAX,[ESP]
			RETN
		}
	}

	//
	// このクラスはAPIをフックします。
	//
	inline struct Api
	{
		//
		// APIフックの初期化処理です。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			HMODULE kernel32 = ::GetModuleHandle(_T("kernel32.dll"));
			MY_TRACE_HEX(kernel32);

			HMODULE user32 = ::GetModuleHandle(_T("user32.dll"));
			MY_TRACE_HEX(user32);

			HMODULE comdlg32 = ::GetModuleHandle(_T("comdlg32.dll"));
			MY_TRACE_HEX(comdlg32);

			Tools::attach(CreateWindowExA);
			Tools::attach(CreateWindowExW);
			Tools::attach(CreateDialogParamA);
			Tools::attach(CreateDialogParamW);
			Tools::attach(CreateDialogIndirectParamA);
			Tools::attach(CreateDialogIndirectParamW);
#if 0
			Tools::attach(DialogBoxParamA);
			Tools::attach(DialogBoxParamW);
			Tools::attach(DialogBoxIndirectParamA);
			Tools::attach(DialogBoxIndirectParamW);
#endif
//			Tools::attach(EnumProcessModules, ::GetProcAddress(kernel32, "K32EnumProcessModules"));
#if 1
			Tools::attach(GetOpenFileNameA, ::GetProcAddress(comdlg32, "GetOpenFileNameA"));
			Tools::attach(GetOpenFileNameW, ::GetProcAddress(comdlg32, "GetOpenFileNameW"));
			Tools::attach(GetSaveFileNameA, ::GetProcAddress(comdlg32, "GetSaveFileNameA"));
			Tools::attach(GetSaveFileNameW, ::GetProcAddress(comdlg32, "GetSaveFileNameW"));
#endif
//			Tools::attach(LoadIconA);
//			Tools::attach(LoadIconW);
			Tools::attach(LoadImageA);
//			Tools::attach(LoadImageW);
			Tools::attach(DrawIconEx);
			Tools::attach(LoadMenuA);

			// 2B = 7671339B - 76713370
			uintptr_t address1 = ::CallWindowProcW((WNDPROC)getRetAddr, 0, 0, 0, 0) - 0x2B;
			BYTE code[1] = {};
			::ReadProcessMemory(::GetCurrentProcess(), (LPCVOID)address1, code, sizeof(code), 0);
			if (code[0] == 0xCC) address1 += 0x01;
			Tools::attach(CallWindowProcInternal, address1);

			Tools::attach(FillRect);
//			Tools::attach(DrawFrame, ::GetProcAddress(user32, "DrawFrame"));
//			Tools::attach(DrawFrameControl);
//			Tools::attach(FrameRect);
			Tools::attach(DrawEdge);
//			Tools::attach(DrawFocusRect);
//			Tools::attach(DrawStateW);
			Tools::attach(ExtTextOutW);
//			Tools::attach(PatBlt);

//			Tools::attach(DrawThemeParentBackground);
			Tools::attach(DrawThemeBackground);
			Tools::attach(DrawThemeBackgroundEx);
			Tools::attach(DrawThemeText);
			Tools::attach(DrawThemeTextEx);
//			Tools::attach(DrawThemeIcon);
//			Tools::attach(DrawThemeEdge);
#if 0
			Tools::attach(OpenThemeData);
			Tools::attach(OpenThemeDataForDpi);
			Tools::attach(OpenThemeDataEx);
			Tools::attach(SetWindowTheme);
#endif
			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE(_T("APIフックに失敗しました\n"));

				return FALSE;
			}

			hive.orig.CallWindowProcInternal = CallWindowProcInternal.orig;
			hive.orig.FillRect = FillRect.orig;
			hive.orig.DrawFrame = DrawFrame.orig;
			hive.orig.DrawFrameControl = DrawFrameControl.orig;
			hive.orig.FrameRect = FrameRect.orig;
			hive.orig.DrawEdge = DrawEdge.orig;
			hive.orig.DrawFocusRect = DrawFocusRect.orig;
			hive.orig.DrawStateW = DrawStateW.orig;
			hive.orig.ExtTextOutW = ExtTextOutW.orig;
			hive.orig.PatBlt = PatBlt.orig;

			hive.orig.DrawThemeParentBackground = DrawThemeParentBackground.orig;
			hive.orig.DrawThemeBackground = DrawThemeBackground.orig;
			hive.orig.DrawThemeBackgroundEx = DrawThemeBackgroundEx.orig;
			hive.orig.DrawThemeText = DrawThemeText.orig;
			hive.orig.DrawThemeTextEx = DrawThemeTextEx.orig;
			hive.orig.DrawThemeIcon = DrawThemeIcon.orig;
			hive.orig.DrawThemeEdge = DrawThemeEdge.orig;

			return TRUE;
		}

		//
		// APIフックの後始末処理です。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} api;
}
