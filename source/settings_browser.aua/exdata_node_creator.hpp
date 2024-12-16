#pragma once

namespace apn::settings_browser
{
	//
	// このクラスは拡張データ変換のインターフェイスです。
	//
	inline struct ExdataNodeCreator
	{
		//
		// 拡張データノード作成のコレクションです。
		//
		std::unordered_map<int32_t, std::function<n_json(void*)>> collection;

		//
		// コンストラクタです。
		//
		ExdataNodeCreator()
		{
			// 動画ファイル
			collection[0] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efMovieFile*)exdata;
				return {
					{ "file", u8(ef->file) },
					{ "i1", ef->i1 },
					{ "i2", ef->i2 },
					{ "frame_n", ef->frame_n },
					{ "video_rate", ef->video_rate },
					{ "video_scale", ef->video_scale },
					{ "i3", ef->i3 },
				};
			};

			// 画像ファイル
			collection[1] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efImageFile*)exdata;
				return {
					{ "unknown", ef->unknown },
					{ "file", u8(ef->file) },
				};
			};

			// 音声ファイル
			collection[2] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efAudioFile*)exdata;
				return {
					{ "file", u8(ef->file) },
					{ "i1", ef->i1 },
					{ "i2", ef->i2 },
					{ "frame_n", ef->frame_n },
					{ "i4", ef->i4 },
					{ "i5", ef->i5 },
				};
			};

			// テキスト
			collection[3] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efText*)exdata;
				return {
					{ "type", ef->type },
					{ "autoadjust", ef->autoadjust },
					{ "soft", ef->soft },
					{ "monospace", ef->monospace },
					{ "align", ef->align },
					{ "spacing_x", ef->spacing_x },
					{ "spacing_y", ef->spacing_y },
					{ "precision", ef->precision },
					{ "color", u8(ef->color) },
					{ "color2", u8(ef->color2) },
					{ "font", u8(ef->font) },
					{ "text", u8(ef->text) },
				};
			};

			// 図形
			collection[4] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efFigure*)exdata;
				return {
					{ "type", ef->type },
					{ "color", u8(ef->color) },
					{ "is_file", !!ef->padding },
					{ "name", u8(ef->name) },
				};
			};

			// フレームバッファ
			collection[5] = [](void* exdata) -> n_json
			{
				return {};
			};

			// 音声波形表示
			collection[6] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efWaveForm*)exdata;
				return {
					{ "file", u8(ef->file) },
					{ "i1", ef->i1 },
					{ "i2", ef->i2 },
					{ "frame_n", ef->frame_n },
					{ "i4", ef->i4 },
					{ "i5", ef->i5 },
					{ "type", ef->type },
					{ "mode", ef->mode },
					{ "res_w", ef->res_w },
					{ "res_h", ef->res_h },
					{ "space_x", ef->space_x },
					{ "space_y", ef->space_y },
					{ "color", u8(ef->color) },
					{ "i10", ef->i10 },
					{ "mirror", ef->mirror },
				};
			};

			// シーン
			collection[7] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efScene*)exdata;
				return {
					{ "scene", ef->scene },
				};
			};

			// シーン(音声)
			collection[8] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efSceneAudio*)exdata;
				return {
					{ "scene", ef->scene },
				};
			};

			// 直前オブジェクト
			collection[9] = [](void* exdata) -> n_json
			{
				return {};
			};

			// 標準描画
			collection[10] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efNormalDraw*)exdata;
				return {
					{ "blend", ef->blend },
				};
			};

			// 拡張描画
			collection[11] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efExtendedDraw*)exdata;
				return {
					{ "blend", ef->blend },
				};
			};

			// 標準再生
			collection[12] = [](void* exdata) -> n_json
			{
				return {};
			};

			// パーティクル出力
			collection[13] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efParticle*)exdata;
				return {
					{ "blend", ef->blend },
				};
			};

			// シーンチェンジ
			collection[14] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efSceneChange*)exdata;
				return {
					{ "type", ef->type },
					{ "filter", ef->filter },
					{ "name", u8(ef->name) },
					{ "param", u8(ef->param) },
				};
			};

			// 色調補正
			collection[15] =
			collection[16] = [](void* exdata) -> n_json
			{
				return {};
			};

			// クリッピング
			collection[17] = [](void* exdata) -> n_json
			{
				return {};
			};

			// ぼかし
			collection[18] =
			collection[20] = [](void* exdata) -> n_json
			{
				return {};
			};

			// 境界ぼかし
			collection[19] = [](void* exdata) -> n_json
			{
				return {};
			};

			// モザイク
			collection[21] =
			collection[22] = [](void* exdata) -> n_json
			{
				return {};
			};

			// 発光
			collection[23] =
			collection[24] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efLightEmission*)exdata;
				return {
					{ "color", u8(ef->color) },
					{ "no_color", ef->color.no_color },
				};
			};

			// 閃光
			collection[25] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efFlash*)exdata;
				return {
					{ "color", u8(ef->color) },
					{ "no_color", ef->color.no_color },
					{ "mode", ef->mode },
				};
			};

			// 拡散光
			collection[26] =
			collection[27] = [](void* exdata) -> n_json
			{
				return {};
			};

			// グロー
			collection[28] =
			collection[29] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efGlow*)exdata;
				return {
					{ "color", u8(ef->color) },
					{ "no_color", ef->color.no_color },
					{ "type", ef->type },
				};
			};

			// クロマキー
			collection[30] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efChromakey*)exdata;
				return {
					{ "color", to_json(ef->color) },
					{ "status", ef->status },
				};
			};

			// カラーキー
			collection[31] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efColorKey*)exdata;
				return {
					{ "color", to_json(ef->color) },
					{ "status", ef->status },
				};
			};

			// ルミナンスキー
			collection[32] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efLuminanskey*)exdata;
				return {
					{ "type", ef->type },
				};
			};

			// ライト
			collection[33] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efLight*)exdata;
				return {
					{ "color", u8(ef->color) },
				};
			};

			// シャドー
			collection[34] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efShadow*)exdata;
				return {
					{ "color", u8(ef->color) },
					{ "file", u8(ef->file) },
				};
			};

			// 縁取り
			collection[35] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efBorder*)exdata;
				return {
					{ "color", u8(ef->color) },
					{ "file", u8(ef->file) },
				};
			};

			// 凸エッジ
			collection[36] = [](void* exdata) -> n_json
			{
				return {};
			};

			// エッジ抽出
			collection[37] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efExtractEdge*)exdata;
				return {
					{ "color", u8(ef->color) },
				};
			};

			// シャープ
			collection[38] = [](void* exdata) -> n_json
			{
				return {};
			};

			// フェード
			collection[39] = [](void* exdata) -> n_json
			{
				return {};
			};

			// ワイプ
			collection[40] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efWipe*)exdata;
				return {
					{ "type", ef->type },
//					{ "name", u8(ef->name) },
				};
			};

			// マスク
			collection[41] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efMask*)exdata;
				return {
					{ "type", ef->type },
					{ "name", u8(ef->name) },
					{ "mode", ef->mode },
				};
			};

			// 斜めクリッピング
			collection[42] = [](void* exdata) -> n_json { return {}; };

			// 放射ブラー
			collection[43] =
			collection[44] = [](void* exdata) -> n_json { return {}; };

			// 方向ブラー
			collection[45] =
			collection[46] = [](void* exdata) -> n_json { return {}; };

			// レンズブラー
			collection[47] =
			collection[48] = [](void* exdata) -> n_json { return {}; };

			// モーションブラー
			collection[49] =
			collection[50] = [](void* exdata) -> n_json { return {}; };

			// 座標
			collection[51] = [](void* exdata) -> n_json { return {}; };

			// 拡大率
			collection[52] = [](void* exdata) -> n_json { return {}; };

			// 透明度
			collection[53] = [](void* exdata) -> n_json { return {}; };

			// 回転
			collection[54] = [](void* exdata) -> n_json { return {}; };

			// 領域拡張
			collection[55] = [](void* exdata) -> n_json { return {}; };

			// リサイズ
			collection[56] = [](void* exdata) -> n_json { return {}; };

			// ローテーション
			collection[57] = [](void* exdata) -> n_json { return {}; };

			// 振動
			collection[58] = 
			collection[59] = [](void* exdata) -> n_json { return {}; };

			// 反転
			collection[60] = 
			collection[61] = [](void* exdata) -> n_json { return {}; };

			// ミラー
			collection[62] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efMirror*)exdata;
				return {
					{ "type", ef->type },
//					{ "name", u8(ef->name) },
				};
			};

			// ラスター
			collection[63] = 
			collection[64] = [](void* exdata) -> n_json { return {}; };

			// 波紋
			collection[65] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efRipple*)exdata;
				return {
					{ "num", ef->num },
					{ "interval", ef->interval },
					{ "add", ef->add },
				};
			};

			// 画像ループ
			collection[66] = 
			collection[67] = [](void* exdata) -> n_json { return {}; };

			// 極座標変換
			collection[68] = [](void* exdata) -> n_json { return {}; };

			// ディスプレイスメントマップ
			collection[69] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efDisplacementMap*)exdata;
				return {
					{ "type", ef->type },
					{ "name", u8(ef->name) },
					{ "mode", ef->mode },
					{ "calc", ef->calc },
				};
			};

			// ノイズ
			collection[70] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efNoise*)exdata;
				return {
					{ "type", ef->type },
					{ "mode", ef->mode },
					{ "seed", ef->seed },
				};
			};

			// 色ずれ
			collection[71] =
			collection[72] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efColorDrift*)exdata;
				return {
					{ "type", ef->type },
				};
			};

			// 単色化
			collection[73] =
			collection[74] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efMonochromatic*)exdata;
				return {
					{ "color", u8(ef->color) },
				};
			};

			// グラデーション
			collection[75] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efGradation*)exdata;
				return {
					{ "blend", ef->blend },
					{ "color", u8(ef->color) },
					{ "no_color", ef->color.no_color },
					{ "color2", u8(ef->color2) },
					{ "no_color2", ef->color2.no_color },
					{ "type", ef->type },
				};
			};

			// 拡張色設定
			collection[76] =
			collection[77] = [](void* exdata) -> n_json { return {}; };

			// 特定色域変換
			collection[78] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efSpecialColorConversion*)exdata;
				return {
					{ "color_yc", to_json(ef->color_yc) },
					{ "color_yc2", to_json(ef->color_yc2) },
				};
			};

			// アニメーション効果
			collection[79] =
			// カスタムオブジェクト
			collection[80] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efAnimationEffect*)exdata;
				return {
					{ "type", ef->type },
					{ "filter", ef->filter },
					{ "name", u8(ef->name) },
					{ "param", u8(ef->param) },
				};
			};

			// スクリプト制御
			collection[81] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efScriptControl*)exdata;
				return {
					{ "text", u8(ef->text) },
				};
			};

			// 動画ファイル合成
			collection[82] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efMovieSynthesis*)exdata;
				return {
					{ "file", u8(ef->file) },
					{ "i1", ef->i1 },
					{ "i2", ef->i2 },
					{ "frame_n", ef->frame_n },
					{ "video_rate", ef->video_rate },
					{ "video_scale", ef->video_scale },
					{ "i3", ef->i3 },
					{ "mode", ef->mode },
				};
			};

			// 画像ファイル合成
			collection[83] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efImageSynthesis*)exdata;
				return {
					{ "mode", ef->mode },
					{ "file", u8(ef->file) },
				};
			};

			// インターレース解除
			collection[84] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efDeinterlacing*)exdata;
				return {
					{ "type", ef->type },
				};
			};

			// カメラ制御オプション
			collection[85] = [](void* exdata) -> n_json { return {}; };

			// オフスクリーン描画
			collection[86] = [](void* exdata) -> n_json { return {}; };

			// オブジェクト分割
			collection[87] = [](void* exdata) -> n_json { return {}; };

			// 部分フィルタ
			collection[88] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efPortionFilter*)exdata;
				return {
					{ "type", ef->type },
					{ "name", u8(ef->name) },
				};
			};

			// 音量フェード
			collection[89] = [](void* exdata) -> n_json { return {}; };

			// 音声ディレイ
			collection[90] =
			collection[91] = [](void* exdata) -> n_json { return {}; };

			// モノラル化
			collection[92] = [](void* exdata) -> n_json { return {}; };

			// 時間制御
			collection[93] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efTimeControl*)exdata;
				return {
					{ "range", ef->range },
					{ "u0", ef->unknown[0] },
					{ "u1", ef->unknown[1] },
					{ "u2", ef->unknown[2] },
					{ "u3", ef->unknown[3] },
				};
			};

			// グループ制御
			collection[94] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efGroupControl*)exdata;
				return {
					{ "range", ef->range },
					{ "u0", ef->unknown[0] },
					{ "u1", ef->unknown[1] },
					{ "u2", ef->unknown[2] },
					{ "u3", ef->unknown[3] },
				};
			};

			// カメラ制御
			collection[95] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efCameraControl*)exdata;
				return {
					{ "range", ef->range },
					{ "u0", ef->unknown[0] },
					{ "u1", ef->unknown[1] },
					{ "u2", ef->unknown[2] },
					{ "u3", ef->unknown[3] },
				};
			};

			// カメラ制御(拡張描画)
			collection[96] = [](void* exdata) -> n_json
			{
				auto ef = (ExEdit::Exdata::efCameraControlExDraw*)exdata;
				return {
					{ "target", ef->target },
				};
			};

			// カメラ効果
			collection[97] = [](void* exdata) -> n_json { return {}; };

			// シャドー(カメラ制御)
			collection[98] = [](void* exdata) -> n_json { return {}; };

			// スクリプト(カメラ制御)
			collection[99] = [](void* exdata) -> n_json { return {}; };
		}

		//
		// フィルタ毎の拡張データからノードを作成して返します。
		//
		n_json operator()(int32_t filter_id, void* exdata)
		{
			MY_TRACE_FUNC("");

			auto it = collection.find(filter_id);
			if (it == collection.end()) return {};
			return it->second(exdata);
		}
	} exdata_node_creator;
}
