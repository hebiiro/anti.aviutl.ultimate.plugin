#pragma once
#include "Paint.h"
#include "SettingDialog.h"
#include "MainWindow.h"

namespace fgo::font_select
{
	//
	// このクラスはフォントを選択するためのカスタムメニューを表示します。
	//
	inline struct FontSelect : Servant, Magi::CommandID::Plugin
	{
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
			return magi.getConfigFileName(L"FontSelect.ini");
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

			getPrivateProfileInt(path, L"Config", L"itemWidth", hive.itemWidth);
			getPrivateProfileInt(path, L"Config", L"itemHeight", hive.itemHeight);
			getPrivateProfileInt(path, L"Config", L"fontHeight", hive.fontHeight);
			getPrivateProfileBSTR(path, L"Config", L"previewText", hive.previewText);
			getPrivateProfileLabel(path, L"Config", L"paint.mode", paint.mode, Paint::Mode::label);
			getPrivateProfileBSTR(path, L"Config", L"paint.normal.name", paint.state[Paint::StateID::Normal].name);
			getPrivateProfileColor(path, L"Config", L"paint.normal.fill", paint.state[Paint::StateID::Normal].color.fill);
			getPrivateProfileColor(path, L"Config", L"paint.normal.text", paint.state[Paint::StateID::Normal].color.text);
			getPrivateProfileColor(path, L"Config", L"paint.normal.font", paint.state[Paint::StateID::Normal].color.font);
			getPrivateProfileBSTR(path, L"Config", L"paint.select.name", paint.state[Paint::StateID::Select].name);
			getPrivateProfileColor(path, L"Config", L"paint.select.fill", paint.state[Paint::StateID::Select].color.fill);
			getPrivateProfileColor(path, L"Config", L"paint.select.text", paint.state[Paint::StateID::Select].color.text);
			getPrivateProfileColor(path, L"Config", L"paint.select.font", paint.state[Paint::StateID::Select].color.font);
			getPrivateProfileBool(path, L"Config", L"useContextMenu", hive.useContextMenu);
			getPrivateProfileBool(path, L"Config", L"useBackward", hive.useBackward);
			getPrivateProfileBool(path, L"Config", L"useForward", hive.useForward);
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

			setPrivateProfileInt(path, L"Config", L"itemWidth", hive.itemWidth);
			setPrivateProfileInt(path, L"Config", L"itemHeight", hive.itemHeight);
			setPrivateProfileInt(path, L"Config", L"fontHeight", hive.fontHeight);
			setPrivateProfileBSTR(path, L"Config", L"previewText", hive.previewText);
			setPrivateProfileLabel(path, L"Config", L"paint.mode", paint.mode, Paint::Mode::label);
			setPrivateProfileBSTR(path, L"Config", L"paint.normal.name", paint.state[Paint::StateID::Normal].name);
			setPrivateProfileColor(path, L"Config", L"paint.normal.fill", paint.state[Paint::StateID::Normal].color.fill);
			setPrivateProfileColor(path, L"Config", L"paint.normal.text", paint.state[Paint::StateID::Normal].color.text);
			setPrivateProfileColor(path, L"Config", L"paint.normal.font", paint.state[Paint::StateID::Normal].color.font);
			setPrivateProfileBSTR(path, L"Config", L"paint.select.name", paint.state[Paint::StateID::Select].name);
			setPrivateProfileColor(path, L"Config", L"paint.select.fill", paint.state[Paint::StateID::Select].color.fill);
			setPrivateProfileColor(path, L"Config", L"paint.select.text", paint.state[Paint::StateID::Select].color.text);
			setPrivateProfileColor(path, L"Config", L"paint.select.font", paint.state[Paint::StateID::Select].color.font);
			setPrivateProfileBool(path, L"Config", L"useContextMenu", hive.useContextMenu);
			setPrivateProfileBool(path, L"Config", L"useBackward", hive.useBackward);
			setPrivateProfileBool(path, L"Config", L"useForward", hive.useForward);
			setPrivateProfileWindow(path, L"MainWindow", mainWindow);

			return TRUE;
		}
	} servant;
}
