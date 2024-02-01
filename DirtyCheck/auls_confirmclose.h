////////////////////////////////////////////////////////////////////////////////
// auls_confirmclose.cpp
// 編集内容が変更されている時のみ終了確認するようにするプラグイン
// -----------------------------------------------------------------------------
// 補足情報
// LoadLibrary で自分自身の参照カウントを増やしておかないと
// 終了時にプロセスが長時間、あるいはずっと残ってしまう。
////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace fgo::dirty_check
{
	LPCSTR FILTER_NAME = "終了確認";
	LPCSTR FILTER_INFORMATION = "Auls終了確認 v1.2 forked by karoterra";

	LPCSTR EXEDIT_NAME = "拡張編集";
	LPCSTR EXEDIT_92 = "拡張編集(exedit) version 0.92 by ＫＥＮくん";

	inline struct DirtyFlag
	{
		const int* g_undo_id_ptr = 0;
		int  g_undo_id_prev = 0;
		bool g_dirty_flag = false;

		//
		// 拡張編集のフィルタを返します。
		//
		static AviUtl::FilterPlugin* get_exedit(AviUtl::FilterPlugin* fp)
		{
			AviUtl::SysInfo si;
			fp->exfunc->get_sys_info(nullptr, &si);
			for (int i = 0; i < si.filter_n; i++) {
				auto fp1 = static_cast<AviUtl::FilterPlugin*>(fp->exfunc->get_filterp(i));
				if (strcmp(fp1->name, EXEDIT_NAME) == 0 && strcmp(fp1->information, EXEDIT_92) == 0) {
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
			auto exedit = get_exedit(fp);
			if (exedit == nullptr) {
				MessageBoxA(fp->hwnd, "対応する拡張編集が見つかりません。", FILTER_NAME, MB_OK | MB_ICONERROR);
				return FALSE;
			}

			// 拡張編集のアンドゥIDへのポインタを取得します。
			g_undo_id_ptr = reinterpret_cast<int*>(reinterpret_cast<size_t>(exedit->dll_hinst) + 0x244E08 + 12);

			return TRUE;
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
		// ダーティーフラグを更新します．
		//
		void update()
		{
			if (!g_dirty_flag && *g_undo_id_ptr != g_undo_id_prev)
				g_dirty_flag = true;
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

	inline struct AviUtlWindow
	{
		HWND hwnd = 0;
		WNDPROC orig = 0;

		//
		// AviUtlウィンドウのウィンドウプロシージャをフックします。
		//
		static LRESULT CALLBACK hook(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
		{
			if (dirty_flag.is_trigger_message(msg, wp)) {
				int id = MessageBoxA(hwnd, "変更された編集データがあります。保存しますか？",
					FILTER_NAME, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);
				if (id == IDYES) {
					SendMessage(hwnd, WM_COMMAND, 1024, 0); // 編集プロジェクトの上書き
					if (dirty_flag.get()) return 0; // 保存がキャンセルされたら終了しない
				}
				else if (id == IDCANCEL) return 0;
			}
			return CallWindowProc(aviutl_window.orig, hwnd, msg, wp, lp);
		}

		//
		// 初期化処理を行います。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// AviUtlウィンドウを取得します。
			hwnd = GetWindow(fp->hwnd, GW_OWNER);

			// AviUtlウィンドウをサブクラス化します。
			orig = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG)hook);

			return TRUE;
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

	BOOL func_wndproc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
	{
		switch (msg) {
		case AviUtl::FilterPlugin::WindowMessage::Update:
			if (!fp->exfunc->is_editing(editp)) break;
			// プロジェクトが変更されたかもしれないのでダーティーフラグを更新します．
			dirty_flag.update();
			break;
		case AviUtl::FilterPlugin::WindowMessage::FileClose:
			// プロジェクトが閉じられたのでダーティーフラグをクリアします。
			dirty_flag.clear();
			break;
		}
		return FALSE;
	}

	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void*, int*)
	{
		// プロジェクトが保存されたのでダーティーフラグをクリアします。
		dirty_flag.clear();

		// フラグをクリアしてもタイトルは変わらないので手動で変更．
		char title[MAX_PATH];
		::GetWindowTextA(fp->hwnd_parent, title, std::size(title));
		if (title[0] == '*' && title[1] == ' ')
			::SetWindowTextA(fp->hwnd_parent, title + 2);
		return FALSE;
	}

	BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int frame, LPSTR title, int max_title)
	{
		// プロジェクトが変更されたかもしれないのでダーティーフラグをチェックします。
		dirty_flag.update();
		if (!dirty_flag.get()) return FALSE;
		std::string str(title);
		sprintf_s(title, max_title, "* %s", str.c_str());
		return TRUE;
	}
}
