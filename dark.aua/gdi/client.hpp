#pragma once

namespace apn::dark::gdi
{
	//
	// このクラスはGDIレンダラの利用者です。
	//
	inline struct Client
	{
		inline static constexpr LPCTSTR c_prop_name = _T("apn::dark::gdi::client");

		//
		// GDIレンダラをウィンドウにアタッチします。
		//
		inline static void attach_renderer(HWND hwnd, const auto& class_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}", hwnd, class_name);

			if (class_name == _T("#32770")) Renderer::attach(hwnd, std::make_shared<DialogRenderer>());
			else if (class_name == WC_STATIC) Renderer::attach(hwnd, std::make_shared<StaticRenderer>());
			else if (class_name == WC_BUTTON) Renderer::attach(hwnd, std::make_shared<ButtonRenderer>());
			else if (class_name == WC_EDIT) Renderer::attach(hwnd, std::make_shared<EditBoxRenderer>());
			else if (class_name == WC_LISTBOX) Renderer::attach(hwnd, std::make_shared<ListBoxRenderer>());
			else if (class_name == _T("ListviewPopup")) Renderer::attach(hwnd, std::make_shared<ListBoxRenderer>());
			else if (class_name == _T("ComboLBox")) Renderer::attach(hwnd, std::make_shared<ListBoxRenderer>());
			else if (class_name == WC_COMBOBOX) Renderer::attach(hwnd, std::make_shared<ComboBoxRenderer>());
//			else if (class_name == WC_COMBOBOXEX) Renderer::attach(hwnd, std::make_shared<ListBoxRenderer>());
			else if (class_name == WC_COMBOBOXEX) Renderer::attach(hwnd, std::make_shared<ComboBoxExRenderer>());
			else if (class_name == TOOLTIPS_CLASS) Renderer::attach(hwnd, std::make_shared<ToolTipRenderer>());
			else if (class_name == TRACKBAR_CLASS) Renderer::attach(hwnd, std::make_shared<TrackBarRenderer>());
			else if (class_name == UPDOWN_CLASS) Renderer::attach(hwnd, std::make_shared<SpinRenderer>());
			else if (class_name == WC_HEADER) Renderer::attach(hwnd, std::make_shared<HeaderRenderer>());
			else if (class_name == WC_LISTVIEW) Renderer::attach(hwnd, std::make_shared<ListViewRenderer>());
			else if (class_name == _T("DirectUIHWND")) Renderer::attach(hwnd, std::make_shared<ListViewRenderer>());
			else if (class_name == WC_TREEVIEW) Renderer::attach(hwnd, std::make_shared<TreeViewRenderer>());
			else if (class_name == TOOLBARCLASSNAME) Renderer::attach(hwnd, std::make_shared<ToolBarRenderer>());
			else if (class_name == WC_TABCONTROL) Renderer::attach(hwnd, std::make_shared<TabRenderer>());
			else if (class_name == _T("AviUtl")) Renderer::attach(hwnd, std::make_shared<AviUtlRenderer>());
			else if (class_name == _T("AviUtlButton")) Renderer::attach(hwnd, std::make_shared<AviUtlButtonRenderer>());
			else if (class_name == _T("ExtendedFilterClass")) Renderer::attach(hwnd, std::make_shared<SettingDialogRenderer>());
#if 0
			else if (class_name.find(_T("WindowsForms10.") _T("Window")) != class_name.npos) Renderer::attach(hwnd, std::make_shared<DialogRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_STATIC) != class_name.npos) Renderer::attach(hwnd, std::make_shared<StaticRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_BUTTON) != class_name.npos) Renderer::attach(hwnd, std::make_shared<ButtonRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_EDIT) != class_name.npos) Renderer::attach(hwnd, std::make_shared<EditBoxRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_COMBOBOX) != class_name.npos) Renderer::attach(hwnd, std::make_shared<ComboBoxRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_LISTBOX) != class_name.npos) Renderer::attach(hwnd, std::make_shared<ListBoxRenderer>());
			else if (class_name.find(_T("WindowsForms10.") TOOLTIPS_CLASS) != class_name.npos) Renderer::attach(hwnd, std::make_shared<ToolTipRenderer>());
			else if (class_name.find(_T("WindowsForms10.") TRACKBAR_CLASS) != class_name.npos) Renderer::attach(hwnd, std::make_shared<TrackBarRenderer>());
			else if (class_name.find(_T("WindowsForms10.") UPDOWN_CLASS) != class_name.npos) Renderer::attach(hwnd, std::make_shared<SpinRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_LISTVIEW) != class_name.npos) Renderer::attach(hwnd, std::make_shared<ListViewRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_TREEVIEW) != class_name.npos) Renderer::attach(hwnd, std::make_shared<TreeViewRenderer>());
			else if (class_name.find(_T("WindowsForms10.") TOOLBARCLASSNAME) != class_name.npos) Renderer::attach(hwnd, std::make_shared<ToolBarRenderer>());
			else if (class_name.find(_T("WindowsForms10.") WC_TABCONTROL) != class_name.npos) Renderer::attach(hwnd, std::make_shared<TabRenderer>());
#endif
		}

		//
		// ウィンドウを微調整します。
		//
		inline static void tweak_window(HWND hwnd, const auto& class_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}", hwnd, class_name);

			if (my::get_style(hwnd) & WS_CAPTION)
			{
				if (!hive.renderer_locked)
					skin::dwm.set_window_attribute(hwnd, FALSE);
			}
			else if (class_name == WC_BUTTON)
			{
				if (my::get_ex_style(hwnd) & WS_EX_STATICEDGE)
					hive.static_edge_buttons.emplace_back(hwnd);
			}
			else if (class_name == WC_STATIC)
			{
				// ラウドネスメーター(全体)のスタティックテキストを微調整します。

				auto parent = ::GetParent(hwnd);
				auto window_name = my::get_window_text(parent);

				if (window_name == _T("ラウドネスメーター(全体)"))
					my::modify_style(hwnd, 0, SS_CENTERIMAGE);
			}
		}

		//
		// レンダラーの初期化処理を実行します。
		//
		inline static void init_renderer(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			// ウィンドウのクラス名を取得します。
			auto class_name = my::get_class_name(hwnd);
			MY_TRACE_STR(class_name);

			// GDIレンダラーをアタッチします。
			attach_renderer(hwnd, class_name);

			// ウィンドウを微調整します。
			tweak_window(hwnd, class_name);
		}

		//
		// レンダラーの後始末処理を実行します。
		//
		inline static void exit_renderer(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			Renderer::detach(hwnd);
		}

		//
		// オリジナルのウィンドウプロシージャを呼び出します。
		//
		inline static LRESULT CALLBACK orig_proc(MessageState* current_state)
		{
			return hive.orig.CallWindowProcInternal(current_state->wnd_proc, current_state->hwnd, current_state->message, current_state->wParam, current_state->lParam);
		}

		//
		// ウィンドウプロシージャのハンドラです。
		//
		inline static LRESULT CALLBACK on_wnd_proc(WNDPROC wnd_proc, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
		{
			auto current_state = MessageState { wnd_proc, hwnd, message, wParam, lParam };

			if (message == WM_NCCREATE && !::GetProp(hwnd, c_prop_name))
			{
				::SetProp(hwnd, c_prop_name, &client);

				init_renderer(hwnd);
			}
			else if (message == WM_NCDESTROY && ::GetProp(hwnd, c_prop_name))
			{
				exit_renderer(hwnd);
			}

//			MY_TRACE_FUNC("{:#010x}, {:#010x}, {:#010x}, {:#010x}, {:#010x}", wnd_proc, hwnd, message, wParam, lParam);

			// レンダラーの使用が抑制されている場合はデフォルト処理のみ行います。
			if (hive.renderer_locked)
				return orig_proc(&current_state);

			switch (message)
			{
			case WM_ACTIVATE:
				{
					auto active = LOWORD(wParam);

					MY_TRACE_FUNC("WM_ACTIVATE, {}", active);

					skin::dwm.set_window_attribute(hwnd, active);

					break;
				}
			case WM_RBUTTONUP:
				{
					// テスト

					MY_TRACE_HWND(hwnd);
					auto parent = ::GetParent(hwnd);
					MY_TRACE_HWND(parent);
					auto rc_hwnd = my::get_window_rect(hwnd);
					auto rc_parent = my::get_window_rect(parent);

					//::SetWindowPos(parent, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

					auto renderer = Renderer::from_handle(hwnd);

					int n = 0;

					break;
				}
			case WM_CTLCOLORDLG:
			case WM_CTLCOLORMSGBOX:
			case WM_CTLCOLORBTN:
			case WM_CTLCOLOREDIT:
			case WM_CTLCOLORLISTBOX:
			case WM_CTLCOLORSCROLLBAR:
			case WM_CTLCOLORSTATIC:
				{
//					MY_TRACE_FUNC("WM_CTLCOLOR, {:#010x}, {:#010x}, {:#010x}, {:#010x}", hwnd, message, wParam, lParam);

					auto dc = (HDC)wParam;
					auto control = (HWND)lParam;
					auto brush = (HBRUSH)orig_proc(&current_state);

					MY_TRACE_HWND(control);

					// レンダラーを取得します。
					auto renderer = Renderer::from_handle(control);
					if (!renderer) return (LRESULT)brush;

					return (LRESULT)renderer->on_ctl_color(hwnd, message, dc, control, brush);
				}
			case WM_NOTIFY:
				{
					auto nm = (NMHDR*)lParam;

					if (my::get_class_name(nm->hwndFrom) == WC_BUTTON)
						return 0; // ボタンの場合は何もしない。

					switch (nm->code)
					{
					case NM_CUSTOMDRAW:
						{
							auto renderer = Renderer::from_handle(nm->hwndFrom);
							if (renderer)
								return renderer->on_custom_draw(&current_state);

							break;
						}
					}

					break;
				}
			}

			auto result = LRESULT {};
			auto old_state = manager.get_current_state();
			manager.set_current_state(current_state);
			auto renderer = Renderer::from_handle(hwnd);
			if (renderer)
				result = renderer->on_subclass_proc(&current_state);
			else
				result = orig_proc(&current_state);
			manager.set_current_state(old_state);
			return result;
		}
	} client;
}
