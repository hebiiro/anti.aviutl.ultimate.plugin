////////////////////////////////////////////////////////////////////////////////
// auls_confirmclose.cpp
// 編集内容が変更されている時のみ終了確認するようにするプラグイン
// -----------------------------------------------------------------------------
// 補足情報
// LoadLibrary で自分自身の参照カウントを増やしておかないと
// 終了時にプロセスが長時間、あるいはずっと残ってしまう。
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace apn::dirty_check
{
	inline constexpr auto FILTER_NAME = "終了確認";
	inline constexpr auto FILTER_INFORMATION = "Auls終了確認 v1.2 forked by karoterra";

	inline constexpr auto EXEDIT_NAME = "拡張編集";
	inline constexpr auto EXEDIT_92 = "拡張編集(exedit) version 0.92 by ＫＥＮくん";

	inline struct DirtyFlag
	{
		const int* g_undo_id_ptr = nullptr;
		int  g_undo_id_prev = 0;
		bool g_dirty_flag = false;

		//
		// 拡張編集のフィルタを返します。
		//
		inline static AviUtl::FilterPlugin* get_exedit(AviUtl::FilterPlugin* fp)
		{
			AviUtl::SysInfo si = {};
			fp->exfunc->get_sys_info(nullptr, &si);

			for (decltype(si.filter_n) i = 0; i < si.filter_n; i++)
			{
				auto fp1 = static_cast<AviUtl::FilterPlugin*>(fp->exfunc->get_filterp(i));

				if (::lstrcmpA(fp1->name, EXEDIT_NAME) == 0 &&
					::lstrcmpA(fp1->information, EXEDIT_92) == 0)
				{
					return fp1;
				}
			}
			return nullptr;
		}

		//
		// 初期化処理を行います。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// 拡張編集を取得します。
			if (auto exedit = get_exedit(fp))
			{
				// 拡張編集のアンドゥIDへのポインタを取得します。
				g_undo_id_ptr = reinterpret_cast<int*>(reinterpret_cast<size_t>(exedit->dll_hinst) + 0x244E08 + 12);

				return TRUE;
			}
			else
			{
				::MessageBoxA(fp->hwnd, "対応する拡張編集が見つかりません。", FILTER_NAME, MB_OK | MB_ICONERROR);

				return FALSE;
			}
		}

		//
		// 指定されたwpがトリガーならtrueを返します。
		//
		bool is_trigger_wp(WPARAM wp)
		{
			return wp == 5157	// 閉じる
				|| wp == 5097	// 開く
				|| wp == 5118	// 編集プロジェクトを開く
				|| (5596 <= wp && wp <= 5603)	// 最近使ったファイル1-8
				;
		}

		//
		// 指定されたウィンドウメッセージがトリガーならtrueを返します。
		//
		bool is_trigger_message(UINT msg, WPARAM wp)
		{
			if (!get()) return false;
			if (msg == WM_CLOSE) return true;
			if (msg == WM_COMMAND && is_trigger_wp(wp)) return true;
			return false;
		}

		//
		// ダーティーフラグを返します。
		//
		bool get()
		{
			return g_dirty_flag;
		}

		//
		// ダーティーフラグを更新します。
		//
		void update()
		{
			// 現在のアンドゥIDと前回のアンドゥIDが異なる場合は
			if (!g_dirty_flag && *g_undo_id_ptr != g_undo_id_prev)
				g_dirty_flag = true; // ダーティーフラグをセットします。
		}

		//
		// ダーティーフラグを初期値に戻します。
		//
		void clear()
		{
			g_undo_id_prev = *g_undo_id_ptr;
			g_dirty_flag = false;
		}
	} dirty_flag;

	inline struct AviUtlWindow : my::Window
	{
		//
		// 初期化を行います。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// AviUtlウィンドウをサブクラス化します。
			return subclass(fp->hwnd_parent);
		}

		//
		// AviUtlウィンドウのウィンドウプロシージャをフックします。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			// 終了確認のトリガーかチェックします。
			if (dirty_flag.is_trigger_message(message, wParam))
			{
				// メッセージボックスを表示します。
				auto id = ::MessageBoxA(hwnd, "変更された編集データがあります。保存しますか？",
					FILTER_NAME, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);

				// ユーザーが「はい」を選択した場合は
				if (id == IDYES)
				{
					// 編集プロジェクトの上書き保存を試みます。
					::SendMessage(hwnd, WM_COMMAND, 1024, 0);

					// 保存がキャンセルされた場合は
					if (dirty_flag.get())
						return 0; // デフォルト処理をスキップします。
				}
				// ユーザーが「キャンセル」を選択した場合は
				else if (id == IDCANCEL)
				{
					// デフォルト処理をスキップします。
					return 0;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} aviutl_window;

	BOOL func_init(AviUtl::FilterPlugin* fp)
	{
		dirty_flag.init(fp);
		aviutl_window.init(fp);

		return TRUE;
	}

	BOOL func_exit(AviUtl::FilterPlugin* fp)
	{
		return TRUE;
	}

	BOOL func_wndproc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		switch (message)
		{
		case AviUtl::FilterPlugin::WindowMessage::Update:
			{
				if (!fp->exfunc->is_editing(editp)) break;

				// プロジェクトが変更されたかもしれないのでダーティーフラグを更新します。
				dirty_flag.update();

				break;
			}
		case AviUtl::FilterPlugin::WindowMessage::FileClose:
			{
				// プロジェクトが閉じられたのでダーティーフラグをクリアします。
				dirty_flag.clear();

				break;
			}
		}

		return FALSE;
	}

	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void*, int*)
	{
		// プロジェクトが保存されたのでダーティーフラグをクリアします。
		dirty_flag.clear();

		// フラグをクリアしてもタイトルは変わらないので手動で元に戻します。
		auto title = my::get_window_text(fp->hwnd_parent);
		if (title[0] == _T('*') && title[1] == _T(' '))
			::SetWindowText(fp->hwnd_parent, title.c_str() + 2);

		return FALSE;
	}

	BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int frame, LPSTR title, int max_title)
	{
		// プロジェクトが変更されたかもしれないのでダーティーフラグを更新します。
		dirty_flag.update();

		// ダーティーフラグが立っていない場合は何もしません。
		if (!dirty_flag.get()) return FALSE;

		// タイトルにダーティーマークを付けます。
		strcpy_s(title, max_title, my::format("* {/}", title).c_str());

		return TRUE;
	}
}
