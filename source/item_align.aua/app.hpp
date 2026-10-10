#pragma once

namespace apn::item_align
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct app_t : app_interface_t
	{
		virtual void align_left() override { return align_left_t().execute(); }
		virtual void align_right() override { return align_right_t().execute(); }
		virtual void stretch_left() override { return stretch_left_t().execute(); }
		virtual void stretch_right() override { return stretch_right_t().execute(); }
		virtual void relative_space_left() override { return relative_space_left_t().execute(); }
		virtual void relative_space_right() override { return relative_space_right_t().execute(); }
		virtual void absolute_space_left() override { return absolute_space_left_t().execute(); }
		virtual void absolute_space_right() override { return absolute_space_right_t().execute(); }
		virtual void shift_down() override { return shift_down_t().execute(); }
		virtual void shift_up() override { return shift_up_t().execute(); }
		virtual void move_vert_down() override { move_vert_down_t().execute(); }
		virtual void move_vert_up() override { move_vert_up_t().execute(); }
		virtual void fix_bpm() override { return fix_bpm_t().execute(); }
		virtual void erase_midpt() override { return erase_midpt_t().execute(); }
	} app_impl;
}
