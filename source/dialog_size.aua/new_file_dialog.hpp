#pragma once

namespace apn::dialog_size
{
	//
	// このクラスは`新規プロジェクトの作成`ダイアログをフックします。
	//
	inline struct NewFileDialog : my::Window
	{
		inline static constexpr struct CommandID {
			inline static constexpr uint32_t c_video_width = 171;
			inline static constexpr uint32_t c_video_height = 172;
			inline static constexpr uint32_t c_video_rate = 173;
			inline static constexpr uint32_t c_audio_rate = 174;
			inline static constexpr uint32_t c_use_file_param = 201;
			inline static constexpr struct Preset {
				inline static constexpr uint32_t c_video_size = 2024;
				inline static constexpr uint32_t c_video_rate = 2025;
				inline static constexpr uint32_t c_audio_rate = 2026;
				inline static constexpr uint32_t c_swap_video_size = 2027;
			} c_preset;
		} c_command_id;

		struct VideoSize {
			std::wstring display_name;
			std::wstring width;
			std::wstring height;
		};
		std::vector<VideoSize> video_size_collection;

		struct VideoRate {
			std::wstring display_name;
			std::wstring rate;
		};
		std::vector<VideoRate> video_rate_collection;

		struct AudioRate {
			std::wstring display_name;
			std::wstring rate;
		};
		std::vector<AudioRate> audio_rate_collection;

		//
		// ノードからコンフィグを読み込みます。
		//
		BOOL read_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			video_size_collection.clear();
			video_rate_collection.clear();
			audio_rate_collection.clear();

			n_json new_file_node;
			get_child_node(root, "new_file", new_file_node);

			get_child_nodes(new_file_node, "video_size",
				[&](const n_json& video_size_node, size_t i)
			{
				auto& video_size = video_size_collection.emplace_back();
				get_string(video_size_node, "width", video_size.width);
				get_string(video_size_node, "height", video_size.height);
				get_string(video_size_node, "name", video_size.display_name);

				return TRUE;
			});

			get_child_nodes(new_file_node, "video_rate",
				[&](const n_json& video_rate_node, size_t i)
			{
				auto& video_rate = video_rate_collection.emplace_back();
				get_string(video_rate_node, "rate", video_rate.rate);
				get_string(video_rate_node, "name", video_rate.display_name);

				return TRUE;
			});

			get_child_nodes(new_file_node, "audio_rate",
				[&](const n_json& audio_rate_node, size_t i)
			{
				auto& audio_rate = audio_rate_collection.emplace_back();
				get_string(audio_rate_node, "rate", audio_rate.rate);
				get_string(audio_rate_node, "name", audio_rate.display_name);

				return TRUE;
			});

			return TRUE;
		}

		//
		// ノードにコンフィグを書き込みます。
		//
		BOOL write_node(n_json& root)
		{
			MY_TRACE_FUNC("");

			n_json new_file_node;

			set_child_nodes(new_file_node, "video_size",
				video_size_collection,
				[&](n_json& video_size_node, const auto& video_size, size_t i)
			{
				set_string(video_size_node, "width", video_size.width);
				set_string(video_size_node, "height", video_size.height);
				set_string(video_size_node, "name", video_size.display_name);

				return TRUE;
			});

			set_child_nodes(new_file_node, "video_rate",
				video_rate_collection,
				[&](n_json& video_rate_node, const auto& video_rate, size_t i)
			{
				set_string(video_rate_node, "rate", video_rate.rate);
				set_string(video_rate_node, "name", video_rate.display_name);

				return TRUE;
			});

			set_child_nodes(new_file_node, "audio_rate",
				audio_rate_collection,
				[&](n_json& audio_rate_node, const auto& audio_rate, size_t i)
			{
				set_string(audio_rate_node, "rate", audio_rate.rate);
				set_string(audio_rate_node, "name", audio_rate.display_name);

				return TRUE;
			});

			set_child_node(root, "new_file", new_file_node);

			return TRUE;
		}

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {:#010x}, {:#010x}", wParam, lParam);

					//
					// コンボボックスのオプションをカスタマイズします。
					//
					const auto customize_combobox_option = [](HWND combobox) {
						auto rc = my::get_window_rect(combobox);
						auto dropped_width = my::get_width(rc) * 2;
						::SendMessage(combobox, CB_SETDROPPEDWIDTH, dropped_width, 0);
					};

					{
						// 映像サイズのコンボボックスを初期化します。
						auto combobox = ::GetDlgItem(hwnd, c_command_id.c_preset.c_video_size);
						customize_combobox_option(combobox);
						for (const auto& video_size : video_size_collection)
						{
							if (video_size.display_name.empty())
							{
								my::combobox::add_text(combobox,
									std::format(L"{} x {}", video_size.width, video_size.height).c_str());
							}
							else
							{
								my::combobox::add_text(combobox, video_size.display_name.c_str());
							}
						}
					}

					{
						// 映像レートのコンボボックスを初期化します。
						auto combobox = ::GetDlgItem(hwnd, c_command_id.c_preset.c_video_rate);
						customize_combobox_option(combobox);
						for (const auto& video_rate : video_rate_collection)
						{
							if (video_rate.display_name.empty())
							{
								my::combobox::add_text(combobox,
									std::format(L"{}fps", video_rate.rate).c_str());
							}
							else
							{
								my::combobox::add_text(combobox, video_rate.display_name.c_str());
							}
						}
					}

					{
						// 音声レートのコンボボックスを初期化します。
						auto combobox = ::GetDlgItem(hwnd, c_command_id.c_preset.c_audio_rate);
						customize_combobox_option(combobox);
						for (const auto& audio_rate : audio_rate_collection)
						{
							if (audio_rate.display_name.empty())
							{
								my::combobox::add_text(combobox,
									std::format(L"{}Hz", audio_rate.rate).c_str());
							}
							else
							{
								my::combobox::add_text(combobox, audio_rate.display_name.c_str());
							}
						}
					}

					break;
				}
			case WM_COMMAND:
				{
					auto code = HIWORD(wParam);
					auto control_id = LOWORD(wParam);
					auto control = (HWND)lParam;

					MY_TRACE_FUNC("WM_COMMAND, {:#04x}, {:#04x}, {:#010x}", code, control_id, control);

					switch (control_id)
					{
					case c_command_id.c_preset.c_video_size:
						{
							if (code != CBN_SELCHANGE) break;

							auto index = (size_t)my::combobox::get_cur_sel(control);
							if (index >= video_size_collection.size()) break;

							// 映像サイズのプリセットを適用します。
							const auto& video_size = video_size_collection[index];
							if (::SendDlgItemMessage(hwnd, c_command_id.c_preset.c_swap_video_size, BM_GETCHECK, 0, 0) == BST_CHECKED)
							{
								::SetDlgItemTextW(hwnd, c_command_id.c_video_width, video_size.height.c_str());
								::SetDlgItemTextW(hwnd, c_command_id.c_video_height, video_size.width.c_str());
							}
							else
							{
								::SetDlgItemTextW(hwnd, c_command_id.c_video_width, video_size.width.c_str());
								::SetDlgItemTextW(hwnd, c_command_id.c_video_height, video_size.height.c_str());
							}

							break;
						}
					case c_command_id.c_preset.c_video_rate:
						{
							if (code != CBN_SELCHANGE) break;

							auto index = (size_t)my::combobox::get_cur_sel(control);
							if (index >= video_rate_collection.size()) break;

							// 映像レートのプリセットを適用します。
							const auto& video_rate = video_rate_collection[index];
							::SetDlgItemTextW(hwnd, c_command_id.c_video_rate, video_rate.rate.c_str());

							break;
						}
					case c_command_id.c_preset.c_audio_rate:
						{
							if (code != CBN_SELCHANGE) break;

							auto index = (size_t)my::combobox::get_cur_sel(control);
							if (index >= audio_rate_collection.size()) break;

							// 音声レートのプリセットを適用します。
							const auto& audio_rate = audio_rate_collection[index];
							::SetDlgItemTextW(hwnd, c_command_id.c_audio_rate, audio_rate.rate.c_str());

							break;
						}
					}

					break;
				}
			}

			return __super::on_wnd_proc(hwnd, message, wParam, lParam);
		}
	} new_file_dialog;
}
