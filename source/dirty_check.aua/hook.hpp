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
	inline struct DirtyFlag
	{
		//
		// タイトル文字列に付与するマークです。
		//
		inline static const std::wstring mark = L"* ";

		const int* g_undo_id_ptr = {};
		int g_undo_id_prev = {};
		BOOL g_dirty_flag = {};

		//
		// 初期化処理を実行します。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// 拡張編集を取得します。
			auto exedit = magi.exin.get_exedit();

			// 拡張編集のアンドゥIDへのポインタを取得します。
			g_undo_id_ptr = (int32_t*)(exedit + 0x244E08 + 12);

			return TRUE;
		}

		//
		// 指定されたウィンドウメッセージがトリガーならTRUEを返します。
		//
		BOOL is_trigger_message(UINT msg, WPARAM wp)
		{
			if (!get()) return FALSE;
			if (msg == WM_CLOSE) return TRUE;
			if (msg == WM_COMMAND)
			{
				return wp == 5157 // 閉じる
					|| wp == 5097 // 開く
					|| wp == 5118 // 編集プロジェクトを開く
					|| (5596 <= wp && wp <= 5603) // 最近使ったファイル1-8
					;
			}

			return FALSE;
		}

		//
		// ダーティーフラグを返します。
		//
		BOOL get()
		{
			return g_dirty_flag;
		}

		//
		// ダーティーフラグをセットします。
		//
		void set()
		{
			// 前回のアンドゥIDと現在のアンドゥIDが異なる場合は
			if (g_undo_id_prev != *g_undo_id_ptr)
			{
				// アンドゥIDを更新します。
				g_undo_id_prev = *g_undo_id_ptr;

				// ダーティーフラグを立てます。
				g_dirty_flag = TRUE;
			}
		}

		//
		// ダーティーフラグをリセットします。
		//
		void reset()
		{
			// ダーティーフラグを消去します。
			g_dirty_flag = FALSE;
		}
	} dirty_flag;

	//
	// このクラスはaviutlウィンドウをフックします。
	//
	inline struct AviUtlWindow : my::Window
	{
		//
		// 初期化を行います。
		//
		BOOL init(AviUtl::FilterPlugin* fp)
		{
			// aviutlウィンドウをサブクラス化します。
			return subclass(fp->hwnd_parent);
		}

		//
		// aviutlウィンドウのウィンドウプロシージャをフックします。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			// 終了確認のトリガーかチェックします。
			if (dirty_flag.is_trigger_message(message, wParam))
			{
				// メッセージボックスを表示します。
				auto id = hive.message_box(
					L"変更された編集データがあります。保存しますか？",
					hwnd, MB_YESNOCANCEL | MB_ICONQUESTION | MB_DEFBUTTON1);

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
		MY_TRACE_FUNC("{/hex}, {/hex}, {/hex}", message, wParam, lParam);

		switch (message)
		{
		case AviUtl::FilterPlugin::WindowMessage::Update:
			{
				if (!fp->exfunc->is_editing(editp)) break;

				// プロジェクトが変更されたかもしれないのでダーティーフラグをセットします。
				dirty_flag.set();

				break;
			}
		case AviUtl::FilterPlugin::WindowMessage::FileClose:
			{
				// プロジェクトが閉じられたのでダーティーフラグをリセットします。
				dirty_flag.reset();

				break;
			}
		}

		return FALSE;
	}

	BOOL func_project_save(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, void* buffer, int* buffer_size)
	{
		// bufferがnullptrのときとそれ以外のときで2回呼ばれるので
		// bufferがnullptrのときにだけ処理します。
		if (!buffer)
		{
			// 以下のeditpのデータが
			// autosaverが保存したときは0、通常保存の場合は0以外になるようなので
			// 0以外のときにだけ処理します。
			if (editp->field_34c[5] ||
				editp->field_34c[6] ||
				editp->field_34c[7])
			{
				// プロジェクトが保存されたのでダーティーフラグをリセットします。
				dirty_flag.reset();

				// フラグをクリアしてもタイトルは変わらないので手動で元に戻します。
				auto title = my::get_window_text(fp->hwnd_parent);
				if (title.starts_with(dirty_flag.mark))
					::SetWindowTextW(fp->hwnd_parent, title.c_str() + dirty_flag.mark.length());
			}
		}

		return FALSE;
	}

	BOOL func_modify_title(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp, int frame, LPSTR title, int max_title)
	{
		// プロジェクトが変更されたかもしれないのでダーティーフラグをセットします。
		dirty_flag.set();

		// ダーティーフラグが立っている場合は
		if (dirty_flag.get())
		{
			// タイトルにダーティーマークを付けます。
			strcpy_s(title, max_title, my::format("{/}{/}", dirty_flag.mark, title).c_str());

			return TRUE;
		}

		return FALSE;
	}
}
