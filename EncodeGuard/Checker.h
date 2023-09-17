#pragma once
#include "Hive.h"

namespace fgo::encode_guard
{
	//
	// このクラスはエンコード設定をチェックします。
	//
	struct Checker
	{
		AviUtl::FilterPlugin* fp = 0;
		int32_t s = 0; // 選択範囲の開始位置。
		int32_t e = 0; // 選択範囲の終了位置。
		AviUtl::EditHandle* editp = 0;
		int32_t frame_n = 0; // フレーム総数。
		int32_t frame_end = 0; // 最終フレーム位置。
		AviUtl::FileInfo fi = {};
		TCHAR text[1024] = {}; // チェックに引っかかったとき出力するテキスト。

		//
		// コンストラクタです。
		//
		Checker(AviUtl::FilterPlugin* fp, int32_t s, int32_t e, AviUtl::EditHandle* editp)
			: fp(fp)
			, s(s)
			, e(e)
			, editp(editp)
		{
			// フレーム総数と最終フレーム位置はよく使うのでここで取得しておく。
			frame_n = fp->exfunc->get_frame_n(editp);
			frame_end = frame_n - 1;

			// フレームを時間に変換するのにファイル情報が必要になるのでここで取得しておく。
			fp->exfunc->get_file_info(editp, &fi);
		}

		//
		// このクラスでフレームを時間に変換する。
		// そんなにたくさんの計算はしないので精度重視の double で処理する。
		//
		struct Time {
			double time;
			Time(int32_t frame, const AviUtl::FileInfo& fi) {
				// ビデオレートとビデオスケールを使用してフレームを時間に変換する。
				// 除算するのでビデオレートは 0 かどうかチェックしておく。
				if (fi.video_rate == 0)
					this->time = 0;
				else
					this->time = (double)frame * fi.video_scale / fi.video_rate;
			}
			int hour(){ return (int)time / 3600; }
			int min(){ return (int)time / 60 % 60; }
			double getSec(){ return fmod(time, 60.0); }
		};

		//
		// 以下の関数で Time クラスを使用して終了位置を時間に変換するとき、
		// +1 してから渡した方が大体切りの良い数値になる事が多いのでそうしている。
		// しかし、絶対そうなるというわけでもないし必須というわけでもない。これは好みの問題。
		//

		//
		// 出力範囲が指定されている場合は TRUE を返す。
		// さらにその場合はメッセージボックスに表示するテキストを text に追加する。
		//
		BOOL checkRange()
		{
			if (s == 0 && e == frame_end)
				return FALSE; // 出力範囲は指定されていないので全体が出力される。

			{
				Time total(frame_end + 1, fi);
				Time selectStart(s, fi);
				Time selectEnd(e + 1, fi);

				TCHAR subText[MAX_PATH] = {};
				::StringCchPrintf(subText, std::size(subText),
					_T("注意 : 全体の長さが [%02d:%02d:%05.2f] のプロジェクトに対して\n")
					_T("[%02d:%02d:%05.2f]～[%02d:%02d:%05.2f] の出力範囲が指定されています\n")
					_T("この設定のままだとプロジェクトの一部分しか出力されません\n\n"),
					total.hour(), total.min(), total.getSec(),
					selectStart.hour(), selectStart.min(), selectStart.getSec(),
					selectEnd.hour(), selectEnd.min(), selectEnd.getSec());

				// text に subText を追加する。
				::StringCchCat(text, std::size(text), subText);
			}

			return TRUE;
		}

		//
		// 最終フレーム位置と全アイテムの最終位置が一致しない場合は TRUE を返す。
		// さらにその場合はメッセージボックスに表示するテキストを text に追加する。
		//
		BOOL checkLastFrame()
		{
			if (magi.auin.GetExEditFrameNumber() == 0)
				return FALSE; // 拡張編集の最終フレーム番号が無効の場合は何もしない。

			// 現在編集中のシーンのインデックスを取得する。
			int scene = magi.auin.GetCurrentSceneIndex();

			// 現在編集中のシーンの中で最も後ろにあるオブジェクトの終了位置を取得する。
			int item_end = -1;
			{
				// オブジェクトの個数を取得する。
				int c = magi.auin.GetCurrentSceneObjectCount();

				for (int i = 0; i < c; i++)
				{
					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetSortedObject(i);

					if (scene != object->scene_set)
						continue; // 現在のシーン内のオブジェクトではなかった。

					item_end = std::max(item_end, object->frame_end);
				}
			}

			// オブジェクトの終了位置が小さすぎる場合は何もしない。
			if (item_end <= 0)
				return FALSE;

			// オブジェクトの終了位置が最終フレーム位置より大きい場合は何もしない。
			if (item_end >= frame_end)
				return FALSE;

			{
				Time frameTime(frame_end + 1, fi);
				Time itemTime(item_end + 1, fi);

				TCHAR subText[MAX_PATH] = {};
				::StringCchPrintf(subText, std::size(subText),
					_T("注意 : 最終フレーム位置と全アイテムの最終位置が一致しません\n")
					_T("[%02d:%02d:%05.2f] (最終フレーム位置)\n")
					_T("[%02d:%02d:%05.2f] (全アイテムの最終位置)\n")
					_T("この設定のままだと尺余りが発生する可能性があります\n\n"),
					frameTime.hour(), frameTime.min(), frameTime.getSec(),
					itemTime.hour(), itemTime.min(), itemTime.getSec());

				// text に subText を追加する。
				::StringCchCat(text, std::size(text), subText);
			}

			return TRUE;
		}

		//
		// [フレームレートの変更] で [なし] 以外が指定されている場合は TRUE を返す。
		// さらにその場合はメッセージボックスに表示するテキストを text に追加する。
		//
		BOOL checkFrameRate()
		{
			// メモリアドレスから [フレームレートの変更] を取得する。
			auto aviutl_base = (uintptr_t)::GetModuleHandle(0);
			auto framerate_fp = (AviUtl::FilterPlugin*)(aviutl_base + 0x080b28);
			auto framerateConfig = framerate_fp->track_array[0];

			if (framerateConfig == 0)
				return FALSE; // [フレームレートの変更] が [なし] だった。

			{
				LPCSTR label = "不明";

				// framerateConfig がインデックスとして使えそうなら項目名を取得する。
				if (framerateConfig >= 0 && framerateConfig < framerate_fp->check_n)
					label = framerate_fp->check_name[framerateConfig];

				TCHAR subText[MAX_PATH] = {};
				::StringCchPrintf(subText, std::size(subText),
					_T("注意 : [設定] > [フレームレートの変更] が\n")
					_T("[%hs] に設定されています\n")
					_T("この設定のままだとフレームレートが低下した状態で出力されます\n\n"),
					label);

				// text に subText を追加する。
				::StringCchCat(text, std::size(text), subText);
			}

			return TRUE;
		}

		//
		// ユーザーが指定したチェックを実行する。
		// チェックに引っかかった場合は処理を継続するかユーザーに問い合わせる。
		// ユーザーが拒否した場合は FALSE を返す。それ以外の場合は TRUE を返す。
		//
		BOOL check()
		{
			// 有効になっているチェックを実行する。
			if (hive.checkRange) checkRange();
			if (hive.checkLastFrame) checkLastFrame();
			if (hive.checkFrameRate) checkFrameRate();

			// text の長さが 0 以外になっているなら、どれかのチェックに引っかかっている。
			if (_tcslen(text) != 0)
			{
				::StringCchCat(text, std::size(text), _T("このまま出力を実行しますか？"));

				// ユーザーが出力を拒否した場合は FALSE を返す。
				if (IDNO == ::MessageBox(hive.mainWindow, text, hive.AppName, MB_YESNO | MB_ICONEXCLAMATION))
					return FALSE;
			}

			return TRUE;
		}
	};
}
