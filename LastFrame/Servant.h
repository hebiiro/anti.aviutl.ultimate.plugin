#pragma once
#include "MainWindow.h"

namespace fgo::last_frame
{
	//
	// このクラスは最終フレームを自動的に調整します。
	//
	inline struct LastFrame : Servant, Magi::CommandID::Plugin
	{
		MainWindow mainWindow;

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"LastFrame";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"最終フレーム自動調整";
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
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (::GetModuleHandleW(L"AdjustLastFrame.auf"))
			{
				::MessageBoxW(hwnd,
					L"LastFrameとAdjustLastFrame.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!mainWindow.create()) return FALSE;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			save();
			mainWindow.destroy();

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
					if (mainWindow)
						Tools::AviUtl::PluginWindowExtension::show(mainWindow);

					break;
				}
			}

			return FALSE;
		}

		//
		// 映像フィルタのタイミングで呼び出されます。
		//
		BOOL on_video_proc(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip, const ProcState& proc_state) override
		{
			// 現在の呼び出し状態をチェックします。
			if (proc_state.is_saving || proc_state.is_playing)
				return FALSE; // 保存や再生中のときは何もしません。

			// 最終フレームを調整します。
			adjustLastFrame(fp, fpip);

			return FALSE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"LastFrame.ini");
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
			getPrivateProfileInt(path, L"Config", L"enable", hive.enable);
			getPrivateProfileWindow(path, L"MainWindow", mainWindow);

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
			setPrivateProfileInt(path, L"Config", L"enable", hive.enable);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}

		//
		// 最終フレームを調整します。
		//
		BOOL adjustLastFrame(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			if (!hive.enable)
				return FALSE; // "最終フレームを自動調整する" にチェックが入っていない場合は何もしない。

			if (magi.auin.GetExEditFrameNumber() == 0)
				return FALSE; // 拡張編集の最終フレーム番号が無効の場合は何もしない。

			// 現在編集中のシーンのインデックスを取得する。
			int scene = magi.auin.GetCurrentSceneIndex();

			// 現在編集中のシーンの中で最も後ろにあるオブジェクト位置を取得する。
			int frameEndNumber = -1000;
			{
				// オブジェクトの個数を取得する。
				int c = magi.auin.GetCurrentSceneObjectCount();

				for (int i = 0; i < c; i++)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(i);

					if (scene != object->scene_set)
						continue; // 現在のシーン内のオブジェクトではなかった。

					frameEndNumber = std::max<int>(frameEndNumber, object->frame_end);
				}
			}

			// 最終フレーム位置を取得する。
			int frameMaxNumber = fp->exfunc->get_frame_n(fpip->editp);

			// 最終オブジェクト位置が小さすぎる場合は何もしない。
			if (frameEndNumber <= 0)
				return FALSE;

			// 最終オブジェクト位置が最終フレーム位置より大きい場合は何もしない。
			if (frameEndNumber + 1 >= frameMaxNumber)
				return FALSE;

			// 拡張編集ウィンドウを取得する。
			HWND exeditWindow = magi.auin.GetExEditWindow();

			if (!exeditWindow)
				return FALSE;

			// 「最後のオブジェクト位置を最終フレーム」コマンドをポストする。
			::PostMessage(exeditWindow, WM_COMMAND, 1097, 0);

			return TRUE;
		}
	} servant;
}
