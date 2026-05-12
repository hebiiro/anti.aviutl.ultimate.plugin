#pragma once

namespace apn::timeline_map::model
{
	//
	// このクラスは描画対象です。
	//
	struct render_target_t
	{
		//
		// DXGIスワップチェーンです。
		//
		ComPtr<IDXGISwapChain1> dxgi_swap_chain;

		//
		// D2Dビットマップです。
		//
		ComPtr<ID2D1Bitmap1> d2d_target_bitmap;

		//
		// この仮想関数はウィンドウハンドルが必要なときに呼び出されます。
		//
		virtual HWND get_hwnd() const = 0;

		//
		// この仮想関数はリソースを作成する必要があるときに呼び出されます。
		//
		virtual BOOL create_resources()
		{
			// DXGIデバイスが無効の場合は何もしません。
			if (!dx.dxgi_device) return FALSE;

			// D2Dデバイスコンテキストが無効の場合は何もしません。
			if (!dx.d2d_device_context) return FALSE;

			// D2Dビットマップが作成済みの場合は
			if (d2d_target_bitmap)
			{
				// レンダーターゲットが既に作成済みなので
				// 何もせずにTRUEを返します。
				return TRUE;
			}

			// レンダーターゲットのリソースをリセットします。
			dxgi_swap_chain = nullptr;
			d2d_target_bitmap = nullptr;

			// DXGIファクトリを取得します。
			ComPtr<IDXGIFactory2> dxgi_factory;
			{
				ComPtr<IDXGIAdapter> dxgi_adapter;
				dx.dxgi_device->GetAdapter(&dxgi_adapter);
				if (!dxgi_adapter) return FALSE;

				dxgi_adapter->GetParent(IID_PPV_ARGS(&dxgi_factory));
				if (!dxgi_factory) return FALSE;
			}

			// DXGIスワップチェーンを作成します。
			{
				DXGI_SWAP_CHAIN_DESC1 desc = {};
				desc.Width = 0;
				desc.Height = 0;
				desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
				desc.SampleDesc.Count = 1;
				desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
				desc.BufferCount = 1;
				desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
				desc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;

				dxgi_factory->CreateSwapChainForHwnd(
					dx.d3d_device.Get(),
					get_hwnd(),
					&desc,
					nullptr,
					nullptr,
					&dxgi_swap_chain);
				if (!dxgi_swap_chain) return FALSE;
			}

			// バックバッファを作成します。
			return create_back_buffer();
		}

		//
		// この仮想関数はリソースをリセットする必要があるときに呼び出されます。
		//
		virtual BOOL reset_resources()
		{
			dxgi_swap_chain = nullptr;
			d2d_target_bitmap = nullptr;

			return TRUE;
		}

		//
		// レンダーターゲットをリサイズします。
		//
		virtual BOOL resize()
		{
			// DXGIスワップチェーンが無効の場合は何もしません。
			if (!dxgi_swap_chain) return FALSE;

			// D2Dデバイスコンテキストが無効の場合は何もしません。
			if (!dx.d2d_device_context) return FALSE;

			// D2Dデバイスコンテキストのターゲットをリセットします。
			dx.d2d_device_context->SetTarget(nullptr);

			// 既存のバックバッファを開放します。
			d2d_target_bitmap = nullptr;

			// DXGIスワップチェーンをリサイズします。
			auto hr = dxgi_swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
			if (FAILED(hr)) return FALSE;

			// バックバッファを作成します。
			return create_back_buffer();
		}

		//
		// レンダーターゲットのバックバッファを作成します。
		// 内部的に使用されます。
		//
		BOOL create_back_buffer()
		{
			// バックバッファを取得します。
			ComPtr<IDXGISurface> dxgi_surface;
			{
				dxgi_swap_chain->GetBuffer(0, IID_PPV_ARGS(&dxgi_surface));
				if (!dxgi_surface) return FALSE;
			}

			// D2Dビットマップを作成します。
			{
				auto bitmap_props =
					D2D1::BitmapProperties1(
						D2D1_BITMAP_OPTIONS_TARGET |
						D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
						D2D1::PixelFormat(
							DXGI_FORMAT_B8G8R8A8_UNORM,
							D2D1_ALPHA_MODE_PREMULTIPLIED));

				dx.d2d_device_context->CreateBitmapFromDxgiSurface(
					dxgi_surface.Get(),
					&bitmap_props,
					&d2d_target_bitmap);
				if (!d2d_target_bitmap) return FALSE;
			}

			return TRUE;
		}
	};
}
