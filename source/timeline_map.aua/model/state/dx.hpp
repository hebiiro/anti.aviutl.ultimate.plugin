#pragma once

namespace apn::timeline_map::model
{
	//
	// このクラスはDirectXのリソースです。
	//
	inline struct dx_t
	{
		ComPtr<ID3D11Device> d3d_device;
		ComPtr<ID3D11DeviceContext> d3d_device_context;

		ComPtr<IDXGIDevice> dxgi_device;

		ComPtr<ID2D1Factory1> d2d_factory;
		ComPtr<ID2D1Device> d2d_device;
		ComPtr<ID2D1DeviceContext> d2d_device_context;

		ComPtr<IDWriteFactory> dw_factory;

		//
		// このクラスはテキスト用の描画リソースです。
		//
		struct text_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> shadow_brush;
		} text;

		//
		// このクラスはレイヤー用の描画リソースです。
		//
		struct layer_t {
			ComPtr<ID2D1SolidColorBrush> odd_brush;
			ComPtr<ID2D1SolidColorBrush> even_brush;
			ComPtr<ID2D1SolidColorBrush> undisp_brush;
			ComPtr<ID2D1SolidColorBrush> undisp_stroke_brush;
			ComPtr<ID2D1SolidColorBrush> locked_stroke_brush;
			ComPtr<ID2D1SolidColorBrush> coordlink_stroke_brush;
			ComPtr<ID2D1SolidColorBrush> clip_stroke_brush;
		} layer;

		//
		// このクラスはアイテム用の描画リソースです。
		//
		struct item_t {
			inline static constexpr auto c_gradient_w = 256;
			inline static constexpr auto c_gradient_h = 1;

			ComPtr<ID2D1SolidColorBrush> stroke_brush;
			struct node_t {
				ComPtr<ID2D1Bitmap> gradient_bitmap;
				ComPtr<ID2D1BitmapBrush> gradient_brush;
			} nodes[property.item.c_item.c_max_size];
		} item;

		//
		// このクラスは中間点用の描画リソースです。
		//
		struct midpt_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> line_brush;
		} midpt;

		//
		// このクラスは現在フレーム用の描画リソースです。
		//
		struct current_frame_t {
			ComPtr<ID2D1SolidColorBrush> brush;
		} current_frame;

		//
		// このクラスは表示領域用の描画リソースです。
		//
		struct visible_area_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> stroke_brush;
		} visible_area;

		//
		// このクラスは制御範囲用の描画リソースです。
		//
		struct control_range_t {
			ComPtr<ID2D1SolidColorBrush> brush;
			ComPtr<ID2D1SolidColorBrush> stroke_brush;
		} control_range;
	} dx;
}
