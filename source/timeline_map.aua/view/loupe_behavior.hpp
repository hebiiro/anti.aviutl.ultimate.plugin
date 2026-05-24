#pragma once

namespace apn::timeline_map::view
{
	//
	// このクラスはビュー層の拡大鏡の振る舞いです。
	//
	struct loupe_behavior_t : model::render_target_t
	{
		//
		// このクラスはドラッグ開始時のデータです。
		//
		struct drag_start_t {
			//
			// マウス座標です。
			//
			POINT point = {};

			//
			// フレーム数です。
			//
			int32_t nb_frames = {};

			//
			// レイヤー数です。
			//
			int32_t nb_layers = {};
		} drag_start;

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
		// コンテキストを作成して返します。
		//
		model::context_t create_context()
		{
			// クライアント矩形を取得します。
			auto rc = my::get_client_rect(get_hwnd());

			// ビューポートへの参照を取得します。
			auto& viewport = model::property.loupe.viewport;

			// 描画領域の開始位置を算出します。
			auto frame_begin = std::max(viewport.frame - viewport.nb_frames / 2, 0);
			auto layer_begin = std::max(viewport.layer - viewport.nb_layers / 2, 0);

			// コンテキストを作成して返します。
			return model::context_t(rc,
				frame_begin, frame_begin + viewport.nb_frames,
				layer_begin, layer_begin + viewport.nb_layers);
		}

		//
		// 拡大鏡を描画します。
		//
		BOOL on_paint()
		{
			// コンテキストを作成します。
			auto context = create_context();

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
//			context.draw_visible_area();

			// 描画処理を終了します。
			model::state.end_draw(dxgi_swap_chain.Get());

			return TRUE;
		}

		//
		// 拡大鏡をリサイズします。
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
		// ユーザーがウィンドウでドラッグを開始したときの処理です。
		//
		BOOL on_begin_drag(POINT point)
		{
			MY_TRACE_FUNC("");

			// ビューポートへの参照を取得します。
			auto& viewport = model::property.loupe.viewport;

			// ドラッグ開始データをセットします。
			drag_start.point = point;
			drag_start.nb_frames = viewport.nb_frames;
			drag_start.nb_layers = viewport.nb_layers;

			return TRUE;
		}

		//
		// ユーザーがウィンドウでドラッグを終了したときの処理です。
		//
		BOOL on_end_drag(POINT point)
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// ユーザーがウィンドウでドラッグしたときの処理です。
		//
		BOOL on_drag(POINT point)
		{
			MY_TRACE_FUNC("{/}, {/}", point.x, point.y);

			// ドラッグオフセットを算出します。
			auto offset = point - drag_start.point;
			MY_TRACE_POINT(offset);

			// ビューポートへの参照を取得します。
			auto& viewport = model::property.loupe.viewport;

			// ビューポートにオフセットを適用します。
			viewport.nb_frames = std::max(drag_start.nb_frames + (int32_t)offset.x * 10, 1);
			viewport.nb_layers = std::clamp(drag_start.nb_layers + (int32_t)offset.y / 20, 1, 100);
			MY_TRACE_INT(viewport.nb_frames);
			MY_TRACE_INT(viewport.nb_layers);

			return TRUE;
		}
	};
}
