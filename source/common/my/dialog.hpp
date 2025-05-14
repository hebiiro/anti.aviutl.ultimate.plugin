#pragma once

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
			Hooker hooker(*this);
			return !!::CreateDialogParam(instance, name, parent, dlg_proc, lParam);
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
			auto parent = ::GetWindow(*this, GW_OWNER);
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
			auto dialog = static_cast<Dialog*>(from_handle(hwnd));
			if (!dialog) return FALSE;
			return dialog->on_dlg_proc(hwnd, message, wParam, lParam);
		}

		// init

		void init_combobox(UINT id, LPCTSTR text)
		{
			::SendDlgItemMessage(*this, id, CB_ADDSTRING, 0, (LPARAM)text);
		}

		template <typename... Tail>
		void init_combobox(UINT id, LPCTSTR text, Tail&&... tail)
		{
			::SendDlgItemMessage(*this, id, CB_ADDSTRING, 0, (LPARAM)text);

			init_combobox(id, std::forward<Tail>(tail)...);
		}

		// get

		tstring get_text(UINT id, size_t buffer_length = MAX_PATH)
		{
			tstring str(buffer_length, _T('\0'));
			auto size = ::GetDlgItemText(*this, id, str.data(), (int)str.length());
			str.resize(size);
			return str;
		}

		int get_int(UINT id)
		{
			return ::GetDlgItemInt(*this, id, nullptr, TRUE);
		}

		UINT get_uint(UINT id)
		{
			return ::GetDlgItemInt(*this, id, nullptr, FALSE);
		}

		float get_float(UINT id)
		{
			return wcstof(get_text(id).c_str(), nullptr);
		}

		BOOL get_check(UINT id)
		{
			return ::SendDlgItemMessage(*this, id, BM_GETCHECK, 0, 0) == BST_CHECKED;
		}

		int get_combobox_index(UINT id)
		{
			return (int)::SendDlgItemMessage(*this, id, CB_GETCURSEL, 0, 0);
		}

		tstring get_combobox_string(UINT id)
		{
			return my::get_text_from_combobox(::GetDlgItem(*this, id));
		}

		// get(参照渡し方式)

		void get_text(UINT id, tstring& value, size_t buffer_length = MAX_PATH)
		{
			value = get_text(id, buffer_length);
		}

		template <typename T>
		void get_int(UINT id, T& value)
		{
			value = (T)get_int(id);
		}

		template <typename T>
		void get_uint(UINT id, T& value)
		{
			value = (T)get_uint(id);
		}

		template <typename T>
		void get_float(UINT id, T& value)
		{
			value = (T)get_float(id);
		}

		template <typename T>
		void get_check(UINT id, T& value)
		{
			value = (T)get_check(id);
		}

		template <typename T>
		void get_combobox_index(UINT id, T& value)
		{
			value = (T)get_combobox_index(id);
		}

		template <typename T>
		void get_combobox_string(UINT id, T& value)
		{
			value = get_combobox_string(id);
		}

		// set

		BOOL set_text(UINT id, const tstring& text)
		{
			return ::SetDlgItemText(*this, id, text.c_str());
		}

		BOOL set_int(UINT id, int value)
		{
			return ::SetDlgItemInt(*this, id, value, TRUE);
		}

		BOOL set_uint(UINT id, UINT value)
		{
			return ::SetDlgItemInt(*this, id, value, FALSE);
		}

		template <typename T>
		BOOL set_float(UINT id, T value, LPCTSTR format = _T("{/}"))
		{
			return set_text(id, my::format(format, value).c_str());
		}

		void set_check(UINT id, BOOL value)
		{
			::SendDlgItemMessage(*this, id, BM_SETCHECK, value ? BST_CHECKED : BST_UNCHECKED, 0);
		}

		int set_combobox_index(UINT id, int value)
		{
			return (int)::SendDlgItemMessage(*this, id, CB_SETCURSEL, value, 0);
		}

		void set_combobox_string(UINT id, const tstring& value)
		{
			auto index = (int)::SendDlgItemMessage(*this, id, CB_FINDSTRING, -1, (LPARAM)value.c_str());

			::SendDlgItemMessage(*this, id, CB_SETCURSEL, index, 0);
		}

		//
		// コントロールサイズの基準となる数値を返します。
		//
		int get_base_size()
		{
#if 1
			auto rc = RECT { 4, 8 };
			::MapDialogRect(*this, &rc);

			// ダイアログ矩形の高さを調整して返します。
			// 20 == 最低値
			// 8 == コントロールのエッジ幅(2 * 2) + 余白(2 * 2)
			return std::max<int>(20, rc.top + 8);
#else
			// フォントを取得します。
			auto font = (HFONT)::SendMessage(*this, WM_GETFONT, 0, 0);
			if (!font) font = (HFONT)::GetStockObject(DEFAULT_GUI_FONT);

			// フォントのメトリックを取得します。
			ClientDC dc(*this);
			gdi::selector font_selector(dc, font);
			TEXTMETRIC tm = {}; ::GetTextMetrics(dc, &tm);

			// フォントの高さを調整して返します。
			// 20 == 最低値
			// 8 == コントロールのエッジ幅(2 * 2) + 余白(2 * 2)
			return std::max<int>(20, tm.tmHeight + 8);
#endif
		}
	};

	struct NoCloseDialog : Dialog
	{
		virtual BOOL close(int result) override
		{
			return FALSE;
		}
	};
}
