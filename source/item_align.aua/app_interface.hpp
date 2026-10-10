#pragma once

namespace apn::item_align
{
	//
	// このクラスはアプリケーションのインターフェイスです。
	//
	inline struct app_interface_t {
		//
		// コンストラクタです。
		//
		app_interface_t() { app = this; }

		virtual void align_left() = 0;
		virtual void align_right() = 0;
		virtual void stretch_left() = 0;
		virtual void stretch_right() = 0;
		virtual void relative_space_left() = 0;
		virtual void relative_space_right() = 0;
		virtual void absolute_space_left() = 0;
		virtual void absolute_space_right() = 0;
		virtual void shift_up() = 0;
		virtual void shift_down() = 0;
		virtual void move_vert_down() = 0;
		virtual void move_vert_up() = 0;
		virtual void fix_bpm() = 0;
		virtual void erase_midpt() = 0;
	} *app = nullptr;
}
