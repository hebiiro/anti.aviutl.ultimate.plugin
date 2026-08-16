#pragma once

namespace apn::dark::kuro::paint::d2d
{
	//
	// このクラスはD2Dを使用した矩形の描画を担当します。
	//
	struct recter_t
	{
		//
		// 点pと直線abの距離を返します。
		//
		inline static float get_distance(const D2D1_POINT_2F& p, const D2D1_POINT_2F& a, const D2D1_POINT_2F& b)
		{
			auto v = b - a;

			if (auto length = get_length(v))
				return fabsf(cross_product(v, p - a)) / length;

			return {};
		}

		//
		// 指定されたベクトルと直角のベクトルを返します。
		//
		inline static D2D1_POINT_2F perpendicular(const D2D1_POINT_2F& v)
		{
			return { -v.y, v.x };
		}

		//
		// グラデーションストップ座標を返します。
		//
		inline static auto get_stop_point(const D2D1_RECT_F& rc, float radius)
		{
			radius = std::max(radius, 0.1f);

			// 矩形の左上座標を取得します。
			auto n = D2D1::Point2F(rc.left, rc.top);

			// 矩形の右下座標を取得します。
			auto f = D2D1::Point2F(rc.right, rc.bottom);

			// 矩形の中心座標を取得します。
			auto p = (n + f) / 2.0f;

			// 傾きの基準直線を取得します。
			auto a = D2D1::Point2F(rc.left, rc.bottom - radius);
			auto b = D2D1::Point2F(rc.right - radius, rc.top);

			// 中心座標からの距離を取得します。
			auto distance = get_distance(p, a, b);

			// 基準ベクトルを正規化します。
			auto v = normalize(b - a);

			// 基準ベクトルの長さを中心座標からの距離にします。
			v = v * distance;

			// 傾きを直角に変更します。
			v = perpendicular(v);

			// 中心座標から直角線方向にずらした座標の組を返します。
			return std::make_pair(p - v, p + v);
		}

		//
		// ソリッドブラシを作成して返します。
		//
		inline static ComPtr<ID2D1SolidColorBrush> create_solid_brush(
			ID2D1RenderTarget* render_target, const D2D1_COLOR_F& d2d_color)
		{
			// ソリッドブラシを作成します。
			ComPtr<ID2D1SolidColorBrush> solid_brush;
			render_target->CreateSolidColorBrush(d2d_color, &solid_brush);
			return solid_brush;
		}

		//
		// グラデーションストップコレクションを作成して返します。
		//
		inline static ComPtr<ID2D1GradientStopCollection> create_gradient_stop_collection(
			ID2D1RenderTarget* render_target,
			const D2D1_COLOR_F& start_color,
			const D2D1_COLOR_F& end_color)
		{
			D2D1_GRADIENT_STOP stops[2] = { { 0.0f, start_color }, { 1.0f, end_color } };

			ComPtr<ID2D1GradientStopCollection> gradient_stop_collection;
			render_target->CreateGradientStopCollection(stops, std::size(stops),
				D2D1_GAMMA_1_0, D2D1_EXTEND_MODE_CLAMP, &gradient_stop_collection);
			return gradient_stop_collection;
		}

		//
		// グラデーションブラシを作成して返します。
		//
		inline static ComPtr<ID2D1LinearGradientBrush> create_gradient_brush(
			ID2D1RenderTarget* render_target,
			const D2D1_POINT_2F& start_point,
			const D2D1_POINT_2F& end_point,
			ID2D1GradientStopCollection* gradient_stop_collection)
		{
			D2D1_LINEAR_GRADIENT_BRUSH_PROPERTIES props = { start_point, end_point };

			ComPtr<ID2D1LinearGradientBrush> gradient_brush;
			render_target->CreateLinearGradientBrush(
				props, gradient_stop_collection, &gradient_brush);
			return gradient_brush;
		}

		//
		// グラデーションブラシを作成して返します。
		//
		inline static ComPtr<ID2D1LinearGradientBrush> create_gradient_brush(
			ID2D1RenderTarget* render_target,
			const D2D1_COLOR_F& start_color,
			const D2D1_COLOR_F& end_color,
			const D2D1_POINT_2F& start_point,
			const D2D1_POINT_2F& end_point)
		{
			auto gradient_stop_collection = create_gradient_stop_collection(
				render_target, start_color, end_color);
			if (!gradient_stop_collection) return {};

			return create_gradient_brush(render_target,
				start_point, end_point, gradient_stop_collection.Get());
		}

		//
		// 描画に使用するプロパティです。
		//
		HDC dc = {};
		LPCRECT rc = {};
		const pigment_t* pigment = {};
		int iw = {}, ih = {};
		float w = {}, h = {};

		//
		// コンストラクタです。
		//
		recter_t(HDC dc, LPCRECT rc, const pigment_t* pigment)
			: dc(dc), rc(rc), pigment(pigment)
		{
			// 引数が無効の場合は失敗します。
			if (!rc || ::IsRectEmpty(rc)) return;

			// ピグメントが無効の場合は失敗します。
			if (!pigment->text.is_valid()) return;

			// 描画の準備に失敗した場合は失敗します。
			if (!core.prepare()) return;

			// 描画寸法を取得します。
			iw = my::get_width(*rc);
			ih = my::get_height(*rc);
			w = (float)iw;
			h = (float)ih;
		}

		//
		// 初期化が正常に完了している場合はTRUEを返します。
		//
		BOOL is_initialized() const
		{
			return iw > 0 && ih > 0;
		}

		//
		// 丸角矩形を描画します。
		//
		BOOL draw_round_rect(float radius)
		{
			MY_TRACE_FUNC("{/hex}, ({/}), {/}", dc, safe_string(rc), radius);

			// 初期化が正常に完了していない場合は何もしません。
			if (!is_initialized()) return 0;

			// レンダーターゲットとデバイスコンテキストをバインドします。
			core_t::binder_t binder(dc, rc);

			// レンダーターゲットを取得します。
			const auto& render_target = core.render_target;

			// 縁の幅を取得します。
			auto border_width = get_border_width_as_float();
			auto half_border_width = border_width / 2.0f;

			// 全体の矩形を取得します。
			auto whole_rc = D2D1::RectF(0, 0, w, h);

			// 縁がはみ出さないように収縮した矩形を取得します。
			auto draw_rc = deflate(whole_rc, half_border_width, half_border_width);

			// 丸角矩形を取得します。
			auto round_rc = D2D1::RoundedRect(draw_rc, radius, radius);

			// 背景が描画可能な場合は
			if (pigment->background.is_valid() && pigment->background.is_opaque())
			{
				// 背景を描画します。

				// 背景用のカラーエントリを取得します。
				const auto& color_entry = pigment->background.color_entry;

				// グラデーションで描画する場合は
				if (hive.gradient.flag_use)
				{
					// グラデーションブラシで背景を描画します。
					auto gradient_brush = create_gradient_brush(
						render_target.Get(),
						to_d2d_color(color_entry.parts[0]),
						get_background_end_color(color_entry),
						D2D1::Point2F(draw_rc.left, draw_rc.top),
						D2D1::Point2F(draw_rc.right, draw_rc.bottom));
					if (!gradient_brush) return FALSE;
					render_target->FillRoundedRectangle(round_rc, gradient_brush.Get());
				}
				// それ以外の場合は
				else
				{
					// ソリッドブラシで背景を描画します。
					auto solid_brush = create_solid_brush(
						render_target.Get(),
						to_d2d_color(color_entry.parts[0]));
					if (!solid_brush) return FALSE;
					render_target->FillRoundedRectangle(round_rc, solid_brush.Get());
				}
			}

			// 縁が描画可能な場合は
			if (pigment->border.is_valid() && pigment->border.is_opaque() && border_width > 0.0f)
			{
				// 縁を描画します。

				// 縁用のカラーエントリを取得します。
				const auto& color_entry = pigment->border.color_entry;

				// グラデーションストップ座標を取得します。
				auto stop_point = get_stop_point(whole_rc, radius);

				// 終了カラーを決定します。
				auto end_color_index = color_entry.parts[1].is_valid() ? 1 : 0;

				// ブレンド用のカラーエントリを取得します。
				auto color_entry_for_blend = get_3d_edge_color_entry();

				// グラデーションブラシで縁を描画します。
				auto gradient_brush = create_gradient_brush(
					render_target.Get(),
					blend(color_entry.parts[0], color_entry_for_blend, 0),
					blend(color_entry.parts[end_color_index], color_entry_for_blend, 1),
					stop_point.first, stop_point.second);
				if (!gradient_brush) return FALSE;
				render_target->DrawRoundedRectangle(round_rc, gradient_brush.Get(), border_width);
			}

			return TRUE;
		}
	};
}
