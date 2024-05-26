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

		virtual BOOL shift_selection() override
		{
			return Executor().shift_selection();
		}

		virtual BOOL move_current_frame_to_start() override
		{
			return Executor().move_current_frame_to_start();
		}

		virtual BOOL move_current_frame_to_end() override
		{
			return Executor().move_current_frame_to_end();
		}

		virtual BOOL move_selection_start() override
		{
			return Executor().move_selection_start();
		}

		virtual BOOL move_selection_end() override
		{
			return Executor().move_selection_end();
		}

		virtual BOOL stretch_selection_start() override
		{
			return Executor().stretch_selection_start();
		}

		virtual BOOL stretch_selection_end() override
		{
			return Executor().stretch_selection_end();
		}

		virtual BOOL delete_midpt() override
		{
			return Executor().delete_midpt();
		}

		virtual BOOL fix_selection_bpm() override
		{
			return Executor().fix_selection_bpm();
		}
	} app;
}
