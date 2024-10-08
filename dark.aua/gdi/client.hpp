#pragma once

namespace apn::dark::gdi
{
	//
	// このクラスはGDIレンダラの利用者です。
	//
	inline struct Client
	{
		inline static constexpr auto c_prop_name = _T("apn::dark::gdi::client");

		//
		// このクラスはウィンドウのクラス名です。
		//
		struct ClassName : my::tstring {
			ClassName(const my::tstring& class_name) : my::tstring(class_name) {}
			bool operator==(LPCTSTR b) const { return ::lstrcmpi(c_str(), b) == 0; }
		};

		//
		// 指定された条件に合致するGDIレンダラを返します。
		//
		std::shared_ptr<Renderer> find_renderer(HWND hwnd, const ClassName& class_name)
		{
			if (class_name == _T("#32770")) return std::make_shared<DialogRenderer>();
			if (class_name == WC_STATIC) return std::make_shared<StaticRenderer>();
			if (class_name == WC_BUTTON) return std::make_shared<ButtonRenderer>();
			if (class_name == WC_EDIT) return std::make_shared<EditBoxRenderer>();
			if (class_name == WC_LISTBOX) return std::make_shared<ListBoxRenderer>();
			if (class_name == _T("ListviewPopup")) return std::make_shared<ListBoxRenderer>();
			if (class_name == _T("ComboLBox")) return std::make_shared<ListBoxRenderer>();
			if (class_name == WC_COMBOBOX) return std::make_shared<ComboBoxRenderer>();
//			if (class_name == WC_COMBOBOXEX) return std::make_shared<ListBoxRenderer>();
			if (class_name == WC_COMBOBOXEX) return std::make_shared<ComboBoxExRenderer>();
			if (class_name == TOOLTIPS_CLASS) return std::make_shared<ToolTipRenderer>();
			if (class_name == TRACKBAR_CLASS) return std::make_shared<TrackBarRenderer>();
			if (class_name == UPDOWN_CLASS) return std::make_shared<SpinRenderer>();
			if (class_name == WC_HEADER)
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
				if (instance == ::GetModuleHandle(_T("shell32.dll")))
					return std::make_shared<ShellHeaderRenderer>();

				return std::make_shared<HeaderRenderer>();
			}
			if (class_name == WC_LISTVIEW)
			{
				auto instance = (HINSTANCE)::GetWindowLongPtr(hwnd, GWLP_HINSTANCE);
				if (instance == ::GetModuleHandle(_T("shell32.dll")))
					return std::make_shared<ShellListViewRenderer>();

				return std::make_shared<ListViewRenderer>();
			}
			if (class_name == _T("DirectUIHWND")) return std::make_shared<ShellListViewRenderer>();
			if (class_name == WC_TREEVIEW) return std::make_shared<TreeViewRenderer>();
			if (class_name == TOOLBARCLASSNAME) return std::make_shared<ToolBarRenderer>();
			if (class_name == WC_TABCONTROL) return std::make_shared<TabRenderer>();
			if (class_name == _T("AviUtl")) return std::make_shared<AviUtlRenderer>();
			if (class_name == _T("AviUtlButton")) return std::make_shared<AviUtlButtonRenderer>();
			if (class_name == _T("ExtendedFilterClass")) return std::make_shared<SettingDialogRenderer>();
			if (class_name == _T("apn::workspace::container")) return std::make_shared<RendererNc>();

			return nullptr;
		}

		//
		// GDIレンダラをウィンドウにアタッチします。
		//
		void attach_renderer(HWND hwnd, const ClassName& class_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}", hwnd, class_name.c_str());

			if (auto renderer = find_renderer(hwnd, class_name))
				Renderer::attach(hwnd, renderer);
		}

		//
		// ウィンドウを微調整します。
		//
		void tweak_window(HWND hwnd, const ClassName& class_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}", hwnd, class_name.c_str());

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
		void init_renderer(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			// ウィンドウのクラス名を取得します。
			auto class_name = ClassName(my::get_class_name(hwnd));
			MY_TRACE_STR(class_name.c_str());

			// GDIレンダラーをアタッチします。
			attach_renderer(hwnd, class_name);

			// ウィンドウを微調整します。
			tweak_window(hwnd, class_name);
		}

		//
		// レンダラーの後始末処理を実行します。
		//
		void exit_renderer(HWND hwnd)
		{
			MY_TRACE_FUNC("{:#010x}", hwnd);

			Renderer::detach(hwnd);
		}

		//
		// オリジナルのウィンドウプロシージャを呼び出します。
		//
		inline static LRESULT CALLBACK orig_wnd_proc(MessageState* current_state)
		{
			return Renderer::orig_wnd_proc(current_state);
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

				client.init_renderer(hwnd);
			}
			else if (message == WM_NCDESTROY && ::GetProp(hwnd, c_prop_name))
			{
				client.exit_renderer(hwnd);
			}

			if (0) // テスト用のコードです。
			{
				auto class_name = my::get_class_name(hwnd);

				MY_TRACE_FUNC("{}, {:#010x}, {:#010x}, {:#010x}, {:#010x}, {:#010x}",
					class_name, wnd_proc, hwnd, message, wParam, lParam);

				if (0)
				{
					auto window_dpi = ::GetDpiForWindow(hwnd);
					auto system_dpi = ::GetDpiForSystem();
					auto dpi1 = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);
					auto dpi2 = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);
					auto dpi3 = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE);
					auto dpi4 = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
					auto dpi5 = ::GetDpiFromDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED);

					MY_TRACE("{}, {}, {}, {}, {}, {}, {}\n", window_dpi, system_dpi, dpi1, dpi2, dpi3, dpi4, dpi5);
				}
			}

			// レンダラーの使用が抑制されている場合はデフォルト処理のみ行います。
			if (hive.renderer_locked)
				return orig_wnd_proc(&current_state);

			switch (message)
			{
			case WM_ACTIVATE:
				{
					auto active = LOWORD(wParam);

					MY_TRACE_FUNC("WM_ACTIVATE, {}", active);

					skin::dwm.set_window_attribute(hwnd, active);

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
					auto brush = (HBRUSH)orig_wnd_proc(&current_state);

					MY_TRACE_HWND(control);

					// レンダラーを取得します。
					if (auto renderer = Renderer::from_handle(control))
						return (LRESULT)renderer->on_ctl_color(hwnd, message, dc, control, brush);
					else
						return (LRESULT)brush;
				}
			case WM_NOTIFY:
				{
					auto nm = (NMHDR*)lParam;

					if (my::get_class_name(nm->hwndFrom) == WC_BUTTON)
						return 0; // ボタンの場合は何もしません。

					switch (nm->code)
					{
					case NM_CUSTOMDRAW:
						{
							if (auto renderer = Renderer::from_handle(nm->hwndFrom))
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
			if (auto renderer = Renderer::from_handle(hwnd))
				result = renderer->on_subclass_proc(&current_state);
			else
				result = orig_wnd_proc(&current_state);
			manager.set_current_state(old_state);
			return result;
		}
	} client;
}
