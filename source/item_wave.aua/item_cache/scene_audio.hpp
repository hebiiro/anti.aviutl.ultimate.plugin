#pragma once

namespace apn::item_wave::item_cache
{
	//
	// このクラスはシーン(音声)アイテムのキャッシュです。
	//
	struct SceneAudio : Node
	{
		//
		// このクラスはアイテムのプロパティです。
		//
		struct Prop
		{
			int32_t frame_begin = 0;
			int32_t frame_end = 0;
			int32_t scene_set = 0;
			BOOL loop = FALSE;
			BOOL link = FALSE;
			int32_t scene = 0;
			float volume = 1.0f;
			float play_begin = 0.0f;
			float play_speed = 1.0f;

			//
			// 指定されたアイテムのプロパティが等しい場合はTRUEを返します。
			//
			inline friend BOOL operator==(const Prop& lhs, const Prop& rhs)
			{
				return !operator!=(lhs, rhs);
			}

			//
			// 指定されたアイテムのプロパティが異なる場合はTRUEを返します。
			//
			inline friend BOOL operator!=(const Prop& lhs, const Prop& rhs)
			{
				if (lhs.loop != rhs.loop) return TRUE;
				if (lhs.link != rhs.link) return TRUE;
				if (lhs.scene != rhs.scene) return TRUE;
				if (lhs.volume != rhs.volume) return TRUE;
				if (lhs.play_begin != rhs.play_begin) return TRUE;
				if (lhs.play_speed != rhs.play_speed) return TRUE;
				if (lhs.frame_end != rhs.frame_end) return TRUE;

				return FALSE;
			}
		};

		//
		// アイテムのプロパティです。
		//
		std::optional<Prop> prop;

		//
		// 音量が作成済みの場合はTRUEになります。
		//
		BOOL cached = FALSE;

		//
		// 指定されたアイテムのプロパティを返します。
		//
		inline static std::optional<Prop> get_prop(const Context& ctx, int32_t object_index)
		{
			// アイテムを取得します。
			auto object = magi.exin.get_object(object_index);

			// シーン(音声)フィルタのインデックスを取得します。
			auto scene_audio_filter_index = magi.exin.get_filter_index(object, c_scene_audio_filter_id);
			if (scene_audio_filter_index == -1) return std::nullopt;

			// 標準再生フィルタのインデックスを取得します。
			auto standard_play_filter_index = magi.exin.get_filter_index(object, c_standard_play_filter_id);
			if (standard_play_filter_index == -1) return std::nullopt;

			auto volume_index = object->filter_param[standard_play_filter_index].track_begin + 0;
			auto pan_index = object->filter_param[standard_play_filter_index].track_begin + 1;
			auto play_begin_index = object->filter_param[scene_audio_filter_index].track_begin + 0;
			auto play_speed_index = object->filter_param[scene_audio_filter_index].track_begin + 1;

			auto prop = Prop {};
			prop.loop = object->check_value[0];
			prop.link = object->check_value[1];

			// シーン(音声)アイテムがシーンアイテムと連携している場合は
			if (prop.link)
			{
				// シーン(音声)アイテムと連携しているシーンアイテムを取得します。
				auto object2 = get_scene_item(ctx, object);
				if (!object2) return std::nullopt;

				auto play_begin_index = object2->filter_param[0].track_begin + 0;
				auto play_speed_index = object2->filter_param[0].track_begin + 1;

				// シーンアイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efScene*)magi.exin.get_exdata(object2, 0);

				auto play_begin = (float)(object2->track_value_left[play_begin_index] - 1);
				auto fps = (float)hive.current_fi.video_rate / hive.current_fi.video_scale;

				prop.scene = exdata->scene;
				prop.play_begin = play_begin / fps;
				prop.play_speed = object2->track_value_left[play_speed_index] / 1000.0f;
			}
			else
			{
				// シーン(音声)アイテムから拡張データを取得します。
				auto exdata = (ExEdit::Exdata::efSceneAudio*)magi.exin.get_exdata(object, 0);

				prop.scene = exdata->scene;
				prop.play_begin = object->track_value_left[play_begin_index] / 100.0f;
				prop.play_speed = object->track_value_left[play_speed_index] / 1000.0f;
			}

			prop.frame_begin = object->frame_begin;
			prop.frame_end = object->frame_end;
			prop.scene_set = object->scene_set;
			prop.volume = object->track_value_left[volume_index] / 1000.0f;

			return prop;
		}

		//
		// シーン(音声)アイテムと連携しているシーンアイテムを返します。
		//
		inline static ExEdit::Object* get_scene_item(const Context& ctx, ExEdit::Object* object)
		{
			auto result = (ExEdit::Object*)nullptr;
			auto distance = 100;

			// 同じ開始位置で最も近いレイヤーにある動画アイテムを探します。
			for (auto object_index : ctx.object_indexes)
			{
				// アイテムを取得します。
				auto object2 = magi.exin.get_object(object_index);

				// シーンが異なるアイテムは無視します。
				if (object2->scene_set != object->scene_set) continue;

				// 開始位置が異なるアイテムは無視します。
				if (object2->frame_begin != object->frame_begin) continue;

				// レイヤー位置が下のアイテムは無視します。
				if (object2->layer_set >= object->layer_set) continue;

				// レイヤー位置が遠い場合は無視します。
				auto distance2 = object->layer_set - object2->layer_set;
				if (distance2 > distance) continue;
#if 0
				// シーン(音声)アイテムの場合は
				if (object2->filter_param[0].id == c_scene_audio_filter_id)
				{
					// このシーン(音声)アイテムより下にある
					// シーンアイテムのみを連携対象とみなします。
					result = nullptr;
					distance = distance2;
				}
				else
#endif
				{
					// シーン以外のアイテムは無視します。
					if (object2->filter_param[0].id != c_scene_filter_id) continue;

					// このシーンアイテムを連携対象とします。
					result = object2;
					distance = distance2;
				}
			}

			return result;
		}

		//
		// アイテムキャッシュが無効の場合はTRUEを返します。
		//
		virtual BOOL is_invalid(int32_t self_object_index) override
		{
			return !prop;
		}

		//
		// 音量を用意できた場合はTRUEを返します。
		//
		virtual BOOL refresh(Context& ctx, int32_t self_object_index) override
		{
			{
				// アイテムのプロパティを取得します。
				auto prop = get_prop(ctx, self_object_index);

				// アイテムのプロパティに変更がない場合は何もしません。
				if (this->prop == prop) return !!this->prop;

				// アイテムのプロパティを更新します。
				this->prop = prop;

				// アイテムが配置されているレイヤーを再描画予定にします。
				ctx.add_redraw_layer(self_object_index);

				// 音量を消去します。
				volumes.clear();

				// アイテムのプロパティが無効の場合はFALSEを返します。
				if (!this->prop) return FALSE;
			}

			// アイテムのプロパティへの参照を取得します。
			const auto& prop = this->prop.value();

			// 対象シーンのインデックスを取得します。
			auto target_scene_index = prop.scene;

			// 対象シーンの音量を用意できなかった場合は
			if (!core->refresh_scene(ctx, target_scene_index))
			{
				// アイテムキャッシュを用意できなかったので
				// アイテムのプロパティをリセットしてFALSEを返します。
				return this->prop.reset(), FALSE;
			}

			// 対象シーンのアイテムを走査します。
			for (auto object_index : ctx.object_indexes)
			{
				// アイテムを取得します。
				auto object = magi.exin.get_object(object_index);

				// 対象シーン内のアイテムではない場合は何もしません。
				if (object->scene_set != target_scene_index) continue;

				// アイテムが配置されているレイヤーの設定を取得します。
				auto layer_setting = magi.exin.get_layer_setting(object->scene_set, object->layer_set);

				// レイヤーが無効化されている場合は何もしません。
				if (!!(layer_setting->flag & ExEdit::LayerSetting::Flag::UnDisp)) continue;

				// アイテムキャッシュを取得します。
				auto item_cache = core->get(object_index);

				// アイテムキャッシュを取得できなかった場合は何もしません。
				if (!item_cache) continue;

				// アイテムの終了位置を取得します。
				auto end = (size_t)object->frame_end + 1;

				// キャッシュサイズが足りない場合は拡張します。
				if (volumes.size() < end) volumes.resize(end, {});

				// アイテムキャッシュの音量を加算します。
				auto c = std::min(item_cache->volumes.size(), end - object->frame_begin);
				for (decltype(c) i = 0; i < c; i++)
					volumes[i + object->frame_begin].level += item_cache->volumes[i].level;
			}

			// アイテムが配置されているレイヤーを再描画します。
			ctx.add_redraw_layer(self_object_index);

			// アイテムキャッシュを用意できたのでTRUEを返します。
			return TRUE;
		}

		//
		// アイテムキャッシュを準備します。
		//
		virtual BOOL prepare(Context& ctx, int32_t self_object_index) override
		{
			// アイテムのプロパティが無効の場合は何もしません。
			if (!prop) return FALSE;

			// シーン(音声)アイテムがカレントシーンを参照している場合は
			if (magi.exin.get_current_scene_index() == prop.value().scene)
			{
				// アイテムのプロパティをリセットします。
				// これにより、次回更新時にシーン(音声)アイテムの音量が再計算されます。
				prop.reset();
			}

			return TRUE;
		}
	};
}
