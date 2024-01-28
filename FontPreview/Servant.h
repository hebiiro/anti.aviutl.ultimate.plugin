#pragma once
#include "Paint.h"
#include "Hook.h"
#include "SettingDialog.h"
#include "MainWindow.h"

namespace fgo::font_preview
{
	//
	// このクラスはフォントのプレビューを表示します。
	//
	inline struct FontPreview : Servant, Magi::CommandID::Plugin
	{
		Hook hook;
		SettingDialog settingDialog;
		MainWindow mainWindow;

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

			return fp->exfunc->add_menu_item(fp, itemText, mainWindow.mainDialog, id, 0, AviUtl::ExFunc::AddMenuItemFlag::None);
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (!paint.init()) return FALSE;
			if (!hook.init()) return FALSE;
			if (!settingDialog.init()) return FALSE;
			if (!mainWindow.init()) return FALSE;
			if (!load()) return FALSE;

			hive.mainWindow = mainWindow;

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			save();
			mainWindow.exit();
			settingDialog.exit();
			hook.exit();
			paint.exit();

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
					if (::IsWindow(mainWindow))
						mainWindow.show();

					break;
				}
			}

			return FALSE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(hive.Name) + L".ini";
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

			getPrivateProfileInt(path, L"Config", L"itemHeight", hive.itemHeight);
			getPrivateProfileInt(path, L"Config", L"listboxWidth", hive.listboxWidth);
			getPrivateProfileInt(path, L"Config", L"listboxHeight", hive.listboxHeight);
			getPrivateProfileEscapeBSTR(path, L"Config", L"previewText", hive.previewText);
			getPrivateProfileBool(path, L"Config", L"marge", hive.marge);
			getPrivateProfileLabel(path, L"Config", L"paint.mode", paint.mode, Paint::Mode::label);
			getPrivateProfileBSTR(path, L"Config", L"paint.normal.name", paint.state[Paint::StateID::Normal].name);
			getPrivateProfileColor(path, L"Config", L"paint.normal.fill", paint.state[Paint::StateID::Normal].color.fill);
			getPrivateProfileColor(path, L"Config", L"paint.normal.text", paint.state[Paint::StateID::Normal].color.text);
			getPrivateProfileBSTR(path, L"Config", L"paint.select.name", paint.state[Paint::StateID::Select].name);
			getPrivateProfileColor(path, L"Config", L"paint.select.fill", paint.state[Paint::StateID::Select].color.fill);
			getPrivateProfileColor(path, L"Config", L"paint.select.text", paint.state[Paint::StateID::Select].color.text);
			getPrivateProfileWindow(path, L"MainWindow", mainWindow);

			mainWindow.mainDialog.updateControls();

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

			setPrivateProfileInt(path, L"Config", L"itemHeight", hive.itemHeight);
			setPrivateProfileInt(path, L"Config", L"listboxWidth", hive.listboxWidth);
			setPrivateProfileInt(path, L"Config", L"listboxHeight", hive.listboxHeight);
			setPrivateProfileEscapeBSTR(path, L"Config", L"previewText", hive.previewText);
			setPrivateProfileBool(path, L"Config", L"marge", hive.marge);
			setPrivateProfileLabel(path, L"Config", L"paint.mode", paint.mode, Paint::Mode::label);
			setPrivateProfileBSTR(path, L"Config", L"paint.normal.name", paint.state[Paint::StateID::Normal].name);
			setPrivateProfileColor(path, L"Config", L"paint.normal.fill", paint.state[Paint::StateID::Normal].color.fill);
			setPrivateProfileColor(path, L"Config", L"paint.normal.text", paint.state[Paint::StateID::Normal].color.text);
			setPrivateProfileBSTR(path, L"Config", L"paint.select.name", paint.state[Paint::StateID::Select].name);
			setPrivateProfileColor(path, L"Config", L"paint.select.fill", paint.state[Paint::StateID::Select].color.fill);
			setPrivateProfileColor(path, L"Config", L"paint.select.text", paint.state[Paint::StateID::Select].color.text);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}
	} servant;
}
