﻿#pragma once

namespace my
{
	struct Dialog : Window
	{
		int result = 0;

		Dialog()
		{
		}

		virtual ~Dialog() override
		{
			destroy();
		}

		int get_result() const { return result; }
		void set_result(int result) { this->result = result; }

		virtual BOOL create(HINSTANCE instance, LPCTSTR name, HWND parent, LPARAM lParam = 0)
		{
			associator.start(this);
			HWND result = ::CreateDialogParam(instance, name, parent, dlg_proc, lParam);
			associator.stop();
			return !!result;
		}

		virtual BOOL close(int result)
		{
			if (!*this) return FALSE;
			::ShowWindow(*this, SW_HIDE);
			::PostMessage(*this, WM_NULL, 0, 0);
			set_result(result);
			return TRUE;
		}

		virtual int do_modal()
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

			return get_result();
		}

		virtual void on_ok()
		{
			close(IDOK);
		}

		virtual void on_cancel()
		{
			close(IDCANCEL);
		}

		virtual INT_PTR on_dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			switch (message)
			{
			case WM_COMMAND:
				{
					switch (LOWORD(wParam))
					{
					case IDOK:
						{
							on_ok();
							return TRUE;
						}
					case IDCANCEL:
						{
							on_cancel();
							return TRUE;
						}
					}

					break;
				}
			}

			return FALSE;
		}

		static INT_PTR CALLBACK dlg_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			Dialog* dialog = static_cast<Dialog*>(from_handle(hwnd));
			if (!dialog) return FALSE;
			return dialog->on_dlg_proc(hwnd, message, wParam, lParam);
		}

		BOOL set_text(UINT id, LPCTSTR text)
		{
			return ::SetDlgItemText(*this, id, text);
		}

		BOOL set_int(UINT id, int value)
		{
			return ::SetDlgItemInt(*this, id, value, TRUE);
		}

		BOOL set_uint(UINT id, UINT value)
		{
			return ::SetDlgItemInt(*this, id, value, FALSE);
		}

		BOOL set_float(UINT id, float value)
		{
			return set_text(id, std::format(_T("{}"), value).c_str());
		}

		void set_check(UINT id, BOOL value)
		{
			::SendDlgItemMessage(*this, id, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
		}

		int set_combobox(UINT id, int value)
		{
			return (int)::SendDlgItemMessage(*this, id, CB_SETCURSEL, value, 0);
		}

		template <typename... Tail>
		void set_combobox(UINT id, int value, LPCTSTR text, Tail&&... tail)
		{
			::SendDlgItemMessage(*this, id, CB_ADDSTRING, 0, (LPARAM)text);

			set_combobox(id, value, std::forward<Tail>(tail)...);
		}

		std::basic_string<TCHAR> get_text(UINT id, size_t buffer_length = MAX_PATH)
		{
			std::basic_string<TCHAR> str;
			str.resize(buffer_length);
			size_t size = ::GetDlgItemText(*this, id, &str[0], str.length());
			str.resize(size);
			return str;
		}

		int get_int(UINT id)
		{
			return ::GetDlgItemInt(*this, id, 0, TRUE);
		}

		UINT get_uint(UINT id)
		{
			return ::GetDlgItemInt(*this, id, 0, FALSE);
		}

		float get_float(UINT id)
		{
			try { return std::stof(get_text(id)); } catch (...) { return 0; }
		}

		BOOL get_check(UINT id)
		{
			return ::SendDlgItemMessage(*this, id, BM_GETCHECK, 0, 0) == BST_CHECKED;
		}

		int get_combobox(UINT id)
		{
			return (int)::SendDlgItemMessage(*this, id, CB_GETCURSEL, 0, 0);
		}

		//
		// コントロールサイズの基準となる数値を返します。
		//
		int get_base_size()
		{
			// フォントを取得します。
			HFONT font = (HFONT)::SendMessage(*this, WM_GETFONT, 0, 0);
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
