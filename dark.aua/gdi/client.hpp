#pragma once

namespace apn::dark::gdi
{
	//
	// このクラスはGDIレンダラの利用者です。
	//
	inline struct Client
	{
		inline static constexpr LPCTSTR c_prop_name = _T("apn::dark::gdi::client");

		using Generator = std::shared_ptr<Renderer> (*)();
		std::unordered_map<my::tstring, Generator> generators;

		//
		// コンストラクタです。
		//
		Client()
		{
			generators[normalize(_T("#32770"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<DialogRenderer>(); };
			generators[normalize(WC_STATIC)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<StaticRenderer>(); };
			generators[normalize(WC_BUTTON)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ButtonRenderer>(); };
			generators[normalize(WC_EDIT)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<EditBoxRenderer>(); };
			generators[normalize(WC_LISTBOX)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ListBoxRenderer>(); };
			generators[normalize(_T("ListviewPopup"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ListBoxRenderer>(); };
			generators[normalize(_T("ComboLBox"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ListBoxRenderer>(); };
			generators[normalize(WC_COMBOBOX)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ComboBoxRenderer>(); };
//			generators[normalize(WC_COMBOBOXEX)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ListBoxRenderer>(); };
			generators[normalize(WC_COMBOBOXEX)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ComboBoxExRenderer>(); };
			generators[normalize(TOOLTIPS_CLASS)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ToolTipRenderer>(); };
			generators[normalize(TRACKBAR_CLASS)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<TrackBarRenderer>(); };
			generators[normalize(UPDOWN_CLASS)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<SpinRenderer>(); };
			generators[normalize(WC_HEADER)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<HeaderRenderer>(); };
			generators[normalize(WC_LISTVIEW)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ListViewRenderer>(); };
			generators[normalize(_T("DirectUIHWND"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ListViewRenderer>(); };
			generators[normalize(WC_TREEVIEW)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<TreeViewRenderer>(); };
			generators[normalize(TOOLBARCLASSNAME)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<ToolBarRenderer>(); };
			generators[normalize(WC_TABCONTROL)] = []() -> std::shared_ptr<Renderer> { return std::make_shared<TabRenderer>(); };
			generators[normalize(_T("AviUtl"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<AviUtlRenderer>(); };
			generators[normalize(_T("AviUtlButton"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<AviUtlButtonRenderer>(); };
			generators[normalize(_T("ExtendedFilterClass"))] = []() -> std::shared_ptr<Renderer> { return std::make_shared<SettingDialogRenderer>(); };
		}

		//
		// 与えられたクラス名をノーマライズして返します。
		//
		inline static my::tstring normalize(const my::tstring& class_name)
		{
			return my::to_upper(class_name);
		}

		//
		// GDIレンダラをウィンドウにアタッチします。
		//
		void attach_renderer(HWND hwnd, const my::tstring& class_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}", hwnd, class_name);

			auto it = generators.find(class_name);
			if (it == generators.end())
			{
				if (my::contains(class_name, normalize(_T("WindowsForms10.") TRACKBAR_CLASS)))
					it = generators.find(normalize(TRACKBAR_CLASS));

				if (it == generators.end())
					return;
			}
			Renderer::attach(hwnd, (*it->second)());
		}

		//
		// ウィンドウを微調整します。
		//
		void tweak_window(HWND hwnd, const my::tstring& class_name)
		{
			MY_TRACE_FUNC("{:#010x}, {}", hwnd, class_name);

			if (my::get_style(hwnd) & WS_CAPTION)
			{
				if (!hive.renderer_locked)
					skin::dwm.set_window_attribute(hwnd, FALSE);
			}
			else if (class_name == normalize(WC_BUTTON))
			{
				if (my::get_ex_style(hwnd) & WS_EX_STATICEDGE)
					hive.static_edge_buttons.emplace_back(hwnd);
			}
			else if (class_name == normalize(WC_STATIC))
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
			auto class_name = normalize(my::get_class_name(hwnd));
			MY_TRACE_STR(class_name);

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

				client.init_renderer(hwnd);
			}
			else if (message == WM_NCDESTROY && ::GetProp(hwnd, c_prop_name))
			{
				client.exit_renderer(hwnd);
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
