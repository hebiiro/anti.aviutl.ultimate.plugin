#pragma once

namespace apn::dark::skin
{
	//
	// このクラスはスキンのコンフィグを保持します。
	//
	inline struct Config
	{
		struct Edges {
			struct Edge {
				COLORREF near_color = CLR_NONE;
				COLORREF far_color = CLR_NONE;
			};
			Edge light_raised;
			Edge light_sunken;
			struct { Edge inner, outer; } raised;
			struct { Edge inner, outer; } sunken;
			struct { Edge inner, outer; } bump;
			struct { Edge inner, outer; } etched;
		} edges;

		struct ExEdit {
			struct Group {
				COLORREF color = CLR_NONE;
				float alpha = 0.0f;
			} group;
			struct XorPen {
				int32_t style = -1;
				int32_t width = -1;
				COLORREF color = CLR_NONE;
			} xor_pen;
		} exedit;

		//
		// スキンのコンフィグをリセットします。
		//
		void clear()
		{
			edges = Edges {};
			exedit = ExEdit {};
		}
	} config;
}
