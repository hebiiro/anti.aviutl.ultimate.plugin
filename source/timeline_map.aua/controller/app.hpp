#pragma once

namespace apn::timeline_map::controller
{
	//
	// このクラスはコントローラ層のアプリケーションです。
	//
	inline struct app_t : view::listner_t
	{
		//
		// コンストラクタです。
		//
		app_t()
		{
			MY_TRACE_FUNC("");

			// 描画設定ダイアログのリスナーに設定します。
			view::listener = this;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			// コンフィグ入出力を初期化します。
			if (!config_io.init())
			{
				MY_TRACE("コンフィグ入出力の初期化に失敗しました\n");
			}

			// フックを初期化します。
			if (!hook::exedit_window.init())
			{
				view::overview.message_box(L"フックの初期化に失敗しました\n");

				return FALSE;
			}

			// 全体図を初期化します。
			if (!view::overview.init())
			{
				view::overview.message_box(L"全体図の初期化に失敗しました\n");

				return FALSE;
			}

			// 拡大鏡を初期化します。
			if (!view::loupe.init())
			{
				view::overview.message_box(L"拡大鏡の初期化に失敗しました\n");

				return FALSE;
			}

			// コンフィグを読み込みます。
			config_io.read();

			return FALSE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			// コンフィグを書き込みます。
			config_io.write();

			// モデル及びビューモデルのリソースを開放します。
			model::state.reset_resources();

			// 拡大鏡を終了します。
			view::loupe.exit();

			// 全体図を終了します。
			view::overview.exit();

			// フックを終了します。
			hook::exedit_window.exit();

			// コンフィグ入出力を終了します。
			config_io.exit();

			return FALSE;
		}

		//
		// 更新処理を実行します。
		//
		BOOL update()
		{
			MY_TRACE_FUNC("");

			// 全体図を更新します。
			view::overview.on_update();

			// 拡大鏡を更新します。
			view::loupe.on_update();

			return FALSE;
		}

		//
		// この仮想関数は描画設定ダイアログで変更があったときに呼び出されます。
		//
		virtual void on_from_ui(BOOL recreate_resources) override
		{
			MY_TRACE_FUNC("{/}", recreate_resources);

			// リソースを再作成します。
			if (recreate_resources)
				model::state.recreate_resources(TRUE);

			// 全体図を再描画します。
			view::overview.redraw();

			// 拡大鏡を再描画します。
			view::loupe.redraw();
		}
	} app;
}
