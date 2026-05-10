#pragma once

namespace apn::timeline_map::model
{
	//
	// このクラスは描画対象です。
	//
	struct render_target_t
	{
		//
		// リセット回数です。
		//
		UINT nb_resets = 0;

		ComPtr<IDXGISwapChain1> dxgi_swap_chain;
		ComPtr<ID2D1Bitmap1> d2d_target_bitmap;
	};
}
