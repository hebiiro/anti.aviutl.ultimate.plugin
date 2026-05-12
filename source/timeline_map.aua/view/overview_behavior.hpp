#pragma once

namespace apn::timeline_map::view
{
	//
	// このクラスはビュー層の全体図の振る舞いです。
	//
	struct overview_behavior_t : model::render_target_t
	{
		//
		// D2Dコマンドリストです。
		//
		ComPtr<ID2D1CommandList> d2d_command_list;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// 描画対象に追加します。
			model::state.register_render_target(this);

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// 描画対象から除外します。
			model::state.unregister_render_target(this);

			return TRUE;
		}

		//
		// D2Dコマンドリストをリセットします。
		//
		BOOL reset_command_list()
		{
			MY_TRACE_FUNC("");

			// D2Dコマンドリストをリセットします。
			d2d_command_list = nullptr;

			return TRUE;
		}

		//
		// この仮想関数はリソースを作成する必要があるときに呼び出されます。
		//
		virtual BOOL create_resources() override
		{
			if (!__super::create_resources()) return FALSE;

			return TRUE;
		}

		//
		// この仮想関数はリソースをリセットする必要があるときに呼び出されます。
		//
		virtual BOOL reset_resources() override
		{
			// D2Dコマンドリストをリセットします。
			reset_command_list();

			return __super::reset_resources();
		}

		//
		// 全体図を描画します。
		//
		BOOL on_paint()
		{
			// コンテキストを作成します。
			model::context_t context(this);

			// コンテキストを初期化できなかった場合は何もしません。
			if (!context.is_initialized()) return FALSE;

			// コマンドリストが無効の場合は
			if (!d2d_command_list)
			{
				// コマンドリストを作成します。
				model::dx.d2d_device_context->CreateCommandList(&d2d_command_list);

				// 描画処理を開始します。
				model::state.begin_draw(d2d_command_list.Get());

				// 各要素を描画します。
				context.draw_layers();
				context.draw_items();
				context.draw_layer_settings();

				// コマンドリストを閉じます。
				d2d_command_list->Close();

				// 描画処理を終了します。
				model::state.end_draw(nullptr);
			}

			// 描画処理を開始します。
			model::state.begin_draw(d2d_target_bitmap.Get());

			// コマンドリストを描画します。
			model::dx.d2d_device_context->DrawImage(d2d_command_list.Get());

			// 各要素を描画します。
			context.draw_current_frame();
			context.draw_visible_area();

			// 描画処理を終了します。
			model::state.end_draw(dxgi_swap_chain.Get());

			return TRUE;
		}

		//
		// 全体図をリサイズします。
		//
		BOOL on_size()
		{
			// レンダーターゲットをリサイズします。
			model::render_target_t::resize();

			// コマンドリストをリセットします。
			reset_command_list();

			return TRUE;
		}

		//
		// ユーザーがウィンドウをクリック(またはドラッグ)したときの処理です。
		//
		BOOL on_click(POINT point)
		{
			// コンテキストを作成します。
			model::context_t context(this);

			// コンテキストを初期化できなかった場合は何もしません。
			if (!context.is_initialized()) return FALSE;

			// タイムラインを水平方向にスクロールします。
			{
				auto left_frame = magi.exin.get_top_visible_frame();
				auto right_frame = (int32_t)magi.exin.x_to_frame(magi.exin.get_layer_width());

				auto frame = context.pixel_to_frame(point.x);
				frame -= (right_frame - left_frame) / 2;
				magi.exin.set_top_visible_frame(frame);
			}

			// タイムラインを垂直方向にスクロールします。
			{
				auto layer = context.pixel_to_layer(point.y);
				layer -= (magi.exin.get_layer_visible_count() / 2);
				magi.exin.set_top_visible_layer(layer);
			}

			return TRUE;
		}
	};
}
