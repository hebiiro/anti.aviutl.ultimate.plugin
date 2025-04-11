#pragma once

namespace apn::item_align
{
	//
	// このクラスは各種計算を実行します。
	//
	struct Utils
	{
		//
		// イプシロンです。
		//
		inline static constexpr auto epsilon = 0.0001;

		//
		// 計算に必要な変数です。
		// コンストラクタで初期化されます。
		//
		AviUtl::EditHandle* editp;
		AviUtl::FilterPlugin* fp;
		int32_t fps;
		int32_t bpm;
		double frame_per_time;

		//
		// コンストラクタです。
		//
		Utils()
		{
			MY_TRACE_FUNC("");

			// よく使う変数を取得しておきます。
			fp = magi.fp;
			editp = magi.exin.get_editp();

			// ファイル情報を取得します。
			AviUtl::FileInfo fi = {};
			fp->exfunc->get_file_info(editp, &fi);

			// フレームレートを取得します。
			if (fi.video_rate != 0 && fi.video_scale != 0)
				fps = fi.video_rate / fi.video_scale;
			MY_TRACE_INT(fps);

			// BPMを取得します。
			bpm = magi.exin.get_bpm();
			MY_TRACE_INT(bpm);

			// 1区間当たりのフレーム数を取得します。
			if (bpm != 0)
				frame_per_time = fps / (bpm / 60.0 / 10000);
			else
				frame_per_time = 0;
			MY_TRACE_REAL(frame_per_time);
		}

		//
		// 浮動小数点数を整数に変換して返します。
		//
		inline static int32_t to_int(double x)
		{
			return (int32_t)(x + epsilon);
		}

		//
		// 指定された文字列を時間に変換して返します。
		//
		inline static double str_to_time(const std::wstring& str)
		{
			std::wstring s(str);

			auto sep1 = s.find(L':');
			if (sep1 == s.npos) {
				return
					wcstod(&s[0], nullptr); // 秒
			}
			s[sep1] = L'\0';

			auto sep2 = s.find(L':', sep1 + 1);
			if (sep2 == s.npos) {
				return
					wcstod(&s[0], nullptr) * 60 + // 分
					wcstod(&s[sep1 + 1], nullptr); // 秒
			}
			s[sep2] = L'\0';

			return
				wcstod(&s[0], nullptr) * 60 * 60 + // 時
				wcstod(&s[sep1 + 1], nullptr) * 60 + // 分
				wcstod(&s[sep2 + 1], nullptr); // 秒
		}

		//
		// 指定された時間を文字列に変換して返します。
		//
		inline static std::wstring time_to_str(double time)
		{
			return my::format(L"{/02d}:{/02d}:{/05.2f}",
				to_int(time) / 60 / 60, // 時
				to_int(time) / 60 % 60, // 分
				std::fmod(time, 60)); // 秒
		}

		//
		// 指定された時間を文字列に変換して返します。
		//
		inline static std::wstring time_to_str(double time_start, double time_end)
		{
			return my::format(L"{/}-{/}", time_to_str(time_start), time_to_str(time_end));
		}

		//
		// フレームを時間に変換して返します。
		//
		double frame_to_time(int32_t frame) const
		{
			return (double)frame / fps;
		}

		//
		// 時間をフレームに変換して返します。
		//
		int32_t time_to_frame(double time) const
		{
			return to_int(time * fps);
		}

		//
		// BPMを時間に変換して返します。
		//
		double bpm_to_time(int32_t bpm) const
		{
			return 10000 * 60.0 * bpm / this->bpm;
		}

		//
		// 時間を開始位置(時間)に変換して返します。
		//
		double to_time_start(double time) const
		{
			return time + frame_to_time(1);
		}

		//
		// 時間を終了位置(時間)に変換して返します。
		//
		double to_time_end(double time) const
		{
			return time - frame_to_time(1);
		}

		//
		// 基準開始位置(時間)を返します。
		//
		double get_base_time_start(double time)
		{
			if (hive.use_current_frame)
				return frame_to_time(magi.exin.get_exedit_current_frame());
			else
				return time;
		}

		//
		// 基準終了位置(時間)を返します。
		//
		double get_base_time_end(double time)
		{
			if (hive.use_current_frame)
				return frame_to_time(magi.exin.get_exedit_current_frame() - 1);
			else
				return time;
		}
	};
}
