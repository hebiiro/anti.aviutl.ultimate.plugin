#pragma once
#include "Hive.h"
#include "MainWindow.h"

namespace fgo::font_select
{
	//
	// このクラスはコンフィグを管理します。
	//
	inline struct Config
	{
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
		BOOL init()
		{
			MY_TRACE_FUNC("");

			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return save(getConfigFileName().c_str());
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
			getPrivateProfileEscapeBSTR(path, L"Config", L"previewText", hive.previewText);
			getPrivateProfileBool(path, L"Config", L"marge", hive.marge);
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
		// 指定されたファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			MY_TRACE_FUNC("%ws", path);

			setPrivateProfileInt(path, L"Config", L"itemWidth", hive.itemWidth);
			setPrivateProfileInt(path, L"Config", L"itemHeight", hive.itemHeight);
			setPrivateProfileInt(path, L"Config", L"fontHeight", hive.fontHeight);
			setPrivateProfileEscapeBSTR(path, L"Config", L"previewText", hive.previewText);
			setPrivateProfileBool(path, L"Config", L"marge", hive.marge);
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
	} config;
}
