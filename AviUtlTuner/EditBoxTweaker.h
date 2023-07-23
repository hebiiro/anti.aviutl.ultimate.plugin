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

		BOOL unicodeInput = TRUE;
		BOOL ctrlA = TRUE;

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

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		// 特に何もしません。
		//
		BOOL exit()
		{
			return TRUE;
		}
	} editbox_tweaker;
}
