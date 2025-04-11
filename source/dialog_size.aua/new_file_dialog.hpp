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

		//
		// ウィンドウプロシージャです。
		//
		virtual LRESULT on_wnd_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) override
		{
			switch (message)
			{
			case WM_INITDIALOG:
				{
					MY_TRACE_FUNC("WM_INITDIALOG, {/hex}, {/hex}", wParam, lParam);

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
						for (const auto& video_size : hive.video_size_collection)
						{
							if (video_size.display_name.empty())
							{
								my::combobox::add_text(combobox,
									my::format(L"{/} x {/}", video_size.width, video_size.height).c_str());
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
						for (const auto& video_rate : hive.video_rate_collection)
						{
							if (video_rate.display_name.empty())
							{
								my::combobox::add_text(combobox,
									my::format(L"{/}fps", video_rate.rate).c_str());
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
						for (const auto& audio_rate : hive.audio_rate_collection)
						{
							if (audio_rate.display_name.empty())
							{
								my::combobox::add_text(combobox,
									my::format(L"{/}Hz", audio_rate.rate).c_str());
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

					MY_TRACE_FUNC("WM_COMMAND, {/hex16}, {/hex16}, {/hex}", code, control_id, control);

					switch (control_id)
					{
					case c_command_id.c_preset.c_video_size:
						{
							if (code != CBN_SELCHANGE) break;

							auto index = (size_t)my::combobox::get_cur_sel(control);
							if (index >= hive.video_size_collection.size()) break;

							// 映像サイズのプリセットを適用します。
							const auto& video_size = hive.video_size_collection[index];
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
							if (index >= hive.video_rate_collection.size()) break;

							// 映像レートのプリセットを適用します。
							const auto& video_rate = hive.video_rate_collection[index];
							::SetDlgItemTextW(hwnd, c_command_id.c_video_rate, video_rate.rate.c_str());

							break;
						}
					case c_command_id.c_preset.c_audio_rate:
						{
							if (code != CBN_SELCHANGE) break;

							auto index = (size_t)my::combobox::get_cur_sel(control);
							if (index >= hive.audio_rate_collection.size()) break;

							// 音声レートのプリセットを適用します。
							const auto& audio_rate = hive.audio_rate_collection[index];
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
