#pragma once

namespace apn::settings_browser
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// 選択アイテムの設定からjsonを生成します。
		//
		virtual BOOL generate_settings_json() override
		{
			MY_TRACE_FUNC("");

			auto message_box = [](auto text)
			{
//				hive.message_box(text);
			};

			auto object_index = magi.exin.get_current_object_index();
			if (object_index < 0)
			{
				message_box(L"アイテムを選択してください");

				return FALSE;
			}

			auto object = magi.exin.get_object(object_index);
			if (!object)
			{
				message_box(L"アイテムが無効です");

				return FALSE;
			}

			if (!(object->flag & ExEdit::Object::Flag::Exist))
			{
				message_box(L"アイテムの状態が無効です");

				return FALSE;
			}

			// 選択アイテムからjsonを作成します。
			auto root = create_root_node(object, object_index);

			// jsonを文字列に変換します。
			hive.settings_json = my::cp_to_wide(root.dump(1, '\t'), CP_UTF8);

			// スクリプトにjson文字列を送信します。
			browser.post_web_message_as_json(hive.settings_json);

			return TRUE;
		}

		//
		// htmlファイルを読み込みます。
		//
		virtual BOOL navigate() override
		{
			MY_TRACE_FUNC("");

			::SetTimer(nullptr, 0, 100,
				[](HWND hwnd, UINT message, UINT_PTR timer_id, DWORD time)
			{
				if (browser.navigate(hive.url))
					::KillTimer(nullptr, timer_id);
			});

			return TRUE;
		}

		//
		// ルートノードを作成して返します。
		//
		n_json create_root_node(ExEdit::Object* object, int32_t object_index)
		{
			MY_TRACE_FUNC("");

			return { { "response_settings", create_response_settings_node(object, object_index) } };
		}

		//
		// 設定表示ノードを作成して返します。
		//
		n_json create_response_settings_node(ExEdit::Object* object, int32_t object_index)
		{
			MY_TRACE_FUNC("");

			return {
				{ "midpt", create_midpt_nodes(object, object_index) },
				{ "exdata", create_exdata_nodes(object, object_index) },
			};
		}

		//
		// 中間点ノードを作成して返します。
		//
		n_json create_midpt_nodes(ExEdit::Object* object, int32_t object_index)
		{
			MY_TRACE_FUNC("");

			n_json midpt_nodes;

			// 中間点リーダーのインデックスを取得します。
			auto midpt_leader = object->index_midpt_leader;

			// 中間点リーダーが存在しない場合は
			if (midpt_leader < 0)
			{
				do
				{
					// 中間点ノードを作成します。
					n_json midpt_node = create_midpt_node(midpt_leader, object_index);
					if (midpt_node.is_null()) break;

					// 中間点ノードを追加します。
					midpt_nodes.emplace_back(midpt_node);
				}
				while (false);
			}
			// 中間点リーダーが存在する場合は
			else
			{
				auto midpt_object_index = midpt_leader;

				while (midpt_object_index >= 0)
				{
					// 中間点ノードを作成します。
					n_json midpt_node = create_midpt_node(midpt_leader, midpt_object_index);
					if (midpt_node.is_null()) break;

					// 中間点ノードを追加します。
					midpt_nodes.emplace_back(midpt_node);

					// 次の中間点に進めます。
					midpt_object_index = magi.exin.get_next_object_index(midpt_object_index);
				}
			}

			return midpt_nodes;
		}

		//
		// 中間点ノードを作成して返します。
		//
		n_json create_midpt_node(int32_t midpt_leader, int32_t midpt_object_index)
		{
			MY_TRACE_FUNC("");

			// 中間点オブジェクトを取得します。
			auto midpt_object = magi.exin.get_object(midpt_object_index);
			if (!midpt_object) {};

			// 中間点元が異なる場合は処理を中止します。
			if (midpt_object->index_midpt_leader != midpt_leader) {};

			n_json midpt_node;
			write_string(midpt_node, "flag", my::format(_T("{/hex}"), (uint32_t)midpt_object->flag));
			write_string(midpt_node, "display_name", my::ws(midpt_object->dispname));
			write_int(midpt_node, "index", midpt_object_index);
			write_int(midpt_node, "index_midpt_leader", midpt_object->index_midpt_leader);
			write_int(midpt_node, "frame_begin", midpt_object->frame_begin);
			write_int(midpt_node, "frame_end", midpt_object->frame_end);
			write_int(midpt_node, "layer_set", midpt_object->layer_set);
			write_int(midpt_node, "scene_set", midpt_object->scene_set);
			write_int(midpt_node, "group_belong", midpt_object->group_belong);
			write_child(midpt_node, "filter", create_filter_nodes(midpt_object));
			return midpt_node;
		}

		//
		// フィルタノードを作成して返します。
		//
		n_json create_filter_nodes(ExEdit::Object* midpt_object)
		{
			MY_TRACE_FUNC("");

			n_json filter_nodes;

			// すべてのフィルタを走査します。
			for (int32_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// フィルタパラメータを取得します。
				auto filter_param = midpt_object->filter_param + i;
				if (filter_param->id < 0) break;

				// フィルタを取得します。
				auto filter = magi.exin.get_filter(filter_param->id);
				if (!filter) break;

				n_json filter_node;
				write_int(filter_node, "id", filter_param->id);
				write_string(filter_node, "name", my::ws(filter->name));
				write_string(filter_node, "flag", my::format(_T("{/hex}"), (uint32_t)filter->flag));
				write_child(filter_node, "track", create_track_nodes(midpt_object, filter_param, filter));
				write_child(filter_node, "check", create_check_nodes(midpt_object, filter_param, filter));
				filter_nodes.emplace_back(filter_node);
			}

			return filter_nodes;
		}

		//
		// トラックノードを作成して返します。
		//
		n_json create_track_nodes(ExEdit::Object* midpt_object, ExEdit::Object::FilterParam* filter_param, ExEdit::Filter* filter)
		{
			MY_TRACE_FUNC("");

			n_json track_nodes;

			for (int32_t i = 0; i < filter->track_n; i++)
			{
				if (!filter->track_name ||
					!filter->track_default ||
					!filter->track_s ||
					!filter->track_e)
				{
					MY_TRACE("フィルタ情報を取得できませんでした\n");

					continue;
				}

				auto track_index = filter_param->track_begin + i;
				auto track_scale = filter->track_scale ? filter->track_scale[i] : 1;

				track_nodes.emplace_back(n_json {
					{ "name", u8(filter->track_name[i]) },
					{ "default", u8(filter->track_default[i], track_scale) },
					{ "s", u8(filter->track_s[i], track_scale) },
					{ "e", u8(filter->track_e[i], track_scale) },
					{ "value_left", u8(midpt_object->track_value_left[track_index], track_scale) },
					{ "value_right", u8(midpt_object->track_value_right[track_index], track_scale) },
					{ "param", midpt_object->track_param[track_index] },
				});
			}

			return track_nodes;
		}

		//
		// チェックノードを作成して返します。
		//
		n_json create_check_nodes(ExEdit::Object* midpt_object, ExEdit::Object::FilterParam* filter_param, ExEdit::Filter* filter)
		{
			MY_TRACE_FUNC("");

			n_json check_nodes;

			for (int32_t i = 0; i < filter->check_n; i++)
			{
				auto check_index = filter_param->check_begin + i;

				n_json check_node;
				if (filter->check_name && filter->check_default)
				{
					auto check_default = filter->check_default[i];
					auto check_name = filter->check_name[i];

					write_int(check_node, "default", check_default);

					switch (check_default)
					{
					// コンボボックスの場合は
					case -2:
						{
							n_json name_node;
							auto name = check_name;
							while (*name)
							{
								name_node.emplace_back(u8(name));

								name += ::lstrlenA(name) + 1;
							}
							write_child_node(check_node, "name", name_node);

							break;
						}
					// ボタンの場合は
					case -1:
						{
							write_string(check_node, "name", my::ws(check_name));

							break;
						}
					// チェックボックスの場合は
					default:
						{
							write_string(check_node, "name", my::ws(check_name));

							break;
						}
					}
				}
				write_int(check_node, "value", midpt_object->check_value[check_index]);
				check_nodes.emplace_back(check_node);
			}

			return check_nodes;
		}

		//
		// 拡張データノードを作成して返します。
		//
		n_json create_exdata_nodes(ExEdit::Object* object, int32_t object_index)
		{
			MY_TRACE_FUNC("");

			// 中間点リーダーが存在する場合は
			// 中間点リーダーの拡張データを使用します。
			auto midpt_leader = object->index_midpt_leader;
			if (midpt_leader >= 0)
			{
				object_index = midpt_leader;
				object = magi.exin.get_object(object_index);
			}

			n_json exdata_nodes;

			// すべてのフィルタを走査します。
			for (int32_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// フィルタパラメータを取得します。
				auto filter_param = object->filter_param + i;
				if (filter_param->id < 0) break;

				// フィルタを取得します。
				auto filter = magi.exin.get_filter(filter_param->id);
				if (!filter) break;

				auto exdata = magi.exin.get_exdata(object, i);
				auto exdata_ptr = filter->exdata_ptr; // この変数はここでは使用できません。
				auto exdata_def = filter->exdata_def;

				n_json exdata_node;
				write_string(exdata_node, "name", my::ws(filter->name));
				write_child(exdata_node, "exdata_ptr", exdata_node_creator(filter_param->id, exdata));
				write_child(exdata_node, "exdata_def", exdata_node_creator(filter_param->id, filter->exdata_def));
				exdata_nodes.emplace_back(exdata_node);
			}

			return exdata_nodes;
		}
	} app_impl;
}
