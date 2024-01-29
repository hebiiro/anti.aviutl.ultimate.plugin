#pragma once
#include "Hive.h"
#include "MainWindow.h"

namespace fgo::font_preview
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
	} config;
}
