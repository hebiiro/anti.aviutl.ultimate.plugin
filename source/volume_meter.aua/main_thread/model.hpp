#pragma once

namespace apn::volume_meter::main_thread
{
	//
	// このクラスはメインスレッドのモデルです。
	//
	inline struct model_t
	{
		//
		// 音量メーターの描画設定です。
		//
		paint_option_t paint_option = {
			.background_color = RGB(0, 0, 0),
			.warning_color = RGB(200, 0, 0),
			.level_color = RGB(0, 128, 255),
			.level_width = 4,
			.min_db = -60,
			.max_db = 0,
			.gamma = 200,
			.nb_separators = 10,
			.separator_width = 1,
			.text_area_height = 24,
			.zones = {
				{ -6, RGB(200, 32, 32), }, // レッドゾーンです。
				{ -12, RGB(192, 168, 0), }, // イエローゾーンです。
				{ -1000, RGB(0, 160, 0), }, // グリーンゾーンです。
			},
			.fonts = {
				{ L"Consolas", 5, 18, RGB(255, 255, 255), RGB(0, 0, 0), { +0, +0 }, { +1, +1 }, FALSE, }, // トータルのフォントです。
				{ L"Consolas", 0, 12, RGB(200, 200, 200), RGB(0, 0, 0), { -2, +0 }, { +1, +1 }, FALSE, }, // ゾーンのフォントです。
				{ L"Consolas", 0, 14, RGB(255, 255, 255), RGB(0, 0, 0), { +0, +0 }, { -1, +1 }, TRUE, }, // ピークのフォントです。
				{ L"Consolas", 0, 14, RGB(255, 255, 255), RGB(0, 0, 0), { +0, +0 }, { -1, +1 }, TRUE, }, // レベルのフォントです。
			},
		};
	} model;
}
