#pragma once

namespace apn::item_align
{
	//
	// このクラスはHive::Appの実装です。
	//
	inline struct App : Hive::App
	{
		//
		// コンストラクタです。
		//
		App()
		{
			hive.app = this;
		}

		virtual void align_left() override { return AlignLeft().execute(); }
		virtual void align_right() override { return AlignRight().execute(); }
		virtual void stretch_left() override { return StretchLeft().execute(); }
		virtual void stretch_right() override { return StretchRight().execute(); }
		virtual void relative_space_left() override { return RelativeSpaceLeft().execute(); }
		virtual void relative_space_right() override { return RelativeSpaceRight().execute(); }
		virtual void absolute_space_left() override { return AbsoluteSpaceLeft().execute(); }
		virtual void absolute_space_right() override { return AbsoluteSpaceRight().execute(); }
		virtual void shift_down() override { return ShiftDown().execute(); }
		virtual void shift_up() override { return ShiftUp().execute(); }
		virtual void move_vert_down() override { MoveVertDown().execute(); }
		virtual void move_vert_up() override { MoveVertUp().execute(); }
		virtual void fix_bpm() override { return FixBpm().execute(); }
		virtual void erase_midpt() override { return EraseMidpt().execute(); }
	} app;
}
