#pragma once

namespace apn::output_check
{
	//
	// このクラスは出力設定をチェックします。
	//
	struct Checker
	{
		AviUtl::FilterPlugin* fp = nullptr;
		AviUtl::EditHandle* editp = nullptr;
		int32_t s = 0; // 選択範囲の開始位置です。
		int32_t e = 0; // 選択範囲の終了位置です。
		int32_t frame_n = 0; // フレーム総数です。
		int32_t frame_end = 0; // 最終フレーム位置です。
		AviUtl::FileInfo fi = {};
		std::wstring text; // チェックに引っかかったとき表示するテキストです。

		//
		// コンストラクタです。
		//
		Checker(AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
			: fp(fp)
			, editp(editp)
		{
			magi.fp->exfunc->get_select_frame(editp, &s, &e);

			// フレーム総数と最終フレーム位置はよく使うのでここで取得しておきます。
			frame_n = fp->exfunc->get_frame_n(editp);
			frame_end = frame_n - 1;

			// フレームを時間に変換するのにファイル情報が必要になるのでここで取得しておきます。
			fp->exfunc->get_file_info(editp, &fi);
		}

		//
		// このクラスでフレームを時間に変換します。
		// そんなにたくさんの計算はしないので精度重視のdoubleで処理します。
		//
		struct Time {
			double time;
			Time(int32_t frame, const AviUtl::FileInfo& fi) {
				// ビデオレートとビデオスケールを使用してフレームを時間に変換します。
				// 除算するのでビデオレートは0かどうかチェックします。
				if (fi.video_rate == 0)
					this->time = 0;
				else
					this->time = (double)frame * fi.video_scale / fi.video_rate;
			}
			int hour(){ return (int)time / 3600; }
			int min(){ return (int)time / 60 % 60; }
			double sec(){ return fmod(time, 60.0); }
		};

		//
		// 終了位置を+1してから時間に変換していますが、これは必須の処理ではありません。
		// しかし、大抵の場合は+1したほうが切りの良い数値になって見栄えが良いです。
		//

		//
		// 出力範囲が指定されている場合はTRUEを返します。
		// その場合はメッセージボックスに表示するテキストをtextに追加します。
		//
		BOOL check_range()
		{
			// 出力範囲が指定されていない場合は何もしません。
			if (s == 0 && e == frame_end) return FALSE;

			{
				Time total(frame_end + 1, fi);
				Time sel_start(s, fi);
				Time sel_end(e + 1, fi);

				text += std::format(
					_T("注意 : 全体の長さが [{:02d}:{:02d}:{:05.2f}] のプロジェクトに対して\n")
					_T("[{:02d}:{:02d}:{:05.2f}]～[{:02d}:{:02d}:{:05.2f}] の出力範囲が指定されています\n")
					_T("この設定のままだとプロジェクトの一部分しか出力されません\n\n"),
					total.hour(), total.min(), total.sec(),
					sel_start.hour(), sel_start.min(), sel_start.sec(),
					sel_end.hour(), sel_end.min(), sel_end.sec());
			}

			return TRUE;
		}

		//
		// 最終フレーム位置と全アイテムの最終位置が一致しない場合はTRUEを返します。
		// その場合はメッセージボックスに表示するテキストをtextに追加します。
		//
		BOOL check_last_frame()
		{
			// 拡張編集の最終フレーム番号が無効の場合は何もしません。
			if (magi.exin.get_exedit_frame_number() == 0) return FALSE;

			// 現在編集中のシーンのインデックスを取得します。
			auto scene = magi.exin.get_current_scene_index();

			// 現在編集中のシーンの中で最も後ろにあるオブジェクトの終了位置を取得します。
			auto item_end = -1;
			{
				// オブジェクトの個数を取得します。
				auto c = magi.exin.get_sorted_object_count();

				for (decltype(c) i = 0; i < c; i++)
				{
					// オブジェクトを取得します。
					auto object = magi.exin.get_sorted_object(i);

					// 現在のシーン内のオブジェクトかチェックします。
					if (scene != object->scene_set) continue;

					// オブジェクトの終了位置の最大値を取得します。
					item_end = std::max(item_end, object->frame_end);
				}
			}

			// オブジェクトの終了位置が小さすぎる場合は何もしません。
			if (item_end <= 0) return FALSE;

			// オブジェクトの終了位置が最終フレーム位置より大きい場合は何もしません。
			if (item_end >= frame_end) return FALSE;

			{
				Time frame_time(frame_end + 1, fi);
				Time item_time(item_end + 1, fi);

				text += std::format(
					_T("注意 : 最終フレーム位置と全アイテムの最終位置が一致しません\n")
					_T("[{:02d}:{:02d}:{:05.2f}] (最終フレーム位置)\n")
					_T("[{:02d}:{:02d}:{:05.2f}] (全アイテムの最終位置)\n")
					_T("この設定のままだと尺余りが発生する可能性があります\n\n"),
					frame_time.hour(), frame_time.min(), frame_time.sec(),
					item_time.hour(), item_time.min(), item_time.sec());
			}

			return TRUE;
		}

		//
		// [フレームレートの変更]で[なし]以外が指定されている場合はTRUEを返します。
		// その場合はメッセージボックスに表示するテキストをtextに追加します。
		//
		BOOL check_frame_rate()
		{
			// メモリアドレスから[フレームレートの変更]を取得します。
			auto aviutl_base = (my::addr_t)::GetModuleHandle(nullptr);
			auto framerate_fp = (AviUtl::FilterPlugin*)(aviutl_base + 0x080b28);
			auto framerate_config = framerate_fp->track_array[0];

			// [フレームレートの変更]が[なし]の場合は何もしません。
			if (framerate_config == 0) return FALSE;

			{
				auto label = "不明";

				// framerate_configがインデックスとして使えそうなら項目名を取得します。
				if (framerate_config >= 0 && framerate_config < framerate_fp->check_n)
					label = framerate_fp->check_name[framerate_config];

				text += std::format(
					_T("注意 : [設定] > [フレームレートの変更]が\n")
					_T("[{}]に設定されています\n")
					_T("この設定のままだとフレームレートが低下した状態で出力されます\n\n"),
					my::ts(label));
			}

			return TRUE;
		}

		//
		// ユーザーが指定したチェックを実行します。
		// チェックに引っかかった場合は処理を継続するかユーザーに問い合わせます。
		// ユーザーが拒否した場合はFALSEを返します。それ以外の場合はTRUEを返します。
		//
		BOOL check()
		{
			// 有効になっているチェックを実行します。
			if (hive.check_range) check_range();
			if (hive.check_last_frame) check_last_frame();
			if (hive.check_frame_rate) check_frame_rate();

			// textが空文字列ではない場合は、チェックに引っかかっています。
			if (!text.empty())
			{
				text += _T("このまま出力を実行しますか？");

				// ユーザーが出力を拒否した場合はFALSEを返します。
				if (IDNO == hive.message_box(text, nullptr, MB_YESNO | MB_ICONEXCLAMATION))
					return FALSE;
			}

			return TRUE;
		}
	};
}
