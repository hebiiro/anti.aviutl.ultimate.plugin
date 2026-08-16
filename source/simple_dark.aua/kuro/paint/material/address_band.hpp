#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはアドレスバンドのマテリアルです。
	//
	inline struct address_band_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto background = create_pigment(L"AddressBand", L"Background",
				style_t::color_e::Background,
				style_t::color_e::Border,
				style_t::color_e::Text);

			palette.set(0, 0, background);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} address_band_material;
}
