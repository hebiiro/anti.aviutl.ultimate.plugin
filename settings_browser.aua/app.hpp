#pragma once

namespace apn::settings_browser
{
	//
	// このクラスはアプリケーションです。
	//
	inline struct App : AppInterface
	{
		//
		// コンストラクタです。
		//
		App() { app = this; }

		//
		// 選択アイテムの設定からjsonを生成します。
		//
		virtual BOOL generate_settings_json() override
		{
			MY_TRACE_FUNC("");

			auto object_index = magi.exin.get_current_object_index();
			if (object_index < 0)
			{
				hive.message_box(L"アイテムを選択してください");

				return FALSE;
			}

			auto object = magi.exin.get_object(object_index);
			if (!object)
			{
				hive.message_box(L"オブジェクトが無効です");

				return FALSE;
			}

			if (!(object->flag & ExEdit::Object::Flag::Exist))
			{
				hive.message_box(L"オブジェクトの状態が無効です");

				return FALSE;
			}

			n_json root;
			{
				n_json node;
				{
					n_json filter_nodes;
					{
						for (int32_t filter_index = 0; filter_index < ExEdit::Object::MAX_FILTER; filter_index++)
						{
							// フィルタを取得します。
							auto filter = magi.exin.get_filter(object, filter_index);
							if (!filter) break;

							n_json filter_node;
							{
								set_string(filter_node, "name", my::ws(filter->name));

								n_json track_nodes;
								{
									for (int32_t track_index = 0; track_index < filter->track_n; track_index++)
									{
										if (filter->track_default[track_index] < 0) continue;

										n_json track_node;
										{
											set_string(track_node, "name", my::ws(filter->track_name[track_index]));
											set_int(track_node, "default", filter->track_default[track_index]);
											set_int(track_node, "s", filter->track_s[track_index]);
											set_int(track_node, "e", filter->track_e[track_index]);
											set_int(track_node, "scale", filter->track_scale[track_index]);
											if (filter->track_drag_min) set_int(track_node, "drag_min", filter->track_drag_min[track_index]);
											if (filter->track_drag_max) set_int(track_node, "drag_max", filter->track_drag_max[track_index]);
											if (filter->track) set_int(track_node, "track", filter->track[track_index]);
											if (filter->track_value_left) set_int(track_node, "value_left", filter->track_value_left[track_index]);
											if (filter->track_value_right) set_int(track_node, "value_right", filter->track_value_right[track_index]);
										}
										track_nodes.emplace_back(track_node);
									}
								}
								set_child(filter_node, "track", track_nodes);

								n_json check_nodes;
								{
									for (int32_t check_index = 0; check_index < filter->check_n; check_index++)
									{
										if (filter->check_default[check_index] < 0) continue;

										n_json check_node;
										{
											set_string(check_node, "name", my::ws(filter->check_name[check_index]));
											set_int(check_node, "default", filter->check_default[check_index]);
											if (filter->check) set_int(check_node, "check", filter->check[check_index]);
											if (filter->check_value) set_int(check_node, "value", filter->check_value[check_index]);
										}
										check_nodes.emplace_back(check_node);
									}
								}
								set_child(filter_node, "check", check_nodes);

								// ここでexdataも解析する。
							}
							filter_nodes.emplace_back(filter_node);
						}
					}
					set_child(node, "filter", filter_nodes);
				}
				set_child(root, "show_settings", node);
			}

			// jsonを文字列に変換します。
			hive.settings_json = my::cp_to_wide(root.dump(1, '\t'), CP_UTF8);

			// ブラウザにjson文字列を送信します。
			browser.post_json(hive.settings_json);

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
	} app_impl;
}
