#pragma once
#include "Tools/edit_predicates.h"
#include "Tools/Hook.h"
#include "Hive.h"
#include "Servant.h"

namespace aut
{
	//
	// このクラスはエディットボックスを微調整するサーヴァントです。
	//
	inline struct EditBoxTweaker : Servant
	{
		//
		// このサーヴァントを識別するための名前を返します。
		//
		inline static LPCWSTR getServantNameStatic() { return L"EditBoxTweaker"; }

		BOOL unicodeInput;
		BOOL ctrlA;
		int delta;
		struct Font {
			_bstr_t name;
			int height;
			int pitch;
			HFONT handle;
		} font;

		//
		// この構造体は ::GetMesageA() をフック関数に置き換えるために使用されます。
		//
		struct GetMessageA {
			static BOOL WINAPI hook(LPMSG msg, HWND hwnd, UINT msgFilterMin, UINT msgFilterMax)
			{
				BOOL result = FALSE;
				if (editbox_tweaker.unicodeInput) {
					// ::GetMessageA() の代わりに ::GetMessageW() を呼び出す。
					result = ::GetMessageW(msg, hwnd, msgFilterMin, msgFilterMax);
				} else {
					result = editbox_tweaker.getMessageA.orig(msg, hwnd, msgFilterMin, msgFilterMax);
				}

				// Ctrl + A.
				if (editbox_tweaker.ctrlA &&
					msg->message == WM_KEYDOWN &&
					msg->wParam == 'A' &&
					::GetKeyState(VK_CONTROL) < 0 &&
					check_window_classname(msg->hwnd, WC_EDITW)) {

					// テキスト全選択してこのメッセージはディスパッチしない．次のメッセージまで待機．
					::SendMessageW(msg->hwnd, EM_SETSEL, 0, -1);
					return hook(msg, hwnd, msgFilterMin, msgFilterMax);
				}

				return result;
			}
		}; Tools::Hook<GetMessageA> getMessageA;

		struct EditBox {
			static LRESULT CALLBACK subclassproc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam, UINT_PTR id, DWORD_PTR)
			{
				switch (msg) {
				case WM_SETFONT:
					MY_TRACE(_T("WM_SETFONT, 0x%08X, 0x%08X\n"), wparam, lparam);
					wparam = reinterpret_cast<WPARAM>(editbox_tweaker.font.handle); // handle is not null here.
				//[[fallthrough]];
				case WM_DESTROY:
					::RemoveWindowSubclass(hwnd, subclassproc, id);
					break;
				}

				return ::DefSubclassProc(hwnd, msg, wparam, lparam);
			}

			//
			// 拡張編集内の複数行エディットボックスを作成する関数(::CreateWindowExW())と置き換えられます。
			//
			static HWND WINAPI hook(DWORD exStyle, LPCWSTR className, LPCWSTR windowName, DWORD style,
				int x, int y, int w, int h, HWND parent, HMENU menu, HINSTANCE instance, LPVOID param)
			{
				MY_TRACE(_T("CreateWindowExW(%ws, %d, %d)\n"), className, w, h);

				HWND hwnd = editbox_tweaker.editbox.orig(exStyle, className, windowName,
					style, x, y, w, h + editbox_tweaker.delta, parent, menu, instance, param);

				if (editbox_tweaker.font.handle != nullptr)
					::SetWindowSubclass(hwnd, subclassproc, 0, 0);

				return hwnd;
			}
		}; Tools::Hook<EditBox> editbox;

		//
		// この構造体は ::DispatchMesageA() を ::DispatchMesageW() に置き換えるために使用されます。
		//
		struct DispatchMessageA {
			decltype(&::DispatchMessageA) hook = ::DispatchMessageW;
		}; Tools::Hook<DispatchMessageA> dispatchMessageA;

		//
		// この構造体は ::PeekMesageA() を ::PeekMesageW() に置き換えるために使用されます。
		//
		struct PeekMessageA {
			decltype(&::PeekMessageA) hook = ::PeekMessageW;
		}; Tools::Hook<PeekMessageA> peekMessageA;

		//
		// コンストラクタです。
		// コンフィグの初期値を設定します。
		//
		EditBoxTweaker()
		{
			unicodeInput = TRUE;
			ctrlA = TRUE;
			delta = 0;
			font.name = L"";
			font.height = 0;
			font.pitch = 0;
		}

		//
		// マスターから呼ばれます。
		//
		BOOL on_init() override
		{
			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// マスターから呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// 設定をコンフィグファイルから読み込みます。
		//
		BOOL load()
		{
			std::wstring path = hive.getConfigFileName(L"EditBoxTweaker.ini");

			getPrivateProfileInt(path.c_str(), L"Config", L"unicodeInput", unicodeInput);
			getPrivateProfileInt(path.c_str(), L"Config", L"ctrlA", ctrlA);
			getPrivateProfileInt(path.c_str(), L"Config", L"delta", delta);
			getPrivateProfileBSTR(path.c_str(), L"Config", L"font.name", font.name);
			getPrivateProfileInt(path.c_str(), L"Config", L"font.height", font.height);
			getPrivateProfileInt(path.c_str(), L"Config", L"font.pitch", font.pitch);

			if (wcslen(font.name) != 0)
			{
				// DPI に合わせてフォントのサイズを調整します。
				int dpi = ::GetSystemDpiForProcess(::GetCurrentProcess());
				int height = ::MulDiv(font.height, dpi, 96);

				// HFONT を作成します。
				// 複数行エディットボックスの WM_SETFONT でこのハンドルが渡されます。
				font.handle = ::CreateFontW(height, 0,
					0, 0, 0, 0, 0, 0, DEFAULT_CHARSET,
					0, 0, 0, font.pitch, font.name);
			}

			return TRUE;
		}

		//
		// 設定をコンフィグファイルに保存します。
		//
		BOOL save()
		{
			std::wstring path = hive.getConfigFileName(L"EditBoxTweaker.ini");

			setPrivateProfileInt(path.c_str(), L"Config", L"unicodeInput", unicodeInput);
			setPrivateProfileInt(path.c_str(), L"Config", L"ctrlA", ctrlA);
			setPrivateProfileInt(path.c_str(), L"Config", L"delta", delta);
			setPrivateProfileBSTR(path.c_str(), L"Config", L"font.name", font.name);
			setPrivateProfileInt(path.c_str(), L"Config", L"font.height", font.height);
			setPrivateProfileInt(path.c_str(), L"Config", L"font.pitch", font.pitch);

			return TRUE;
		}

		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		//
		BOOL init()
		{
			using namespace Tools;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			// 拡張編集のモジュールハンドルを取得する。
			addr_t exedit_auf = (addr_t)::GetModuleHandle(_T("exedit.auf"));

			getMessageA.attach(::GetMessageA);

			if (unicodeInput) {
				dispatchMessageA.attach(::DispatchMessageA);
				peekMessageA.attach(::PeekMessageA);
			}

			// 拡張編集内の ::CreateWindowExW() の呼び出しをフックします。
			// コンスト値も書き換えます。
			if (delta != 0 || font.handle != nullptr) {
				editbox.attach_to_abs_call(exedit_auf + 0x0008C46E);
				editbox.attach_to_abs_call(exedit_auf + 0x00087658);
				add_int32(exedit_auf + 0x0008CC56 + 1, delta);
				add_int32(exedit_auf + 0x000876DE + 1, delta);
			}

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			if (font.handle)
				::DeleteObject(font.handle), font.handle = 0;

			return TRUE;
		}
	} editbox_tweaker;
}
