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
			Dialog* dialog = static_cast<Dialog*>(fromHandle(hwnd));
			if (!dialog) return FALSE;
			return dialog->onDlgProc(hwnd, message, wParam, lParam);
		}

		BOOL setInt(UINT id, int value)
		{
			return ::SetDlgItemInt(*this, id, value, TRUE);
		}

		BOOL setUInt(UINT id, UINT value)
		{
			return ::SetDlgItemInt(*this, id, value, FALSE);
		}

		void setCheck(UINT id, BOOL value)
		{
			::SendDlgItemMessage(*this, id, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
		}

		int setComboBox(UINT id, int value)
		{
			return (int)::SendDlgItemMessage(*this, id, CB_SETCURSEL, value, 0);
		}

		template <class... Tail>
		void setComboBox(UINT id, int value, LPCTSTR text, Tail&&... tail)
		{
			::SendDlgItemMessage(*this, id, CB_ADDSTRING, 0, (LPARAM)text);

			setComboBox(id, value, std::forward<Tail>(tail)...);
		}

		int getInt(UINT id)
		{
			return ::GetDlgItemInt(*this, id, 0, TRUE);
		}

		UINT getUInt(UINT id)
		{
			return ::GetDlgItemInt(*this, id, 0, FALSE);
		}

		BOOL getCheck(UINT id)
		{
			return ::SendDlgItemMessage(*this, id, BM_GETCHECK, 0, 0) == BST_CHECKED;
		}

		int getComboBox(UINT id)
		{
			return (int)::SendDlgItemMessage(*this, id, CB_GETCURSEL, 0, 0);
		}

		//
		// コントロールサイズの基準となる数値を返します。
		//
		int getBaseSize()
		{
			// フォントを取得します。
			HFONT font = (HFONT)::SendMessage(*this, WM_GETFONT, 0, 0);
			MY_TRACE_HEX(font);
			if (!font) font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

			// フォントのメトリックを取得します。
			ClientDC dc(*this);
			GdiObjSelector selector(dc, font);
			TEXTMETRIC tm = {}; ::GetTextMetrics(dc, &tm);

			// フォントの高さを返します。
			// 20 == 最低値
			// 8 == コントロールのエッジ幅(2 * 2) + 余白(2 * 2)
			return std::max<int>(20, tm.tmHeight + 8);
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
