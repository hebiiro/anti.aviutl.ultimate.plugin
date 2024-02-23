#pragma once
#include "Hive.h"
#include "Skin.h"

namespace fgo::dark::hook
{
	//
	// このクラスは::LoadIconA()をフックします。
	//
	inline struct
	{
		inline static HICON WINAPI hook(HINSTANCE instance, LPCSTR iconName)
		{
			MY_TRACE_FUNC("0x%08X, %hs", instance, hive.safe_string(iconName));

			return orig(instance, iconName);
		}
		inline static decltype(&hook) orig = ::LoadIconA;
	} LoadIconA;

	//
	// このクラスは::LoadIconW()をフックします。
	//
	inline struct
	{
		inline static HICON WINAPI hook(HINSTANCE instance, LPCWSTR iconName)
		{
			MY_TRACE_FUNC("0x%08X, %ws", instance, hive.safe_string(iconName));

			return orig(instance, iconName);
		}
		inline static decltype(&hook) orig = ::LoadIconW;
	} LoadIconW;

	//
	// このクラスは::LoadImageA()をフックします。
	//
	inline struct
	{
		inline static HANDLE WINAPI hook(HINSTANCE instance, LPCSTR name, UINT type, int cx, int cy, UINT flags)
		{
			MY_TRACE_FUNC("0x%08X, %hs, 0x%08X, %d, %d, 0x%08X", instance, hive.safe_string(name), type, cx, cy, flags);

			if ((uintptr_t)name > 0x0000FFFF && !::IsBadReadPtr(name, 1))
			{
				if (instance == ::GetModuleHandle(0) && ::StrStrIA(name, "ICON_"))
				{
					MY_TRACE(_T("AviUtlのアイコンをアイコンマネージャに追加します %hs, 0x%08X\n"), name, flags);

					char name2[MAX_PATH] = {};
					::StringCchCopyA(name2, std::size(name2), "AVIUTL_");
					::StringCchCatA(name2, std::size(name2), name);

					HANDLE result = orig(hive.instance, name2, type, cx, cy, flags);
					skin::icon_manager.add((HICON)result, (_bstr_t)name2);
					return result;
				}
				else if (instance == ::GetModuleHandle(_T("exedit.auf")) && ::StrStrIA(name, "ICON_"))
				{
					MY_TRACE(_T("拡張編集のアイコンをアイコンマネージャに追加します %hs, 0x%08X\n"), name, flags);

					char name2[MAX_PATH] = {};
					::StringCchCopyA(name2, std::size(name2), "EXEDIT_");
					::StringCchCatA(name2, std::size(name2), name);

					HANDLE result = orig(hive.instance, name2, type, cx, cy, flags);
					skin::icon_manager.add((HICON)result, (_bstr_t)name2);
					return result;
				}
			}

			return orig(instance, name, type, cx, cy, flags);
		}
		inline static decltype(&hook) orig = ::LoadImageA;
	} LoadImageA;

	//
	// このクラスは::LoadImageW()をフックします。
	//
	inline struct
	{
		inline static HANDLE WINAPI hook(HINSTANCE instance, LPCWSTR name, UINT type, int cx, int cy, UINT flags)
		{
			MY_TRACE_FUNC("0x%08X, %ws, 0x%08X, %d, %d, 0x%08X", instance, hive.safe_string(name), type, cx, cy, flags);

			return orig(instance, name, type, cx, cy, flags);
		}
		inline static decltype(&hook) orig = ::LoadImageW;
	} LoadImageW;

	//
	// このクラスは::DrawIconEx()をフックします。
	//
	inline struct
	{
		inline static BOOL WINAPI hook(HDC dc, int x, int y, HICON icon, int w, int h, UINT step, HBRUSH brush, UINT flags)
		{
			MY_TRACE_FUNC("0x%08X, %d, %d, 0x%08X, %d, %d, %d, 0x%08X, 0x%08X", dc, x, y, icon, w, h, step, brush, flags);

			icon = skin::icon_manager.get(icon);

			return orig(dc, x, y, icon, w, h, step, brush, flags);
		}
		inline static decltype(&hook) orig = ::DrawIconEx;
	} DrawIconEx;

	//
	// このクラスは::LoadMenuA()をフックします。
	//
	inline struct
	{
		inline static HMENU WINAPI hook(HINSTANCE instance, LPCSTR menuName)
		{
			MY_TRACE_FUNC("0x%08X, %hs", instance, hive.safe_string(menuName));

			if (::GetModuleHandle(_T("exedit.auf")))
			{
				Tools::detach(LoadMenuA);

//				ExEdit::initExEdit();
			}

			return orig(instance, menuName);
		}
		inline static decltype(&hook) orig = ::LoadMenuA;
	} LoadMenuA;
}
