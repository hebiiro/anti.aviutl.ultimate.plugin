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

		virtual BOOL shift() override
		{
			return Executor().shift();
		}

		virtual BOOL move_to_begin() override
		{
			return Executor().move_to_begin();
		}

		virtual BOOL move_to_end() override
		{
			return Executor().move_to_end();
		}

		virtual BOOL move_begin() override
		{
			return Executor().move_begin();
		}

		virtual BOOL move_end() override
		{
			return Executor().move_end();
		}

		virtual BOOL stretch_begin() override
		{
			return Executor().stretch_begin();
		}

		virtual BOOL stretch_end() override
		{
			return Executor().stretch_end();
		}

		virtual BOOL delete_midpt() override
		{
			return Executor().delete_midpt();
		}

		virtual BOOL fix_bpm() override
		{
			return Executor().fix_bpm();
		}
	} app;
}
