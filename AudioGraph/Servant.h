#pragma once
#include "Hive.h"
#include "Gui.h"
#include "Presenter.h"
#include "View.h"

namespace fgo::audio_graph
{
	//
	// このクラスは音声をグラフにして表示します。
	//
	inline struct AudioGraph : Servant, Magi::CommandID::Plugin
	{
		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return Hive::Name;
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return Hive::DisplayName;
		}

		//
		// この仮想関数は、DLLの初期化のタイミングで呼ばれます。
		//
		BOOL on_dll_init(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			hive.instance = instance;

			return TRUE;
		}

		//
		// この仮想関数は、DLLの後始末のタイミングで呼ばれます。
		//
		BOOL on_dll_exit(HINSTANCE instance) override
		{
			MY_TRACE_FUNC("0x%08X", instance);

			return TRUE;
		}

		//
		// AviUtlにメニューアイテムを追加します。
		//
		BOOL addMenuItem(AviUtl::FilterPlugin* fp, LPCWSTR text, UINT id)
		{
			char itemText[MAX_PATH] = {};
			::StringCchPrintfA(itemText, std::size(itemText), "[%ws]%ws", Hive::DisplayName, text);

			return fp->exfunc->add_menu_item(fp, itemText, hive.mainWindow, id, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			if (!presenter.init()) return FALSE;
			if (!view.init()) return FALSE;
			if (!gui.init()) return FALSE;
			if (!view.notify(gui.pi.dwThreadId)) return FALSE;
			if (!load()) return FALSE;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			MY_TRACE_FUNC("");

			save();
			view.exit();
			gui.exit();
			presenter.exit();

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウのコマンドのタイミングで呼ばれます。
		//
		BOOL on_window_command(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			switch (wParam)
			{
			case ID_ADDIN:
				{
					if (::IsWindow(hive.mainWindow))
						::ShowWindow(hive.mainWindow, SW_SHOW);

					break;
				}
			}

			return FALSE;
		}

		BOOL on_audio_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			MY_TRACE(_T("frame = %d, audio_n = %d\n"), fpip->frame, fpip->audio_n);

			switch (hive.state)
			{
			case Hive::State::Off:
				{
					return FALSE; // 無効化されているときは何もしません。
				}
			case Hive::State::OnWithoutPlaying:
				{
					if (proc_state.is_playing)
						return FALSE; // 再生中のときは何もしません。

					break;
				}
			}

			if (proc_state.is_saving)
				return FALSE; // 保存中のときは何もしません。

			if (!view.isVisible())
				return FALSE; // グラフコンテナが表示されていないときは何もしません。

			return presenter.send(fp, fpip, proc_state);
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"AudioGraph.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			getPrivateProfileBSTR(path, L"Config", L"window.left.name", hive.window.left.name);
			getPrivateProfileBSTR(path, L"Config", L"window.right.name", hive.window.right.name);
			getPrivateProfileLabel(path, L"Config", L"state", hive.state, Hive::State::label);

			view.load(path);

			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			setPrivateProfileBSTR(path, L"Config", L"window.left.name", hive.window.left.name);
			setPrivateProfileBSTR(path, L"Config", L"window.right.name", hive.window.right.name);
			setPrivateProfileLabel(path, L"Config", L"state", hive.state, Hive::State::label);

			view.save(path);

			return TRUE;
		}
	} servant;
}
