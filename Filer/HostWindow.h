#pragma once
#include "Gui.h"
#include "FilerWindow.h"

namespace fgo::filer
{
	//
	// このクラスはホストプロセスのメインウィンドウです。
	//
	struct HostWindow : Tools::AviUtl::PluginWindow
	{
		//
		// GUI を担当するクライアントプロセスです。
		//
		std::shared_ptr<Gui> gui;

		//
		// ファイラウィンドウのコレクションです。
		//
		std::map<std::wstring, std::shared_ptr<FilerWindow>> filerWindows;

		//
		// コンストラクタです。
		//
		HostWindow()
		{
			MY_TRACE(_T("HostWindow::HostWindow()\n"));

			if (!Tools::AviUtl::PluginWindowExtension::create(
				*this,
				hive.instance,
				fate.auin.GetAviUtlWindow(),
				_T("ファイラ"),
				0,
				WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
				100, 100, 400, 400))
			{
				throw _T("ホストウィンドウの失敗しました");
			}

			gui = std::make_shared<Gui>(*this);
		}

		//
		// デストラクタです。
		//
		~HostWindow()
		{
			MY_TRACE(_T("HostWindow::~HostWindow()\n"));
		}

		//
		// ファイラウィンドウを作成して返します。
		// すでに作成済みの場合はそのファイラウィンドウを返します。
		//
		std::shared_ptr<FilerWindow> createFilerWindow(LPCWSTR name)
		{
			MY_TRACE(_T("HostWindow::createFilerWindow(%ws)\n"), name);

			auto it = filerWindows.find(name);
			if (it != filerWindows.end()) return it->second;
			return filerWindows[name] = std::make_shared<FilerWindow>(
				name, Share::Filer::HostWindow::getClientWindow(*this));
		}

		//
		// 設定を読み込みます。
		//
		BOOL load(const MSXML2::IXMLDOMElementPtr& element)
		{
			MY_TRACE(_T("HostWindow::load()\n"));

			getPrivateProfileWindow(element, L"placement", *this, SW_SHOW);

			MSXML2::IXMLDOMNodeListPtr nodeList = element->selectNodes(L"filer");
			int c = nodeList->length;
			for (int i = 0; i < c; i++)
			{
				MSXML2::IXMLDOMElementPtr element = nodeList->item[i];

				_bstr_t name;
				getPrivateProfileString(element, L"name", name);
				MY_TRACE_WSTR((BSTR)name);

				if ((BSTR)name)
				{
					try
					{
						auto filerWindow = createFilerWindow(name);

						filerWindow->load(element);
					}
					catch (LPCTSTR e)
					{
						::MessageBox(fate.fp->hwnd, e, _T("Filer"), MB_OK);
					}
				}
			}

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		LRESULT onWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			return __super::onWndProc(hwnd, message, wParam, lParam);
		}
	};
}
