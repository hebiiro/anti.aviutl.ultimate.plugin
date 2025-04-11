#pragma once

namespace apn::dark::hook
{
	//
	// このクラスはアイコン関係のフックを担当します。
	//
	inline struct Icon
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

//			my::hook::attach(LoadIconA);
//			my::hook::attach(LoadIconW);
			my::hook::attach(LoadImageA);
//			my::hook::attach(LoadImageW);
			my::hook::attach(DrawIconEx);
//			my::hook::attach(LoadMenuA);

			if (DetourTransactionCommit() != NO_ERROR)
			{
				MY_TRACE("APIフックに失敗しました\n");

				return FALSE;
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// このクラスは::LoadIconA()をフックします。
		//
		struct {
			inline static HICON WINAPI hook_proc(HINSTANCE instance, LPCSTR icon_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", instance, safe_string(icon_name));

				return orig_proc(instance, icon_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadIconA;
		} LoadIconA;

		//
		// このクラスは::LoadIconW()をフックします。
		//
		struct {
			inline static HICON WINAPI hook_proc(HINSTANCE instance, LPCWSTR icon_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", instance, safe_string(icon_name));

				return orig_proc(instance, icon_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadIconW;
		} LoadIconW;

		//
		// このクラスは::LoadImageA()をフックします。
		//
		struct {
			inline static HANDLE WINAPI hook_proc(HINSTANCE instance, LPCSTR name, UINT type, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/}, {/}, {/hex}", instance, safe_string(name), type, cx, cy, flags);

				if ((my::addr_t)name > 0x0000FFFF && !::IsBadReadPtr(name, 1))
				{
					if (instance == ::GetModuleHandle(nullptr) && ::StrStrIA(name, "ICON_"))
					{
						MY_TRACE("AviUtlのアイコンをアイコンマネージャに追加します {/}, {/hex}\n", name, flags);

						auto str = "AVIUTL_"s + name;

						auto result = orig_proc(hive.instance, str.c_str(), type, cx, cy, flags);
						skin::icon_manager.add((HICON)result, my::ws(str).c_str());
						return result;
					}
					else if (instance == ::GetModuleHandle(_T("exedit.auf")) && ::StrStrIA(name, "ICON_"))
					{
						MY_TRACE("拡張編集のアイコンをアイコンマネージャに追加します {/}, {/hex}\n", name, flags);

						auto str = "EXEDIT_"s + name;

						auto result = orig_proc(hive.instance, str.c_str(), type, cx, cy, flags);
						skin::icon_manager.add((HICON)result, my::ws(str).c_str());
						return result;
					}
				}

				return orig_proc(instance, name, type, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadImageA;
		} LoadImageA;

		//
		// このクラスは::LoadImageW()をフックします。
		//
		struct {
			inline static HANDLE WINAPI hook_proc(HINSTANCE instance, LPCWSTR name, UINT type, int cx, int cy, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/hex}, {/}, {/}, {/hex}", instance, safe_string(name), type, cx, cy, flags);

				return orig_proc(instance, name, type, cx, cy, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadImageW;
		} LoadImageW;

		//
		// このクラスは::DrawIconEx()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(HDC dc, int x, int y, HICON icon, int w, int h, UINT step, HBRUSH brush, UINT flags)
			{
				MY_TRACE_FUNC("{/hex}, {/}, {/}, {/hex}, {/}, {/}, {/}, {/hex}, {/hex}", dc, x, y, icon, w, h, step, brush, flags);

				icon = skin::icon_manager.get(icon);

				return orig_proc(dc, x, y, icon, w, h, step, brush, flags);
			}
			inline static decltype(&hook_proc) orig_proc = ::DrawIconEx;
		} DrawIconEx;
#if 0
		//
		// このクラスは::LoadMenuA()をフックします。
		//
		struct {
			inline static HMENU WINAPI hook_proc(HINSTANCE instance, LPCSTR menu_name)
			{
				MY_TRACE_FUNC("{/hex}, {/}", instance, safe_string(menu_name));

				if (::GetModuleHandle(_T("exedit.auf")))
				{
					my::hook::detach(LoadMenuA);

					ExEdit::initExEdit();
				}

				return orig_proc(instance, menu_name);
			}
			inline static decltype(&hook_proc) orig_proc = ::LoadMenuA;
		} LoadMenuA;
#endif
	} icon;
}
