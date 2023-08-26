#pragma once
#include "HostWindow.h"
#include "FilerWindow.h"
#include "Gui.h"

namespace fgo::filer
{
	//
	// このクラスは AviUtl にファイラを追加します。
	//
	inline struct Filer : Servant
	{
		std::shared_ptr<HostWindow> hostWindow;
		BOOL useCommonDialog = TRUE;

		//
		// この仮想関数は、このサーヴァントの名前が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"Filer";
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
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
			try
			{
				hostWindow = std::make_shared<HostWindow>();

				load();
			}
			catch (LPCTSTR e)
			{
				::MessageBox(sheba.fp->hwnd, e, _T("Filer"), MB_OK);
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
				save();

				hostWindow = 0;
			}

			return FALSE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return sheba.getConfigFileName(L"Filer.xml");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			MY_TRACE(_T("Filer::load(%s)\n"), path);

			try
			{
				MSXML2::IXMLDOMDocumentPtr document(__uuidof(MSXML2::DOMDocument));

				if (document->load(path) == VARIANT_FALSE)
				{
					MY_TRACE(_T("%ws の読み込みに失敗しました\n"), path);

					return FALSE;
				}

				MSXML2::IXMLDOMElementPtr element = document->documentElement;

				getPrivateProfileBool(element, L"useCommonDialog", useCommonDialog);
				MY_TRACE_INT(useCommonDialog);

				hostWindow->load(element);
			}
			catch (_com_error& e)
			{
				MY_TRACE(_T("%s\n"), e.ErrorMessage());

				return FALSE;
			}

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
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			MY_TRACE(_T("Filer::save(%s)\n"), path);

			return TRUE;
		}
	} servant;
}
