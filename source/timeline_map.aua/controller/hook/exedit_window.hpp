#pragma once

namespace apn::timeline_map::controller::hook
{
	//
	// このクラスは拡張編集ウィンドウをフックします。
	//
	inline struct exedit_window_t : my::Window
	{
		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のモジュールハンドルを取得します。
			auto exedit = magi.exin.get_exedit();
			if (!exedit) return FALSE;

			// 拡張編集の内部関数をフックします。
			my::hook::detours detours;
			my::hook::attach(redraw_layers, magi.exin.address.function.redraw_layers);
			my::hook::attach(redraw_exedit_window, magi.exin.address.function.redraw_exedit_window);

			// 拡張編集ウィンドウをサブクラス化します。
			return subclass(magi.exin.get_exedit_window());
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return unsubclass();
		}

		//
		// このクラスは拡張編集内のredraw_layers()の呼び出しをフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(int32_t flags[])
			{
				view::overview.redraw();

				return orig_proc(flags);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} redraw_layers;

		//
		// このクラスは拡張編集内のredraw_exedit_window()の呼び出しをフックします。
		//
		inline static struct {
			inline static void CDECL hook_proc(HWND exedit_window)
			{
				view::overview.redraw();

				return orig_proc(exedit_window);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} redraw_exedit_window;

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM w_param, LPARAM l_param) override
		{
			switch (message)
			{
			case WM_MOUSEMOVE:
				{
//					MY_TRACE_FUNC("{/}, {/hex}, {/hex}", my::message_to_string(message), w_param, l_param);

					// タイムラインが操作対象の場合は
					if (model::property.loupe.flag_timeline)
					{
						// マウス座標を取得します。
						auto point = my::lp_to_pt(l_param);

						view::loupe.set_viewport(
							(int32_t)magi.exin.x_to_frame(point.x),
							(int32_t)magi.exin.y_to_layer(point.y));
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, w_param, l_param);
		}
	} exedit_window;
}
