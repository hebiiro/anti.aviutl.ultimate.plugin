#pragma once

namespace apn::dark::kuro::paint
{
	//
	// このクラスはツリービューのマテリアルです。
	//
	inline struct treeview_material_t : material_t
	{
		//
		// マテリアルの初期化処理を実行します。
		//
		virtual void on_init_material() override
		{
			MY_TRACE_FUNC("");

			auto separator = create_pigment(L"TreeView", L"Separator",
				style_t::color_e::Border,
				{},
				{});

			auto item_normal = create_pigment(L"TreeView", L"Normal",
				style_t::color_e::Background,
				{},
				style_t::color_e::Text);

			auto item_disabled = create_pigment(L"TreeView", L"Disable",
				style_t::color_e::Background,
				{},
				style_t::color_e::TextDisable);

			auto item_hot = create_pigment(L"TreeView", L"Hot",
				style_t::color_e::ButtonBodyHover,
				{},
				style_t::color_e::Text);

			auto item_selected = create_pigment(L"TreeView", L"Select",
				style_t::color_e::ButtonBodySelect,
				{},
				style_t::color_e::Text);

			auto glyph_normal = create_pigment(L"TreeViewGlyph", L"Normal", item_normal);
			auto glyph_hot = create_pigment(L"TreeViewGlyph", L"Hot", item_hot);

			palette.set(TVP_TREEITEM, 0, separator);
			palette.set(TVP_TREEITEM, TREIS_NORMAL, item_normal);
			palette.set(TVP_TREEITEM, TREIS_DISABLED, item_disabled);
			palette.set(TVP_TREEITEM, TREIS_HOT, item_hot);
			palette.set(TVP_TREEITEM, TREIS_SELECTED, item_selected);
			palette.set(TVP_TREEITEM, TREIS_SELECTEDNOTFOCUS, item_selected);
			palette.set(TVP_TREEITEM, TREIS_HOTSELECTED, item_selected);

			palette.set(TVP_GLYPH, GLPS_CLOSED, glyph_normal);
			palette.set(TVP_GLYPH, GLPS_OPENED, glyph_normal);
			palette.set(TVP_HOTGLYPH, GLPS_CLOSED, glyph_hot);
			palette.set(TVP_HOTGLYPH, GLPS_OPENED, glyph_hot);
		}

		//
		// マテリアルの後始末処理を実行します。
		//
		virtual void on_exit_material() override
		{
			MY_TRACE_FUNC("");
		}
	} treeview_material;
}
