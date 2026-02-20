#pragma once

namespace apn::timeline_map::main_thread
{
	//
	// このクラスはメインスレッドのコンテキストです。
	// 描画やクリック処理の瞬間にだけ作成され、使用されます。
	//
	struct context_t
	{
		//
		// このクラスは矩形データを保持します。
		//
		struct rect_t : RECT
		{
			int32_t w, h;

			void operator=(const RECT& rc)
			{
				*(RECT*)this = rc;
				w = my::get_width(rc);
				h = my::get_height(rc);
			}
		};

		//
		// レイアウト矩形です。
		//
		struct rects_t {
			rect_t client, layer, time, body;
		} rects = {};

		//
		// 描画するフレーム、レイヤー、オブジェクトの数です。
		//
		int32_t nb_frames {}, nb_layers {}, nb_sorted_objects {};

		//
		// 操作対象のウィンドウです。
		//
		HWND hwnd = {};

		// 以下の変数はレイアウトが決定した後にしか算出できません。

		//
		// テキストを描画するとき使用するフラグです。
		// これはプロパティの値から算出されます。
		//
		UINT text_flags = {};

		//
		// テキストを描画するとき使用するフォーマットです。
		//
		ComPtr<IDWriteTextFormat> text_format = {};

		//
		// レイヤー上部のスペースの高さです。
		//
		float layer_top_space = {};

		//
		// 中間点を表す図形です。
		//
		ComPtr<ID2D1PathGeometry> midpt_path = {};

		//
		// コンストラクタです。
		//
		context_t(HWND hwnd)
			: hwnd(hwnd)
		{
			// ステートを初期化します。
			if (!state.init(hwnd)) return;

			// レイアウト矩形を初期化します。
			{
				// レイヤーボタンの幅です。可変にすることも可能です。
				auto layer_width = 0;

				// 時間目盛りの高さです。可変にすることも可能です。
				auto time_height = 0;

				// クライアント矩形をセットします。
				rects.client = my::get_client_rect(hwnd);

				// タイムライン左側のレイヤーボタン領域の矩形をセットします。
				rects.layer = RECT {
					rects.client.left,
					rects.client.top + time_height,
					rects.client.left + layer_width,
					rects.client.bottom,
				};

				// タイムライン上側の時間目盛り領域の矩形をセットします。
				rects.time = RECT {
					rects.client.left + layer_width,
					rects.client.top,
					rects.client.right,
					rects.client.top + time_height,
				};

				// タイムライン本体部分の矩形をセットします。
				rects.body = RECT {
					rects.client.left + layer_width,
					rects.client.top + time_height,
					rects.client.right,
					rects.client.bottom,
				};
			}

			// 描画するフレーム、レイヤー、オブジェクトの数をセットします。
			{
				nb_frames = 1;
				nb_layers = 100;
				nb_sorted_objects = magi.exin.get_sorted_object_count();

				if (nb_sorted_objects > 0)
				{
					// 実際のフレーム数とレイヤー数を取得します。
					nb_frames = magi.exin.get_aviutl_frame_number();
					nb_layers = magi.exin.get_sorted_object(nb_sorted_objects - 1)->layer_set + 1;
				}
			}

			// レイヤーの高さを算出します。
			auto layer_height = layer_to_pixel(1) - layer_to_pixel(0);

			// レイヤー上部のスペースの高さを算出します。
			layer_top_space = layer_height * property.layer.top_space / 100.0f;

			// 中間点図形用のパスを作成します。
			{
				state.d2d_factory->CreatePathGeometry(midpt_path.ReleaseAndGetAddressOf());
				if (!midpt_path) return;

				ComPtr<ID2D1GeometrySink> sink;
				midpt_path->Open(&sink);
				if (!sink) return;

				sink->BeginFigure(D2D1::Point2F(0, layer_top_space), D2D1_FIGURE_BEGIN_FILLED);
				sink->AddLine(D2D1::Point2F(-layer_top_space, 0));
				sink->AddLine(D2D1::Point2F(+layer_top_space, 0));
				sink->EndFigure(D2D1_FIGURE_END_CLOSED);

				sink->Close();
			}

			// テキスト関連の変数を初期化します。
			{
				// フォント名を取得します。
				auto font_name = property.text.font_name;

				// フォントサイズを算出します。
				auto font_size = property.text.font_size ?
					(property.text.font_size / 100.0f) : (layer_height / 2.0f);

				// テキストフラグを取得します。
				text_flags = 0; {
					switch (property.text.horz_align) {
					case property.text.c_horz_align.c_left: text_flags |= DT_LEFT; break;
					case property.text.c_horz_align.c_right: text_flags |= DT_RIGHT; break;
					case property.text.c_horz_align.c_center: text_flags |= DT_CENTER; break;
					}

					switch (property.text.vert_align) {
					case property.text.c_vert_align.c_top: text_flags |= DT_TOP; break;
					case property.text.c_vert_align.c_bottom: text_flags |= DT_BOTTOM; break;
					case property.text.c_vert_align.c_center: text_flags |= DT_VCENTER; break;
					}
				}

				// テキストフォーマットを作成します。
				text_format = state.create_text_format(font_name, font_size, text_flags);
			}
		}

		//
		// 初期化が正常に終了している場合はTRUEを返します。
		//
		inline BOOL is_initialized() const { return !!text_format; }

		//
		// フレーム番号をピクセル座標に変換して返します。
		//
		inline float frame_to_pixel(int32_t frame)
		{
			return (float)rects.body.w * frame / nb_frames + rects.body.left;
		}

		//
		// レイヤー番号をピクセル座標に変換して返します。
		//
		inline float layer_to_pixel(int32_t layer)
		{
			return (float)rects.body.h * layer / nb_layers + rects.body.top;
		}

		//
		// ピクセル座標をフレーム番号に変換して返します。
		//
		inline int32_t pixel_to_frame(int32_t x)
		{
			if (rects.body.w <= 0) return 0;
			if (x <= rects.body.left) return 0;
			return (x - rects.body.left) * nb_frames / rects.body.w;
		}

		//
		// ピクセル座標をレイヤー番号に変換して返します。
		//
		inline int32_t pixel_to_layer(int32_t y)
		{
			if (rects.body.h <= 0) return 0;
			if (y <= rects.body.top) return 0;
			return (y - rects.body.top) * nb_layers / rects.body.h;
		}

		//
		// 指定されたオブジェクトに対応するアイテムプロパティを返します。
		//
		inline auto& get_item_prop(auto object)
		{
			// Controlが指定されている場合は制御オブジェクトです。
			if (!!(object->flag & ExEdit::Object::Flag::Control))
				return property.item.nodes[property.item.c_item.c_control];

			// Soundが指定されている場合は音声オブジェクトです。
			if (!!(object->flag & ExEdit::Object::Flag::Sound))
			{
				// Filterが指定されている場合はメディアオブジェクトです。
				if (!!(object->flag & ExEdit::Object::Flag::Filter))
					return property.item.nodes[property.item.c_item.c_sound_media];

				// それ以外の場合はフィルタオブジェクトです。
				return property.item.nodes[property.item.c_item.c_sound_filter];
			}
			// それ以外の場合は画像オブジェクトです。
			else
			{
				// Effectが指定されている場合はフィルタ効果オブジェクトです。
				if (!!(object->flag & ExEdit::Object::Flag::Effect))
					return property.item.nodes[property.item.c_item.c_image_filter_effect];

				// Filterが指定されている場合はメディアオブジェクトです。
				if (!!(object->flag & ExEdit::Object::Flag::Filter))
					return property.item.nodes[property.item.c_item.c_image_media];

				// それ以外の場合はフィルタオブジェクトです。
				return property.item.nodes[property.item.c_item.c_image_filter];
			}
		}

		//
		// 指定されたオブジェクトの表示名を返します。
		//
		inline std::wstring get_display_name(auto object)
		{
			auto display_name = my::ws(object->dispname);
			if (display_name.length()) return display_name;

			switch (object->filter_param[0].id)
			{
			case 0: return L"動画ファイル";
			case 1: return L"画像ファイル";
			case 2: return L"音声ファイル";
			case 3: return L"テキスト";
			case 4: return L"図形";
			case 5: return L"フレームバッファ";
			case 6: return L"音声波形表示";
			case 7: return L"シーン";
			case 8: return L"シーン(音声)";
			case 9: return L"直前オブジェクト";
			case 10: return L"標準描画";
			case 11: return L"拡張描画";
			case 12: return L"標準再生";
			case 13: return L"パーティクル出力";
			case 14: return L"シーンチェンジ";
			case 15: return L"色調補正";
			case 16: return L"色調補正";
			case 17: return L"クリッピング";
			case 18: return L"ぼかし";
			case 19: return L"境界ぼかし";
			case 20: return L"ぼかし";
			case 21: return L"モザイク";
			case 22: return L"モザイク";
			case 23: return L"発光";
			case 24: return L"発光";
			case 25: return L"閃光";
			case 26: return L"拡散光";
			case 27: return L"拡散光";
			case 28: return L"グロー";
			case 29: return L"グロー";
			case 30: return L"クロマキー";
			case 31: return L"カラーキー";
			case 32: return L"ルミナンスキー";
			case 33: return L"ライト";
			case 34: return L"シャドー";
			case 35: return L"縁取り";
			case 36: return L"凸エッジ";
			case 37: return L"エッジ抽出";
			case 38: return L"シャープ";
			case 39: return L"フェード";
			case 40: return L"ワイプ";
			case 41: return L"マスク";
			case 42: return L"斜めクリッピング";
			case 43: return L"放射ブラー";
			case 44: return L"放射ブラー";
			case 45: return L"方向ブラー";
			case 46: return L"方向ブラー";
			case 47: return L"レンズブラー";
			case 48: return L"レンズブラー";
			case 49: return L"モーションブラー";
			case 50: return L"モーションブラー";
			case 51: return L"座標";
			case 52: return L"拡大率";
			case 53: return L"透明度";
			case 54: return L"回転";
			case 55: return L"領域拡張";
			case 56: return L"リサイズ";
			case 57: return L"ローテーション";
			case 58: return L"振動";
			case 59: return L"振動";
			case 60: return L"反転";
			case 61: return L"反転";
			case 62: return L"ミラー";
			case 63: return L"ラスター";
			case 64: return L"ラスター";
			case 65: return L"波紋";
			case 66: return L"画像ループ";
			case 67: return L"画像ループ";
			case 68: return L"極座標変換";
			case 69: return L"ディスプレイスメントマップ";
			case 70: return L"ノイズ";
			case 71: return L"色ずれ";
			case 72: return L"色ずれ";
			case 73: return L"単色化";
			case 74: return L"単色化";
			case 75: return L"グラデーション";
			case 76: return L"拡張色設定";
			case 77: return L"拡張色設定";
			case 78: return L"特定色域変換";
			case 79: return L"アニメーション効果";
			case 80: return L"カスタムオブジェクト";
			case 81: return L"スクリプト制御";
			case 82: return L"動画ファイル合成";
			case 83: return L"画像ファイル合成";
			case 84: return L"インターレース解除";
			case 85: return L"カメラ制御オプション";
			case 86: return L"オフスクリーン描画";
			case 87: return L"オブジェクト分割";
			case 88: return L"部分フィルタ";
			case 89: return L"音量フェード";
			case 90: return L"音声ディレイ";
			case 91: return L"音声ディレイ";
			case 92: return L"モノラル化";
			case 93: return L"時間制御";
			case 94: return L"グループ制御";
			case 95: return L"カメラ制御";
			case 96: return L"カメラ制御(拡張描画)";
			case 97: return L"カメラ効果";
			case 98: return L"シャドー(カメラ制御)";
			case 99: return L"スクリプト(カメラ制御)";
			}

			return L"不明";
		}

		//
		// 矩形を指定された値の分だけ縮めて返します。
		//
		inline static D2D1_RECT_F deflate(const D2D1_RECT_F& rc, float x, float y)
		{
			return { rc.left + x, rc.top + y, rc.right - x, rc.bottom - y };
		}

		//
		// 矩形を指定された値の分だけ縮めて返します。
		//
		inline static D2D1_RECT_F deflate(const D2D1_RECT_F& rc, float value)
		{
			return deflate(rc, value, value);
		}

		//
		// 丸角矩形を指定された値の分だけ縮めて返します。
		//
		inline static D2D1_ROUNDED_RECT deflate(const D2D1_ROUNDED_RECT& rc, float x, float y)
		{
			return {
				{ rc.rect.left + x, rc.rect.top + y, rc.rect.right - x, rc.rect.bottom - y },
				rc.radiusX - x, rc.radiusY - y,
			};
		}

		//
		// 丸角矩形を指定された値の分だけ縮めて返します。
		//
		inline static D2D1_ROUNDED_RECT deflate(const D2D1_ROUNDED_RECT& rc, float value)
		{
			return deflate(rc, value, value);
		}

		//
		// 丸角矩形に角丸めモードを適用して返します。
		//
		inline static D2D1_ROUNDED_RECT round(const D2D1_ROUNDED_RECT& _rc, int32_t round_mode)
		{
			auto rc = _rc;

			auto w = rc.rect.right - rc.rect.left;
			auto h = rc.rect.bottom - rc.rect.top;

			switch (round_mode)
			{
			case property.c_round_mode.c_none:
				{
					// 角丸めを無効化します。
					rc.radiusX = 0.0f;
					rc.radiusY = 0.0f;

					break;
				}
			case property.c_round_mode.c_absolute:
				{
					// 何もしません。

					break;
				}
			case property.c_round_mode.c_relative:
				{
					auto size = std::min(w, h);

					// 角丸めを最小幅に対する比率にします。
					rc.radiusX *= size;
					rc.radiusY *= size;

					break;
				}
			case property.c_round_mode.c_discrete:
				{
					// 角丸めを各幅に対する比率にします。
					rc.radiusX *= h;
					rc.radiusY *= w;

					break;
				}
			}

			return rc;
		}

		//
		// 矩形を描画します。
		//
		BOOL draw_rectangle(const D2D1_RECT_F& rc,
			ID2D1Brush* fill_brush, ID2D1Brush* stroke_brush, float stroke_width)
		{
			// 矩形を塗りつぶします。
			state.render_target->FillRectangle(rc, fill_brush);

			// 縁の幅が有効の場合は
			if (stroke_width > 0.0f)
			{
				// 矩形の縁を描画します。
				state.render_target->DrawRectangle(
					deflate(rc, stroke_width / 2.0f), stroke_brush, stroke_width);
			}

			return TRUE;
		}

		//
		// 角丸矩形を描画します。
		//
		BOOL draw_rounded_rectangle(const D2D1_ROUNDED_RECT& rc,
			ID2D1Brush* fill_brush, ID2D1Brush* stroke_brush, float stroke_width)
		{
			// 丸め半径が無効の場合は
			if (rc.radiusX <= 0.0f && rc.radiusY <= 0.0f)
			{
				// 普通の矩形を描画します。
				return draw_rectangle(rc.rect, fill_brush, stroke_brush, stroke_width);
			}

			// 角丸矩形を塗りつぶします。
			state.render_target->FillRoundedRectangle(rc, fill_brush);

			// 縁の幅が有効の場合は
			if (stroke_width > 0.0f)
			{
				// 角丸矩形の縁を描画します。
				state.render_target->DrawRoundedRectangle(
					deflate(rc, stroke_width / 2.0f), stroke_brush, stroke_width);
			}

			return TRUE;
		}

		//
		// 文字列を描画します。
		//
		BOOL draw_text(const std::wstring& text, const D2D1_RECT_F& item_rc_, UINT text_flags,
			IDWriteTextFormat* text_format, ID2D1Brush* text_brush, ID2D1Brush* text_shadow_brush)
		{
			// アイテム枠の分だけ矩形を縮めます。
			auto item_rc = deflate(item_rc_, property.item.stroke_width / 100.0f);

			// パディングの分だけ矩形を縮めます。
			item_rc = deflate(item_rc, property.text.padding.x / 100.f, property.text.padding.y / 100.f);

			// 座標を取得します。
			auto x = item_rc.left;
			auto y = item_rc.top;
			auto w = item_rc.right - item_rc.left;
			auto h = item_rc.bottom - item_rc.top;

			// テキストレイアウトを作成します。
			auto text_layout = state.create_text_layout(text, text_flags, text_format, w, h);
			if (!text_layout) return FALSE;

			// 影を描画する場合は
			if (property.text.shadow_offset.x > 0 ||
				property.text.shadow_offset.y > 0)
			{
				auto shadow_offset = D2D1::Point2F(
					property.text.shadow_offset.x / 100.f,
					property.text.shadow_offset.y / 100.f);

				// 影を描画します。
				state.render_target->DrawTextLayout(
					D2D1::Point2F(x + shadow_offset.x, y + shadow_offset.y),
					text_layout.Get(),
					text_shadow_brush,
					D2D1_DRAW_TEXT_OPTIONS_CLIP);
			}

			// テキストを描画します。
			state.render_target->DrawTextLayout(
				D2D1::Point2F(x, y),
				text_layout.Get(),
				text_brush,
				D2D1_DRAW_TEXT_OPTIONS_CLIP |
				D2D1_DRAW_TEXT_OPTIONS_ENABLE_COLOR_FONT);

			return TRUE;
		}

		//
		// 各レイヤーを描画します。
		//
		BOOL draw_layers()
		{
			// レイヤーを走査します。
			for (decltype(nb_layers) i = 0; i < nb_layers; i++)
			{
				// レイヤー矩形を作成します。
				auto layer_rc = D2D1::RectF(
					(float)rects.body.left, layer_to_pixel(i),
					(float)rects.body.right, layer_to_pixel(i + 1));

				// レイヤー矩形を描画します。
				state.render_target->FillRectangle(layer_rc,
					(i % 2) ? state.layer.even_brush.Get() : state.layer.odd_brush.Get());
			}

			return TRUE;
		}

		//
		// 各アイテムを描画します。
		//
		BOOL draw_items()
		{
			// 基本となる角丸め半径を算出します。
			auto base_radius = property.item.round_size / 100.0f;

			// アイテムを走査します。
			for (decltype(nb_sorted_objects) i = 0; i < nb_sorted_objects; i++)
			{
				// オブジェクトを取得します。
				auto object = magi.exin.get_sorted_object(i);
				auto object_index = magi.exin.get_object_index(object);

				// アイテムの追加情報を取得します。
				auto item_prop = get_item_prop(object);

				// アイテムの描画位置を取得します。
				auto start_point = D2D1::Point2F(frame_to_pixel(object->frame_begin), layer_to_pixel(object->layer_set));
				auto end_point = D2D1::Point2F(frame_to_pixel(object->frame_end + 1), layer_to_pixel(object->layer_set+ 1));

				// アイテム矩形を仮作成します。
				auto item_rc = D2D1::RoundedRect(
					D2D1::RectF(start_point.x, start_point.y, end_point.x, end_point.y),
					base_radius, base_radius);

				// アイテム矩形をレイヤー上部のスペース分だけ縮めます。
				item_rc.rect.top += layer_top_space;

				// 中間点の描画位置です。
				std::vector<float> midpt_x_vec;

				// 中間点を処理します。
				{
					// 次のオブジェクトを取得します。
					auto next_object = magi.exin.get_sorted_object(i + 1);

					// 次のオブジェクトが中間点の場合は
					while (next_object->index_midpt_leader == object_index)
					{
						// 中間点の描画位置を取得しておきます。
						midpt_x_vec.emplace_back(item_rc.rect.right);

						// アイテムの終了位置を再設定します。
						item_rc.rect.right = frame_to_pixel(next_object->frame_end + 1);

						// 次のオブジェクトを取得します。
						next_object = magi.exin.get_sorted_object(++i + 1);
					}
				}

				// アイテム矩形に角丸めモードを適用します。
				item_rc = round(item_rc, property.item.round_mode);

				// 制御範囲を描画します。
				{
					switch (object->filter_param[0].id)
					{
					case 93:
					case 94:
					case 95:
						{
							// レイヤー設定を取得します。
							auto layer_setting = magi.exin.get_layer_setting(object->layer_set);
							if (!layer_setting) break;

							// アイテムが非表示レイヤーにある場合は除外します。
							if (has_flag(layer_setting->flag, ExEdit::LayerSetting::Flag::UnDisp)) break;

							// 拡張データ内の制御範囲を取得します。
							auto control_range = *(int32_t*)magi.exin.get_exdata(object, 0);

							// 下辺のレイヤーを取得します。
							auto bottom_layer = (control_range > 0) ?
								std::min(object->layer_set + 1 + control_range, 100) : 100;

							// 下辺を取得します。
							auto bottom = layer_to_pixel(bottom_layer);

							// 制御範囲矩形を取得します。
							auto control_range_rc = item_rc;
							control_range_rc.rect.bottom = bottom;

							// 制御範囲矩形を描画します。
							draw_rounded_rectangle(control_range_rc, state.control_range_brush.Get(),
								state.control_range_stroke_brush.Get(), property.control_range.stroke_width / 100.0f);

							break;
						}
					}
				}

				// アイテム矩形を描画します。
				if (0)
				{
					// アイテム用のブラシを作成します。
					auto item_fill_brush = state.create_gradient_brush(
						item_prop.start_color, item_prop.end_color,
						D2D1::Point2F(item_rc.rect.left, item_rc.rect.top),
						D2D1::Point2F(item_rc.rect.right, item_rc.rect.top));
					if (!item_fill_brush) return FALSE;

					// アイテム矩形を描画します。
					draw_rounded_rectangle(item_rc, item_fill_brush.Get(),
						state.item_stroke_brush.Get(), property.item.stroke_width / 100.0f);
				}

				// 中間点を走査します。
				for (auto midpt_x : midpt_x_vec)
				{
					// (midpt_x, start_point.y) の位置に中間点図形を描画します。
					state.render_target->SetTransform(D2D1::Matrix3x2F::Translation(midpt_x, start_point.y));
					state.render_target->FillGeometry(midpt_path.Get(), state.midpt_brush.Get());
					state.render_target->SetTransform(D2D1::Matrix3x2F::Identity());

					// 中間点の線幅が有効の場合は
					if (property.midpt.line_width > 0)
					{
						// 中間点の位置に縦線を描画します。
						state.render_target->DrawLine(
							D2D1::Point2F(midpt_x, item_rc.rect.top),
							D2D1::Point2F(midpt_x, item_rc.rect.bottom),
							state.midpt_line_brush.Get(),
							property.midpt.line_width / 100.0f);
					}
				}

				// アイテム名を描画します。
//				if (0)
				{
					// アイテムの表示名を取得します。
					auto text = get_display_name(object);
					{
						static const std::wregex re(L"\r\n");
						static const std::wstring fmt = L"\\n";

						// 改行を置換します。
						text = std::regex_replace(text, re, fmt);
					}

					// アイテム名を描画します。
					draw_text(text, item_rc.rect, text_flags, text_format.Get(),
						state.text_brush.Get(), state.text_shadow_brush.Get());
				}
			}

			return TRUE;
		}

		//
		// 各レイヤー設定を描画します。
		//
		BOOL draw_layer_settings()
		{
			// レイヤーを走査します。
			for (decltype(nb_layers) i = 0; i < nb_layers; i++)
			{
				// レイヤー設定を取得します。
				auto layer_setting = magi.exin.get_layer_setting(i);
				if (!layer_setting) continue;

				// レイヤー矩形を取得します。
				auto layer_rc = D2D1::RectF(
					(float)rects.body.left, layer_to_pixel(i),
					(float)rects.body.right, layer_to_pixel(i + 1));

				if (has_flag(layer_setting->flag, ExEdit::LayerSetting::Flag::UnDisp))
				{
					// 非表示レイヤー矩形を描画します。
					draw_rectangle(layer_rc, state.layer.undisp_brush.Get(),
						state.layer.undisp_stroke_brush.Get(), property.layer.undisp_stroke_width / 100.0f);
				}

				auto y = layer_rc.top;

				const auto draw_setting = [&](auto flag, const auto& brush, float stroke_width)
				{
					// 指定されたフラグがレイヤーに設定されている場合は
					if (has_flag(layer_setting->flag, flag))
					{
						// 設定を横線として描画します。
						state.render_target->DrawLine(
							D2D1::Point2F((float)rects.body.left, y + stroke_width / 2.0f),
							D2D1::Point2F((float)rects.body.right, y + stroke_width / 2.0f),
							brush, stroke_width);

						y += stroke_width;
					}
				};

				draw_setting(ExEdit::LayerSetting::Flag::Locked, state.layer.locked_stroke_brush.Get(), property.layer.locked_stroke_width / 100.0f);
				draw_setting(ExEdit::LayerSetting::Flag::CoordLink, state.layer.coordlink_stroke_brush.Get(), property.layer.coordlink_stroke_width / 100.0f);
				draw_setting(ExEdit::LayerSetting::Flag::Clip, state.layer.clip_stroke_brush.Get(), property.layer.clip_stroke_width / 100.0f);
			}

			return TRUE;
		}

		//
		// 現在フレームを描画します。
		//
		BOOL draw_current_frame()
		{
			// 現在フレームを取得します。
			auto current_frame = magi.exin.get_exedit_current_frame();

			// 現在フレームのX座標を取得します。
			auto x = frame_to_pixel(current_frame);

			// 現在フレームを縦線として描画します。
			state.render_target->DrawLine(
				D2D1::Point2F(x, (float)rects.client.top),
				D2D1::Point2F(x, (float)rects.client.bottom),
				state.current_frame_brush.Get(),
				property.current_frame.line_width / 100.0f);

			return TRUE;
		}

		//
		// 表示範囲を描画します。
		//
		BOOL draw_visible_area()
		{
			// 表示範囲のフレームとレイヤーを取得します。
			auto left_frame = magi.exin.get_top_visible_frame();
			auto right_frame = (int32_t)magi.exin.x_to_frame(magi.exin.get_layer_width());
			auto top_layer = magi.exin.get_top_visible_layer();
			auto bottom_layer = top_layer + magi.exin.get_layer_visible_count();

			// 基本となる角丸め半径を算出します。
			auto base_radius = property.visible_area.round_size / 100.0f;

			// 表示範囲矩形を取得します。
			auto visible_area_rc = D2D1::RoundedRect(
				D2D1::RectF(
					frame_to_pixel(left_frame), layer_to_pixel(top_layer),
					frame_to_pixel(right_frame), layer_to_pixel(bottom_layer)),
				base_radius, base_radius);

			// 表示範囲矩形に角丸めモードを適用します。
			visible_area_rc = round(visible_area_rc, property.visible_area.round_mode);

			// 表示範囲矩形を描画します。
			draw_rounded_rectangle(visible_area_rc, state.visible_area_brush.Get(),
				state.visible_area_stroke_brush.Get(), property.visible_area.stroke_width / 100.0f);

			return TRUE;
		}

		//
		// タイムラインマップを描画します。
		//
		BOOL on_paint()
		{
			// WM_PAINTを処理します。
			my::PaintDC dc(hwnd);

			// 初期化が完了しているか確認します。
			if (!is_initialized()) return FALSE;

			// 描画処理を開始します。
			state.render_target->BeginDraw();
			state.render_target->Clear();

			// 各要素を描画します。
			draw_layers();
			draw_items();
			draw_layer_settings();
			draw_current_frame();
			draw_visible_area();

			// 描画処理を終了します。
			if (state.render_target->EndDraw() == D2DERR_RECREATE_TARGET)
				state.render_target = nullptr;

			return TRUE;
		}

		//
		// WM_SIZEを処理します。
		//
		BOOL on_size()
		{
			// 初期化が完了しているか確認します。
			if (!is_initialized()) return FALSE;

			// レンダーターゲットをリサイズします。
			state.render_target->Resize(
				D2D1::SizeU(rects.client.w, rects.client.h));

			return TRUE;
		}

		//
		// WM_LBUTTONDOWNを処理します。
		//
		BOOL on_click(POINT point)
		{
			// 初期化が完了しているか確認します。
			if (!is_initialized()) return FALSE;

			// タイムラインを水平方向にスクロールします。
			{
				auto left_frame = magi.exin.get_top_visible_frame();
				auto right_frame = (int32_t)magi.exin.x_to_frame(magi.exin.get_layer_width());

				auto frame = pixel_to_frame(point.x);
				frame -= (right_frame - left_frame) / 2;
				magi.exin.set_top_visible_frame(frame);
			}

			// タイムラインを垂直方向にスクロールします。
			{
				auto layer = pixel_to_layer(point.y);
				layer -= (magi.exin.get_layer_visible_count() / 2);
				magi.exin.set_top_visible_layer(layer);
			}

			return TRUE;
		}
	};
}
