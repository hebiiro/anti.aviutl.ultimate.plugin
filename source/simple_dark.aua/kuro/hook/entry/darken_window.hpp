#pragma once

namespace apn::dark::kuro::hook
{
	//
	// 出力ストリームにワイド文字列を書き込みます。
	//
	inline static auto& operator<<(std::ofstream& ofs, const std::wstring& s)
	{
		return ofs.write((LPCSTR)s.c_str(), s.length() * sizeof(wchar_t)), ofs;
	}

	//
	// このクラスは黒窓関係のフックを担当します。
	//
	inline struct darken_window_t : entry_t::node_t
	{
		//
		// 黒窓のダミーのインスタンスハンドルです。
		//
		HINSTANCE dummy = nullptr;

		//
		// 初期化処理を実行します。
		//
		virtual BOOL on_init() override
		{
			MY_TRACE_FUNC("");
#if 0
			// 黒窓のダミーをロードします。
			{
				base_path_t base(hive.instance);

				auto path = base.dir / L"DarkenWindow.aul";
				dummy = ::LoadLibraryW(path.c_str());
				MY_TRACE_HEX(dummy);
			}
#endif
			// APIフックを開始します。
			{
				auto kernel32 = ::GetModuleHandleW(L"kernel32.dll");
				MY_TRACE_HEX(kernel32);

				my::hook::detours detours;
				my::hook::attach(EnumProcessModules, ::GetProcAddress(kernel32, "K32EnumProcessModules"));
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		virtual BOOL on_exit() override
		{
			MY_TRACE_FUNC("");

			// APIフックを終了します。
			{
				my::hook::detours detours;
				my::hook::detach(EnumProcessModules);
			}
#if 0
			// 黒窓のダミーをアンロードします。
			{
				::FreeLibrary(dummy), dummy = nullptr;
			}
#endif
			return TRUE;
		}

		//
		// このクラスは::EnumProcessModules()をフックします。
		//
		struct {
			inline static BOOL WINAPI hook_proc(
				HANDLE process, HMODULE* modules, DWORD cb, LPDWORD cb_needed)
			{
				MY_TRACE_FUNC("{/hex}", process);

				// まず、デフォルト処理を実行します。
				auto result = orig_proc(process, modules, cb, cb_needed);
				if (!result) return FALSE;

				// ファイル選択ダイアログ以降にも適用する場合は
				if (!hive.dark.flag_exclude_comdlg32)
				{
					// 黒窓の設定ファイルを構築します。
					if (darken_window.deploy_setting_files()) return result;
				}

				// 黒窓を構築できなかったので無効にします。
				{
					auto c = *cb_needed / sizeof(HMODULE);
					for (decltype(c) i = 0; i < c; i++)
					{
						if (modules[i] == darken_window.dummy)
						{
							// 黒窓を無効にします。
							modules[i] = hive.instance;

							break;
						}
					}
				}

				return result;
			}
			inline static decltype(&hook_proc) orig_proc = ::EnumProcessModules;
		} EnumProcessModules;

		inline static std::wstring to_string(COLORREF color)
		{
			return my::format(LR"(#{/02x}{/02x}{/02x})",
				GetRValue(color), GetGValue(color), GetBValue(color));
		}

		inline static std::wstring to_string(LPCWSTR name, const kuro::paint::pigment_t* pigment)
		{
			if (!pigment) return {};

			return my::format(LR"(<NamedColor name="{/}" fillColor="{/}" edgeColor="{/}" textForeColor="{/}" textBackColor="{/}" />)",
				name,
				to_string(pigment->background.get_win32_color()),
				to_string(pigment->border.get_win32_color()),
				to_string(pigment->text.get_win32_color()),
				to_string(CLR_NONE));
		}

		inline static std::wstring to_string(LPCWSTR name, const kuro::paint::palette_t& palette, int part_id, int state_id)
		{
			return to_string(name, palette.get(part_id, state_id));
		}

		//
		// 黒窓の設定ファイルを配置します。
		//
		BOOL deploy_setting_files()
		{
			MY_TRACE_FUNC("");

			try
			{
				auto plugin_path = my::get_module_file_name(hive.instance).parent_path().parent_path();
				auto darken_window_path = plugin_path / L"DarkenWindow";
				auto darken_window_settings_stem = L"DarkenWindowSettings.xml";
				auto darken_window_settings_path = darken_window_path / darken_window_settings_stem;
				auto skin_name = L"dummy"s;
				auto skin_stem = skin_name + L".xml";
				auto skin_relative_path = std::filesystem::path(L"Skin") / skin_stem;
				auto skin_path = darken_window_path / skin_relative_path;
				auto skin_settings_stem = L"_Settings.xml"s;
				auto skin_settings_relative_path = std::filesystem::path(skin_name) / skin_settings_stem;
				auto skin_settings_path = darken_window_path / L"Skin" / skin_settings_relative_path;

				// 黒窓フォルダを作成します。
				std::filesystem::create_directories(skin_settings_path.parent_path());

				// 設定ファイルを作成します。
				{
					std::ofstream ofs(darken_window_settings_path, std::ios::out | std::ios::binary);
					ofs << L"\ufeff"s << my::format(LR"(<?xml version="1.0" encoding="UTF-16" standalone="no"?>)");
					ofs << L"\r\n"s << my::format(LR"(<Settings skin="{/}" shadowMode="{/}" roundMode="{/}" />)",
						skin_relative_path.c_str(),
						FALSE ? L"ON" : L"OFF",
						FALSE ? L"ON" : L"OFF");
				}

				// スキンファイルを作成します。
				{
					std::ofstream ofs(skin_path, std::ios::out | std::ios::binary);
					ofs << L"\ufeff"s << my::format(LR"(<?xml version="1.0" encoding="UTF-16" standalone="no"?>)");
					ofs << L"\r\n"s << my::format(LR"(<Settings>)");
					ofs << L"\r\n\t"s << my::format(LR"(<Skin fileName="{/}" />)", skin_settings_relative_path.c_str());
					ofs << L"\r\n"s << my::format(LR"(</Settings>)");
				}

				// スキン設定ファイルを作成します。
				{
					std::ofstream ofs(skin_settings_path, std::ios::out | std::ios::binary);
					ofs << L"\ufeff"s << my::format(LR"(<?xml version="1.0" encoding="UTF-16" standalone="no"?>)");
					ofs << L"\r\n"s << my::format(LR"(<Skin>)");
					ofs << L"\r\n\t"s << my::format(LR"(<Attributes>)");
					ofs << L"\r\n\t\t"s << my::format(LR"(<Dwm)");
					ofs << L"\r\n\t\t\t"s << my::format(LR"(activeBorderColor="{/}" activeCaptionColor="{/}" activeTextColor="{/}")",
						to_string(kuro::paint::window_material.active.border.get_win32_color()),
						to_string(kuro::paint::window_material.active.background.get_win32_color()),
						to_string(kuro::paint::window_material.active.text.get_win32_color()));
					ofs << L"\r\n\t\t\t"s << my::format(LR"(inactiveBorderColor="{/}" inactiveCaptionColor="{/}" inactiveTextColor="{/}")",
						to_string(kuro::paint::window_material.inactive.border.get_win32_color()),
						to_string(kuro::paint::window_material.inactive.background.get_win32_color()),
						to_string(kuro::paint::window_material.inactive.text.get_win32_color()));
					ofs << L"\r\n\t\t\t"s << my::format(LR"(darkMode="{/}" cornerMode="{/}")", 0, 0);
					ofs << L"\r\n\t\t"s << my::format(LR"(/>)");
					ofs << L"\r\n\t\t"s << my::format(LR"(<NamedColors>)");

					const auto& menu = kuro::paint::menu_material.palette;
					const auto& dialog = kuro::paint::dialog_material.palette;
					const auto& button = kuro::paint::button_material.palette;
					const auto& editbox = kuro::paint::editbox_material.palette;
					const auto& scrollbar = kuro::paint::scrollbar_material.palette;
					const auto& trackbar = kuro::paint::trackbar_material.palette;

					ofs << L"\r\n\t\t\t"s << to_string(L"caption_active", &kuro::paint::window_material.active);
					ofs << L"\r\n\t\t\t"s << to_string(L"caption_inactive", &kuro::paint::window_material.inactive);
					ofs << L"\r\n\t\t\t"s << to_string(L"caption_disabled", &kuro::paint::window_material.inactive);

					ofs << L"\r\n\t\t\t"s << to_string(L"border_active", &kuro::paint::window_material.active);
					ofs << L"\r\n\t\t\t"s << to_string(L"border_inactive", &kuro::paint::window_material.inactive);
					ofs << L"\r\n\t\t\t"s << to_string(L"border_disabled", &kuro::paint::window_material.inactive);

					ofs << L"\r\n\t\t\t"s << to_string(L"dialog", dialog, WP_DIALOG, ETS_NORMAL);

					ofs << L"\r\n\t\t\t"s << to_string(L"c1_normal", editbox, EP_EDITTEXT, ETS_NORMAL);
					ofs << L"\r\n\t\t\t"s << to_string(L"c1_hot", editbox, EP_EDITTEXT, ETS_HOT);
					ofs << L"\r\n\t\t\t"s << to_string(L"c1_selected", editbox, EP_EDITTEXT, ETS_SELECTED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c1_disabled", editbox, EP_EDITTEXT, ETS_DISABLED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c1_readonly", editbox, EP_EDITTEXT, ETS_READONLY);
					ofs << L"\r\n\t\t\t"s << to_string(L"c1_statictext", dialog, WP_DIALOG, ETS_NORMAL);

					ofs << L"\r\n\t\t\t"s << to_string(L"c2_normal", button, BP_PUSHBUTTON, ETS_NORMAL);
					ofs << L"\r\n\t\t\t"s << to_string(L"c2_hot", button, BP_PUSHBUTTON, PBS_HOT);
					ofs << L"\r\n\t\t\t"s << to_string(L"c2_selected", button, BP_PUSHBUTTON, PBS_PRESSED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c2_disabled", button, BP_PUSHBUTTON, PBS_DISABLED);

					ofs << L"\r\n\t\t\t"s << to_string(L"c3_normal", scrollbar, SBP_THUMBBTNHORZ, SCRBS_NORMAL);
					ofs << L"\r\n\t\t\t"s << to_string(L"c3_hover", scrollbar, SBP_THUMBBTNHORZ, SCRBS_HOVER);
					ofs << L"\r\n\t\t\t"s << to_string(L"c3_hot", scrollbar, SBP_THUMBBTNHORZ, SCRBS_HOT);
					ofs << L"\r\n\t\t\t"s << to_string(L"c3_selected", scrollbar, SBP_THUMBBTNHORZ, SCRBS_PRESSED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c3_disabled", scrollbar, SBP_THUMBBTNHORZ, SCRBS_DISABLED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c3_gutter", trackbar, TKP_TRACK, TRS_NORMAL);
					ofs << L"\r\n\t\t\t"s << to_string(L"c3_track", editbox, EP_EDITTEXT, ETS_SELECTED);

					ofs << L"\r\n\t\t\t"s << to_string(L"c4_normal", menu, MENU_BARITEM, MBI_NORMAL);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_hot", menu, MENU_BARITEM, MBI_HOT);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_selected", menu, MENU_BARITEM, MBI_PUSHED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_disabled", menu, MENU_BARITEM, MBI_DISABLED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_disabled_hot", menu, MENU_BARITEM, MBI_DISABLEDHOT);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_disabled_selected", menu, MENU_BARITEM, MBI_DISABLEDPUSHED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_gutter", menu, MENU_POPUPGUTTER, 0);
					ofs << L"\r\n\t\t\t"s << to_string(L"c4_separator", menu, MENU_POPUPSEPARATOR, 0);

					ofs << L"\r\n\t\t\t"s << to_string(L"c5_normal", dialog, WP_DIALOG, ETS_NORMAL);
					ofs << L"\r\n\t\t\t"s << to_string(L"c5_hot", button, BP_PUSHBUTTON, PBS_HOT);
					ofs << L"\r\n\t\t\t"s << to_string(L"c5_selected", button, BP_PUSHBUTTON, PBS_PRESSED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c5_disabled", dialog, WP_DIALOG, ETS_DISABLED);
					ofs << L"\r\n\t\t\t"s << to_string(L"c5_checked", button, BP_CHECKBOX, CBS_CHECKEDNORMAL);

					ofs << L"\r\n\t\t"s << my::format(LR"(</NamedColors>)");
					ofs << L"\r\n\t"s << my::format(LR"(</Attributes>)");
					ofs << L"\r\n"s << my::format(LR"(</Skin>)");
				}

				return TRUE;
			}
			catch (...)
			{
			}

			return FALSE;
		}
	} darken_window;
}
