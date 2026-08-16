#pragma once

namespace apn::dark::kuro
{
	//
	// このクラスはaviutl2のstyle.confの変数を保持します。
	//
	inline struct style_t : entry_t::node_t
	{
		virtual BOOL on_init() { return init(); }
		virtual BOOL on_exit() { return exit(); }

		//
		// 配色のインデックスです。
		//
		enum class color_e : size_t {
			// デフォルトの背景色 
			Background,
			// ウィンドウの枠線色 
			WindowBorder,
			// ウィンドウ間の背景色
			WindowSeparator,
			// フッターの背景色
			Footer,
			// フッターの進捗色
			FooterProgress,
			// グループ項目の背景色
			Grouping,
			// グループ項目の背景色(マウスが乗った時)
			GroupingHover,
			// グループ項目の背景色(選択時)
			GroupingSelect,
			// グループ項目の区切色
			GroupingSeparator,
			// タイトルヘッダーの背景色 
			TitleHeader,
			BorderSelect,
			Border,
			BorderFocus,
			// テキストの色
			Text,
			// テキストの色(無効時)
			TextDisable,
			// テキスト範囲選択色
			TextSelect,
			// ボタンの背景色
			ButtonBody,
			// ボタンの背景色(マウスが乗った時)
			ButtonBodyHover,
			// ボタンの背景色(マウスを押している時)
			ButtonBodyPress,
			// ボタンの背景色(無効時)
			ButtonBodyDisable,
			// ボタンの背景色(選択時)
			ButtonBodySelect,
			// スライダーのカーソル色
			SliderCursor,
			// スライダーのつまみの背景色
			SliderThumbBody,
			// スライダーのつまみの背景色(フォーカス時)
			SliderThumbBodyFocus,
			// スライダーのつまみの枠線色
			SliderThumbBorder,
			TrackBarRange,
			ZoomGauge,
			ZoomGaugeHover,
			ZoomGaugeOff,
			ZoomGaugeOffHover,
			// フレーム選択範囲の色
			FrameRangeSelect,
			// フレーム選択範囲外の色
			FrameRangeOutside,
			FrameCursor,
			FrameCursorWide,
			PlayerCursor,
			GuideLine,
			// レイヤーの背景色
			Layer,
			// レイヤーの背景色(ヘッダー部分)
			LayerHeader,
			// レイヤーの背景色(マウスが乗った時)
			LayerHover,
			// レイヤーの背景色(非表示)
			LayerDisable,
			// レイヤーの背景色(選択時)
			LayerSelect,
			// レイヤー範囲の背景色 
			LayerRange,
			// レイヤー範囲の枠線色 
			LayerRangeFrame,
			// 映像メディアオブジェクトの色
			ObjectVideo,
			// 映像メディアオブジェクトの色(選択時)
			ObjectVideoSelect,
			// 音声メディアオブジェクトの色
			ObjectAudio,
			// 音声メディアオブジェクトの色(選択時)
			ObjectAudioSelect,
			// 制御オブジェクトの色
			ObjectControl,
			// 制御オブジェクトの色(選択時)
			ObjectControlSelect,
			// 映像フィルタオブジェクトの色
			ObjectVideoFilter,
			// 映像フィルタオブジェクトの色(選択時)
			ObjectVideoFilterSelect,
			// 音声フィルタオブジェクトの色
			ObjectAudioFilter,
			// 音声フィルタオブジェクトの色(選択時)
			ObjectAudioFilterSelect,
			// 映像フィルタ効果オブジェクトの色
			ObjectVideoEffect,
			// 映像フィルタ効果オブジェクトの色(選択時)
			ObjectVideoEffectSelect,
			// 音声フィルタ効果オブジェクトの色
			ObjectAudioEffect,
			// 音声フィルタ効果オブジェクトの色(選択時)
			ObjectAudioEffectSelect,
			// オブジェクトの枠色(マウスが乗った時)
			ObjectHover,
			// オブジェクトの枠色(フォーカス時)
			ObjectFocus,
			// オブジェクト中間点の色
			ObjectSection,
			// オブジェクト音声波形の色
			ObjectWaveform,
			// 仮想バッファ出力オブジェクトの色(下部)
			ObjectTempBuffer,
			// クリッピングオブジェクトの色(下部)
			ClippingObject,
			// クリッピングオブジェクトの視覚化時の色
			ClippingObjectMask,
			// グリッド線色
			GridLine,
			// アンカー枠色
			Anchor,
			// アンカー線色
			AnchorLine,
			// アンカー枠色(開始)
			AnchorIn,
			// アンカー枠色(終了)
			AnchorOut,
			// アンカー枠色(マウスが乗った時)
			AnchorHover,
			// アンカー枠色(選択時)
			AnchorSelect,
			// アンカー枠の縁色
			AnchorEdge,
			// 中心点の色(グループ)
			CenterGroup,
			HandleX,
			HandleY,
			HandleZ,
			HandleXHover,
			HandleYHover,
			HandleZHover,
			// 表示領域外の色
			OutsideDisplay,
			// 音声レベルメーターの色
			LevelMeterRed,
			LevelMeterYellow,
			LevelMeterGreen,
			// ログの色
			LogVerbose,
			LogInfo,
			LogScript,
			LogPlugin,
			LogWarn,
			LogError,
			MaxSize,
		};

		//
		// 配色インデックスのマップです。
		// キーは配色名です。
		//
		const std::unordered_map<std::wstring, color_e> index_map = {
			// デフォルトの背景色 
			{ L"Background", color_e::Background },
			// ウィンドウの枠線色 
			{ L"WindowBorder", color_e::WindowBorder },
			// ウィンドウ間の背景色
			{ L"WindowSeparator", color_e::WindowSeparator },
			// フッターの背景色
			{ L"Footer", color_e::Footer },
			// フッターの進捗色
			{ L"FooterProgress", color_e::FooterProgress },
			// グループ項目の背景色
			{ L"Grouping", color_e::Grouping },
			// グループ項目の背景色(マウスが乗った時)
			{ L"GroupingHover", color_e::GroupingHover },
			// グループ項目の背景色(選択時)
			{ L"GroupingSelect", color_e::GroupingSelect },
			// グループ項目の区切色
			{ L"GroupingSeparator", color_e::GroupingSeparator },
			// タイトルヘッダーの背景色 
			{ L"TitleHeader", color_e::TitleHeader },
			{ L"BorderSelect", color_e::BorderSelect },
			{ L"Border", color_e::Border },
			{ L"BorderFocus", color_e::BorderFocus },
			// テキストの色
			{ L"Text", color_e::Text },
			// テキストの色(無効時)
			{ L"TextDisable", color_e::TextDisable },
			// テキスト範囲選択色
			{ L"TextSelect", color_e::TextSelect },
			// ボタンの背景色
			{ L"ButtonBody", color_e::ButtonBody },
			// ボタンの背景色(マウスが乗った時)
			{ L"ButtonBodyHover", color_e::ButtonBodyHover },
			// ボタンの背景色(マウスを押している時)
			{ L"ButtonBodyPress", color_e::ButtonBodyPress },
			// ボタンの背景色(無効時)
			{ L"ButtonBodyDisable", color_e::ButtonBodyDisable },
			// ボタンの背景色(選択時)
			{ L"ButtonBodySelect", color_e::ButtonBodySelect },
			// スライダーのカーソル色
			{ L"SliderCursor", color_e::SliderCursor },
			// スライダーのつまみの背景色
			{ L"SliderThumbBody", color_e::SliderThumbBody },
			// スライダーのつまみの背景色(フォーカス時)
			{ L"SliderThumbBodyFocus", color_e::SliderThumbBodyFocus },
			// スライダーのつまみの枠線色
			{ L"SliderThumbBorder", color_e::SliderThumbBorder },
			{ L"TrackBarRange", color_e::TrackBarRange },
			{ L"ZoomGauge", color_e::ZoomGauge },
			{ L"ZoomGaugeHover", color_e::ZoomGaugeHover },
			{ L"ZoomGaugeOff", color_e::ZoomGaugeOff },
			{ L"ZoomGaugeOffHover", color_e::ZoomGaugeOffHover },
			// フレーム選択範囲の色
			{ L"FrameRangeSelect", color_e::FrameRangeSelect },
			// フレーム選択範囲外の色
			{ L"FrameRangeOutside", color_e::FrameRangeOutside },
			{ L"FrameCursor", color_e::FrameCursor },
			{ L"FrameCursorWide", color_e::FrameCursorWide },
			{ L"PlayerCursor", color_e::PlayerCursor },
			{ L"GuideLine", color_e::GuideLine },
			// レイヤーの背景色
			{ L"Layer", color_e::Layer },
			// レイヤーの背景色(ヘッダー部分)
			{ L"LayerHeader", color_e::LayerHeader },
			// レイヤーの背景色(マウスが乗った時)
			{ L"LayerHover", color_e::LayerHover },
			// レイヤーの背景色(非表示)
			{ L"LayerDisable", color_e::LayerDisable },
			// レイヤーの背景色(選択時)
			{ L"LayerSelect", color_e::LayerSelect },
			// レイヤー範囲の背景色 
			{ L"LayerRange", color_e::LayerRange },
			// レイヤー範囲の枠線色 
			{ L"LayerRangeFrame", color_e::LayerRangeFrame },
			// 映像メディアオブジェクトの色
			{ L"ObjectVideo", color_e::ObjectVideo },
			// 映像メディアオブジェクトの色(選択時)
			{ L"ObjectVideoSelect", color_e::ObjectVideoSelect },
			// 音声メディアオブジェクトの色
			{ L"ObjectAudio", color_e::ObjectAudio },
			// 音声メディアオブジェクトの色(選択時)
			{ L"ObjectAudioSelect", color_e::ObjectAudioSelect },
			// 制御オブジェクトの色
			{ L"ObjectControl", color_e::ObjectControl },
			// 制御オブジェクトの色(選択時)
			{ L"ObjectControlSelect", color_e::ObjectControlSelect },
			// 映像フィルタオブジェクトの色
			{ L"ObjectVideoFilter", color_e::ObjectVideoFilter },
			// 映像フィルタオブジェクトの色(選択時)
			{ L"ObjectVideoFilterSelect", color_e::ObjectVideoFilterSelect },
			// 音声フィルタオブジェクトの色
			{ L"ObjectAudioFilter", color_e::ObjectAudioFilter },
			// 音声フィルタオブジェクトの色(選択時)
			{ L"ObjectAudioFilterSelect", color_e::ObjectAudioFilterSelect },
			// 映像フィルタ効果オブジェクトの色
			{ L"ObjectVideoEffect", color_e::ObjectVideoEffect },
			// 映像フィルタ効果オブジェクトの色(選択時)
			{ L"ObjectVideoEffectSelect", color_e::ObjectVideoEffectSelect },
			// 音声フィルタ効果オブジェクトの色
			{ L"ObjectAudioEffect", color_e::ObjectAudioEffect },
			// 音声フィルタ効果オブジェクトの色(選択時)
			{ L"ObjectAudioEffectSelect", color_e::ObjectAudioEffectSelect },
			// オブジェクトの枠色(マウスが乗った時)
			{ L"ObjectHover", color_e::ObjectHover },
			// オブジェクトの枠色(フォーカス時)
			{ L"ObjectFocus", color_e::ObjectFocus },
			// オブジェクト中間点の色
			{ L"ObjectSection", color_e::ObjectSection },
			// オブジェクト音声波形の色
			{ L"ObjectWaveform", color_e::ObjectWaveform },
			// 仮想バッファ出力オブジェクトの色(下部)
			{ L"ObjectTempBuffer", color_e::ObjectTempBuffer },
			// クリッピングオブジェクトの色(下部)
			{ L"ClippingObject", color_e::ClippingObject },
			// クリッピングオブジェクトの視覚化時の色
			{ L"ClippingObjectMask", color_e::ClippingObjectMask },
			// グリッド線色
			{ L"GridLine", color_e::GridLine },
			// アンカー枠色
			{ L"Anchor", color_e::Anchor },
			// アンカー線色
			{ L"AnchorLine", color_e::AnchorLine },
			// アンカー枠色(開始)
			{ L"AnchorIn", color_e::AnchorIn },
			// アンカー枠色(終了)
			{ L"AnchorOut", color_e::AnchorOut },
			// アンカー枠色(マウスが乗った時)
			{ L"AnchorHover", color_e::AnchorHover },
			// アンカー枠色(選択時)
			{ L"AnchorSelect", color_e::AnchorSelect },
			// アンカー枠の縁色
			{ L"AnchorEdge", color_e::AnchorEdge },
			// 中心点の色(グループ)
			{ L"CenterGroup", color_e::CenterGroup },
			{ L"HandleX", color_e::HandleX },
			{ L"HandleY", color_e::HandleY },
			{ L"HandleZ", color_e::HandleZ },
			{ L"HandleXHover", color_e::HandleXHover },
			{ L"HandleYHover", color_e::HandleYHover },
			{ L"HandleZHover", color_e::HandleZHover },
			// 表示領域外の色
			{ L"OutsideDisplay", color_e::OutsideDisplay },
			// 音声レベルメーターの色
			{ L"LevelMeterRed", color_e::LevelMeterRed },
			{ L"LevelMeterYellow", color_e::LevelMeterYellow },
			{ L"LevelMeterGreen", color_e::LevelMeterGreen },
			// ログの色
			{ L"LogVerbose", color_e::LogVerbose },
			{ L"LogInfo", color_e::LogInfo },
			{ L"LogScript", color_e::LogScript },
			{ L"LogPlugin", color_e::LogPlugin },
			{ L"LogWarn", color_e::LogWarn },
			{ L"LogError", color_e::LogError },
		};
		//
		// カラーエントリの配列です。
		//
		color_entry_t color_entries[(size_t)color_e::MaxSize] = {};

		//
		// カラーエントリを返します。
		//
		const color_entry_t& get_color_entry(color_e color_index) const
		{
			return color_entries[(size_t)color_index];
		}

		//
		// 値をカラーパーツに変換してコレクションに追加します。
		//
		void add_color_parts(const std::wstring& section, const std::wstring& key, const std::wstring& value)
		{
			// キーに対応するイテレータを取得します。
			auto it = index_map.find(key);
			if (it == index_map.end()) return;

			// 配色インデックスを取得します。
			auto index = (size_t)it->second;
			if (index >= std::size(color_entries)) return;

			// 値を配列に分割します。
			auto vec = split(value, L',');

			// 最大数を取得します。
			auto c = std::min(color_entry_t::c_max_size, vec.size());

			// 配列を走査します。
			for (size_t i = 0; i < c; i++)
			{
				// 文字列を取得します。
				const auto& str = vec[i];

				// 文字列が空の場合は何もしません。
				if (str.empty()) continue;

				// 取得予定のRGBAです。
				auto rgba = rgba_t {};

				// 文字列の長さで分岐します。
				switch (str.length())
				{
				// rrggbb形式の場合は
				case 6:
					{
						// 文字列をRGBAに変換します。
						rgba.value = wcstoul(str.data(), nullptr, 16) << 8;
						rgba.a = 0xff;

						break;
					}
				// rrggbbaa形式の場合は
				case 8:
					{
						// 文字列をRGBAに変換します。
						rgba.value = wcstoul(str.data(), nullptr, 16);

						break;
					}
				// それ以外の場合は
				default:
					{
						// 何もしません。
						continue;
					}
				}

				// カラーパーツをセットします。
				color_entries[index].parts[i] = { rgba };
			}
		}

		//
		// スタイルファイルを読み込みます。
		//
		BOOL read_file(const std::wstring& ini_file_name)
		{
			MY_TRACE_FUNC("{/}", ini_file_name);

			// 現在のセクションです。
			auto section = std::wstring {};

			// ファイルストリームを開きます。UTF-8です。
			std::ifstream stream(ini_file_name);

			// 一行ずつ読み込みます。
			auto utf8_line = std::string {};
			while (std::getline(stream, utf8_line))
			{
				// ワイド文字列に変換します。
				auto line = my::cp_to_wide(utf8_line, CP_UTF8);

				// 前後の空白を削除します。
				line = trim(line);

				// 空行は無視します。
				if (line.empty()) continue;

				// コメント行の場合は無視します。
				if (line.starts_with(L';')) continue;

				// セクション行の場合は
				if (line.starts_with(L'[') && line.ends_with(L']'))
				{
					// セクションを更新します。
					section = line.substr(1, line.length() - 2);

					// ループを続けます。
					continue;
				}

				// セパレータの位置を取得します。
				auto separator_pos = line.find_first_of(L'=');

				// セパレータの位置が無効の場合は無視します。
				if (separator_pos == line.npos) continue;

				// キーを取得します。
				auto key = trim(line.substr(0, separator_pos));

				// 値を取得します。
				auto value = trim(line.substr(separator_pos + 1));

				// コレクションに追加します。
				add_color_parts(section, key, value);
			}

			return TRUE;
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			try
			{
				// スタイルファイルのサブパスを作成します。
				auto sub_path = std::filesystem::path(hive.c_name) / L"style.conf";

				// スタイルファイルのパスを取得します。
				auto assets_path = magi.get_assets_file_name(sub_path);
				auto config_path = magi.get_config_file_name(sub_path);

				// コンフィグファイルが存在しない場合は
				if (!std::filesystem::exists(config_path))
				{
					// アセットファイルををコピーします。
					std::filesystem::copy(assets_path, config_path);
				}

				// ハイブにスタイルファイルのパスをセットします。
				hive.dark.style_file_name = config_path;

				// スタイルファイルを読み込みます。
				read_file(hive.dark.style_file_name);
			}
			// 例外が発生した場合は
			catch (std::exception& error)
			{
				// メッセージボックスでユーザーに通知します。
				hive.message_box(my::ws(error.what()));
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} style;
}
