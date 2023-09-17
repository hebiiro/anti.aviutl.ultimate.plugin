#pragma once
#include "HostWindow.h"

namespace fgo::filer
{
	//
	// このクラスは AviUtl にファイラを追加します。
	//
	inline struct Filer : Servant, Magi::CommandID::Plugin
	{
		std::shared_ptr<HostWindow> hostWindow;

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"Filer";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"ファイラ";
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"ObjectExplorer.auf"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"FilerとObjectExplorer.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			return TRUE;
		}

		//
		// この仮想関数は、ウィンドウの初期化のタイミングで呼ばれます。
		//
		BOOL on_window_init(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			// Nestから補助関数をインポートします。
			hive.nest.init();

			// ホストウィンドウの作成に失敗する可能性もあるので、
			// メインウィンドウをAviUtlウィンドウにしておきます。
			hive.mainWindow = magi.auin.GetAviUtlWindow();

			try
			{
				// ホストウィンドウを作成します。
				hostWindow = std::make_shared<HostWindow>();
				MainDialog::listener = hostWindow;
				FilerWindow::listener = hostWindow->mainDialog;

				// ホストウィンドウをメインウィンドウに設定します。
				hive.mainWindow = *hostWindow;

				// ホストプロセスの初期化が開始されたことをクライアントプロセスに通知します。
				hostWindow->sendMessage(Share::Filer::Message::PreInit, 0, 0);

				// 無駄な処理を省くために
				// コンフィグファイルを読み込む前にメインダイアログをロックします。
				hostWindow->mainDialog->setLock(TRUE);

				// コンフィグファイルから設定を読み込みます。
				if (load())
				{
					// ホストプロセスの初期化が完了したことをクライアントプロセスに通知します。
					hostWindow->postMessage(Share::Filer::Message::PostInit, 0, 0);
				}

				// メインダイアログのロックを解除し、コントロールをリフレッシュします。
				hostWindow->mainDialog->setLock(FALSE);
				hostWindow->mainDialog->refresh();
			}
			catch (LPCTSTR e)
			{
				::MessageBox(hive.mainWindow, e, hive.AppName, MB_OK);
			}

			return FALSE;
		}

		//
		// この仮想関数は、ウィンドウの後始末のタイミングで呼ばれます。
		//
		BOOL on_window_exit(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp) override
		{
			if (hostWindow)
			{
				// ホストプロセスの後始末が開始されたことをクライアントプロセスに通知します。
				hostWindow->sendMessage(Share::Filer::Message::PreExit, 0, 0);

				// ファイルに設定を保存します。
				if (save())
				{
					// ホストプロセスの後始末が完了したことをクライアントプロセスに通知します。
					hostWindow->sendMessage(Share::Filer::Message::PostExit, 0, 0);
				}

				// ホストウィンドウを削除します。
				hostWindow = 0;
			}

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
					if (hostWindow)
						Tools::AviUtl::PluginWindowExtension::show(*hostWindow);

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
			return magi.getConfigFileName(L"Filer.xml");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str()) == S_OK;
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		HRESULT load(LPCWSTR path)
		{
			MY_TRACE(_T("Filer::load(%s)\n"), path);

			try
			{
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				if (document->load(path) == VARIANT_FALSE)
				{
					MY_TRACE(_T("%ws の読み込みに失敗しました\n"), path);

					return S_FALSE;
				}

				MSXML2::IXMLDOMElementPtr element = document->documentElement;

				getPrivateProfileBool(element, L"useCommonDialog", hive.useCommonDialog);
				MY_TRACE_INT(hive.useCommonDialog);

				hostWindow->load(element);
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());
				return e.Error();
			}

			return S_OK;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str()) == S_OK;
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		HRESULT save(LPCWSTR path)
		{
			MY_TRACE(_T("Filer::save(%s)\n"), path);

			try
			{
				// ドキュメントを作成します。
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				// ドキュメントエレメントを作成します。
				MSXML2::IXMLDOMElementPtr element = appendElement(document, document, L"config");

				setPrivateProfileBool(element, L"useCommonDialog", hive.useCommonDialog);

				hostWindow->save(element);

				return saveXMLDocument(document, path, L"UTF-16");
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());
				return e.Error();
			}

			return S_OK;
		}
	} servant;
}
