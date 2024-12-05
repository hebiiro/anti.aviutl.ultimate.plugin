#pragma once

namespace apn::reboot
{
	//
	// 指定された名前のサブメニューを検索して返します。
	//
	inline HMENU find_menu(HMENU menu, LPCTSTR name)
	{
		auto c = ::GetMenuItemCount(menu);
		for (auto i = 0; i < c; i++)
		{
			if (my::get_menu_item_text(menu, i, MF_BYPOSITION) == name)
				return ::GetSubMenu(menu, i);
		}

		return nullptr;
	}

	//
	// 指定された名前の項目を検索してそのIDを返します。
	//
	inline uint32_t find_menu_item(HMENU menu, LPCTSTR name)
	{
		auto c = ::GetMenuItemCount(menu);
		for (auto i = 0; i < c; i++)
		{
			if (my::get_menu_item_text(menu, i, MF_BYPOSITION) == name)
				return ::GetMenuItemID(menu, i);
		}

		return 0;
	}
}

namespace apn::reboot::project
{
	//
	// プロジェクトを編集中の場合はTRUEを返します。
	//
	inline BOOL is_editing()
	{
		AviUtl::SysInfo si = {};
		magi.fp->exfunc->get_sys_info(magi.exin.get_editp(), &si);
		return !!(si.flag & AviUtl::SysInfo::Flag::Edit);
	}

	//
	// プロジェクトを上書き保存します。
	//
	inline BOOL write()
	{
		// プロジェクトを開いていない場合は何もしません。
		if (!is_editing()) return FALSE;

		// プロジェクトを上書き保存します。
		::SendMessage(hive.aviutl_window, WM_COMMAND, 1024, 0);

		return TRUE;
	}

	//
	// プロジェクトを名前を付けて保存します。
	//
	inline BOOL write_as()
	{
		// プロジェクトを開いていない場合は何もしません。
		if (!is_editing()) return FALSE;

		// プロジェクトを名前を付けて保存します。
		::SendMessage(hive.aviutl_window, WM_COMMAND, 1023, 0);

		return TRUE;
	}

	//
	// プロジェクトを開きます。
	//
	inline BOOL open()
	{
		// プロジェクトを開きます。
		::SendMessage(hive.aviutl_window, WM_COMMAND, 5118, 0);

		return TRUE;
	}

	//
	// プロジェクトを閉じます。
	//
	inline BOOL close()
	{
		// プロジェクトを開いていない場合は何もしません。
		if (!is_editing()) return FALSE;

		// プロジェクトを閉じます。
		::SendMessage(hive.aviutl_window, WM_COMMAND, 5157, 0);

		return TRUE;
	}

	//
	// プロジェクトを再生します。
	//
	inline BOOL play()
	{
		// プロジェクトを開いていない場合は何もしません。
		if (!is_editing()) return FALSE;

		// プロジェクトを再生します。
		::SendMessage(hive.aviutl_window, WM_COMMAND, 1099, 0);

		return TRUE;
	}
}
