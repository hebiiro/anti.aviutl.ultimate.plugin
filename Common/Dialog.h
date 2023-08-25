#pragma once
#include "Window.h"

namespace Tools
{
	struct Dialog : Window
	{
		int result = 0;

		Dialog()
		{
		}

		virtual ~Dialog()
		{
			destroy();
		}

		int getResult() const { return result; }
		void setResult(int newResult) { result = newResult; }

		virtual BOOL create(HINSTANCE instance, LPCTSTR name, HWND parent, LPARAM lParam = 0)
		{
			associator.start(this);
			HWND result = ::CreateDialogParam(instance, name, parent, dlgProc, lParam);
			associator.stop();
			return !!result;
		}

		virtual BOOL close(int result)
		{
			if (!*this) return FALSE;
			::ShowWindow(*this, SW_HIDE);
			::PostMessage(*this, WM_NULL, 0, 0);
			setResult(result);
			return TRUE;
		}

		virtual int doModal()
		{
			HWND parent = ::GetWindow(*this, GW_OWNER);
			::EnableWindow(parent, FALSE);
			::ShowWindow(*this, SW_SHOW);

			MSG msg = {};
			while (::IsWindowVisible(*this) && ::GetMessage(&msg, 0, 0, 0) > 0)
			{
				if (!::IsDialogMessage(*this, &msg))
				{
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}
			}

			::EnableWindow(parent, TRUE);
			::SetForegroundWindow(parent);

			return getResult();
		}

		virtual void onOK()
		{
			close(IDOK);
		}

		virtual void onCancel()
		{
			close(IDCANCEL);
		}

		virtual INT_PTR onDlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					switch (LOWORD(wParam))
					{
					case IDOK:
						{
							onOK();
							return TRUE;
						}
					case IDCANCEL:
						{
							onCancel();
							return TRUE;
						}
					}

					break;
				}
			}

			return FALSE;
		}

		static INT_PTR CALLBACK dlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Dialog* dialog = static_cast<Dialog*>(getPointer(hwnd));
			if (!dialog) return FALSE;
			return dialog->onDlgProc(hwnd, message, wParam, lParam);
		}
	};

	struct NoCloseDialog : Dialog
	{
		virtual BOOL close(int result)
		{
			return FALSE;
		}
	};
}
