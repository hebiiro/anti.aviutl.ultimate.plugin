#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはリストビューのマテリアルです。
	//
	inline struct listview_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto separator = create_pigment(L"ListView", L"Separator",
				style_t::color_e::Border,
				{},
				{});

			auto item_normal = create_pigment(L"ListView", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto item_disabled = create_pigment(L"ListView", L"Disable",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			auto item_hot = create_pigment(L"ListView", L"Hot",
				style_t::color_e::ButtonBodyHover,
				{},
				style_t::color_e::Text);

			auto item_selected = create_pigment(L"ListView", L"Select",
				style_t::color_e::ButtonBodySelect,
				{},
				style_t::color_e::Text);

			palette.set(LVP_LISTITEM, 0, separator);
			palette.set(LVP_LISTITEM, LISS_NORMAL, item_normal);
			palette.set(LVP_LISTITEM, LISS_DISABLED, item_disabled);
			palette.set(LVP_LISTITEM, LISS_HOT, item_hot);
			palette.set(LVP_LISTITEM, LISS_SELECTED, item_selected);
			palette.set(LVP_LISTITEM, LISS_SELECTEDNOTFOCUS, item_selected);
			palette.set(LVP_LISTITEM, LISS_HOTSELECTED, item_selected);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} listview_material;
}
