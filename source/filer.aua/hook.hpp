#pragma once

namespace apn::filer
{
	//
	// このクラスはフックを管理します。
	//
	inline struct HookManager
	{
		//
		// 独自のエイリアス読み込み処理を行うためのフラグです。
		//
		inline static BOOL hook_add_alias = FALSE;

		//
		// エイリアス読み込みに使用するファイル名の配列です。
		//
		inline static std::vector<std::string> alias_file_names;

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			::SetWindowSubclass(magi.exin.get_exedit_window(), exedit_window.hook_proc, (UINT_PTR)this, 0);
			::SetWindowSubclass(magi.exin.get_setting_dialog(), setting_dialog.hook_proc, (UINT_PTR)this, 0);

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			add_alias.orig_proc = magi.exin.address.function.add_alias;
			my::hook::attach(add_alias);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE("拡張編集のフックに成功しました\n");

				return TRUE;
			}
			else
			{
				MY_TRACE("拡張編集のフックに失敗しました\n");

				return FALSE;
			}
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}

		//
		// エイリアスファイル名の配列を消去します。
		//
		inline static void clear_alias_file_names()
		{
			alias_file_names.clear();
		}

		//
		// エイリアスファイルを読み込み、フィルタ毎に分解します。
		// そして、それぞれのフィルタを一時ファイルに保存し、そのファイル名を配列に追加します。
		//
		inline static BOOL add_alias_file(LPCSTR file_name)
		{
			// 戻り値です。
			BOOL ret_value = FALSE;

			// テンポラリフォルダのパスです。
			auto temp_path = my::hs(my::get_temp_path());
			MY_TRACE_STR(temp_path);

			// カレントプロセスのIDです。
			auto pid = ::GetCurrentProcessId();
			MY_TRACE_INT(pid);

			// iniファイル内のセクションを読み込むためのバッファです。
			std::vector<char> section;

			for (auto i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// セクションのapp_nameを取得します。
				auto app_name = my::format("vo.{/}", i);
				MY_TRACE_STR(app_name);

				// セクション内の_nameを取得します。
				std::string name(MAX_PATH, '\0');
				auto name_length = ::GetPrivateProfileStringA(
					app_name.c_str(), "_name", "", name.data(), name.size(), file_name);
				name.resize(name_length);
				MY_TRACE_STR(name);

				// 読み込み可能なフィルタかチェックします。
				if (name.empty()) continue;
				else if (name == "動画ファイル") continue;
				else if (name == "画像ファイル") continue;
				else if (name == "音声ファイル") continue;
				else if (name == "テキスト") continue;
				else if (name == "図形") continue;
				else if (name == "フレームバッファ") continue;
				else if (name == "音声波形") continue;
				else if (name == "シーン") continue;
				else if (name == "シーン(音声)") continue;
				else if (name == "直前オブジェクト") continue;
				else if (name == "標準描画") continue;
				else if (name == "拡張描画") continue;
				else if (name == "標準再生") continue;
				else if (name == "パーティクル出力") continue;
				else if (name == "カスタムオブジェクト") continue;
				else if (name == "時間制御") continue;
				else if (name == "グループ制御") continue;
				else if (name == "カメラ制御") continue;

				// セクションデータを取得します。
				section.clear();
				section.resize(32767, '\0');
				auto size = ::GetPrivateProfileSectionA(app_name.c_str(), section.data(), section.size(), file_name);
				section.resize(size);
				std::replace(section.begin(), section.end(), '\0', '\n');

				// セクションのヘッダーを構築します。
				auto section_header = my::format("[vo.0]\n");
				MY_TRACE_STR(section_header);

				// 一時ファイルのファイル名を構築します。
				auto temp_file_name = my::format("{/}\\apn_filer_{/}_{/}.exa", temp_path, pid, i);
				MY_TRACE_STR(temp_file_name);

				// 一時ファイルにセクションヘッダーとセクションデータを書き込みます。
				std::ofstream ofs(temp_file_name);
				ofs.write(section_header.data(), section_header.size());
				ofs.write(section.data(), section.size());

				// 配列に一時ファイルのファイル名を追加します。
				alias_file_names.push_back(temp_file_name);

				ret_value = TRUE;
			}

			return ret_value;
		}

		//
		// アイテムにエイリアスを追加します。
		// WM_DROPFILESのタイミングで呼ばれます。
		//
		inline static void load_alias()
		{
			MY_TRACE_FUNC("");

			hook_add_alias = TRUE;
			// この中でadd_alias()が呼ばれるのでフックします。
			// 1036はエイリアスを追加するコマンドです。1はエイリアスのインデックスです。
			::SendMessage(magi.exin.get_exedit_window(), WM_COMMAND, 1036, 1);
			hook_add_alias = FALSE;
		}

		//
		// フィルタの名前を返します。
		//
		inline static LPCSTR get_name(ExEdit::Object* object, int filter_index)
		{
			if (filter_index < 0)
			{
				if (object->dispname[0])
					return object->dispname;

				filter_index = 0;
			}

			auto filter = magi.exin.get_filter(object, filter_index);
			if (!filter) return "";

			auto id = object->filter_param[filter_index].id;
			if (id == 79) // アニメーション効果
			{
				auto exdata = magi.exin.get_exdata(object, filter_index);
				auto name = (LPCSTR)(exdata + 0x04);
				if (!name[0])
				{
					auto type = *(WORD*)(exdata + 0);
					MY_TRACE_HEX(type);

					auto filter = *(WORD*)(exdata + 2);
					MY_TRACE_HEX(filter);

					switch (type)
					{
					case 0x00: name = "震える"; break;
					case 0x01: name = "振り子"; break;
					case 0x02: name = "弾む"; break;
					case 0x03: name = "座標の拡大縮小(個別オブジェクト)"; break;
					case 0x04: name = "画面外から登場"; break;
					case 0x05: name = "ランダム方向から登場"; break;
					case 0x06: name = "拡大縮小して登場"; break;
					case 0x07: name = "ランダム間隔で落ちながら登場"; break;
					case 0x08: name = "弾んで登場"; break;
					case 0x09: name = "広がって登場"; break;
					case 0x0A: name = "起き上がって登場"; break;
					case 0x0B: name = "何処からともなく登場"; break;
					case 0x0C: name = "反復移動"; break;
					case 0x0D: name = "座標の回転(個別オブジェクト)"; break;
					case 0x0E: name = "立方体(カメラ制御)"; break;
					case 0x0F: name = "球体(カメラ制御)"; break;
					case 0x10: name = "砕け散る"; break;
					case 0x11: name = "点滅"; break;
					case 0x12: name = "点滅して登場"; break;
					case 0x13: name = "簡易変形"; break;
					case 0x14: name = "簡易変形(カメラ制御)"; break;
					case 0x15: name = "リール回転"; break;
					case 0x16: name = "万華鏡"; break;
					case 0x17: name = "円形配置"; break;
					case 0x18: name = "ランダム配置"; break;
					default: name = "アニメーション効果"; break;
					}
				}
				return name;
			}
			else if (id == 80) // カスタムオブジェクト
			{
				auto exdata = magi.exin.get_exdata(object, filter_index);
				auto name = (LPCSTR)(exdata + 0x04);
				if (!name[0]) name = "カスタムオブジェクト";
				return name;
			}
			else
			{
				return filter->name;
			}
		}

		//
		// 指定されたファイル名の中の使用できない文字を'_'に置き換えます。
		//
		inline static void normalize(LPSTR file_name)
		{
			auto c = strlen(file_name);
			for (decltype(c) i = 0; i < c; i++)
			{
				auto ch = file_name[i];
				auto type = ::PathGetCharTypeA(ch);
				switch (type)
				{
				case GCT_INVALID:
				case GCT_WILD:
					{
						file_name[i] = '_';
						break;
					}
				}
			}
		}

		//
		// ファイル選択ダイアログを出してフィルタをエイリアスファイルに保存します。
		//
		inline static BOOL save_alias(HWND hwnd, int object_index, int filter_index)
		{
			MY_TRACE_FUNC("{/hex}, {/}, {/}", hwnd, object_index, filter_index);

			// オブジェクトを取得します。
			auto object = magi.exin.get_object(object_index);
			if (!object) return FALSE;

			// ダイアログのタイトルとファイル名を取得します。
			std::string title, file_name;
			{
				auto name = get_name(object, filter_index);

				switch (filter_index)
				{
				case -2:
					{
						title = my::format("オブジェクトの保存");
						file_name = my::format("{/}.exa", name);

						break;
					}
				case -1:
					{
						title = my::format("全フィルタの保存");
						file_name = my::format("{/}のフィルタ全体.exa", object->dispname);

						break;
					}
				default:
					{
						title = my::format("フィルタの保存");
						file_name = my::format("{/}.exa", name);

						break;
					}
				}

				normalize(file_name.data());
			}

			// フォルダ名を取得します。
			auto folder_name = my::hs(my::get_window_text(
				share::filer_window::get_browser(hive.active_window)));
			if (folder_name.empty())
				folder_name = my::hs(my::get_current_directory());

			file_name.resize(MAX_PATH);

			OPENFILENAMEA ofn = { sizeof(ofn) };
			ofn.hwndOwner = hwnd;
			ofn.Flags = OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = title.c_str();
			ofn.lpstrInitialDir = folder_name.c_str();
			ofn.lpstrFile = file_name.data();
			ofn.nMaxFile = file_name.size();
			ofn.lpstrFilter = "エイリアスファイル (*.exa)\0*.exa\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = "exa";

			if (!::GetSaveFileNameA(&ofn))
				return FALSE;

			return magi.exin.save_filter_alias(object_index, filter_index, file_name.c_str());
		}

		//
		// このクラスは拡張編集ウィンドウをフックします。
		//
		struct {
			inline static LRESULT CALLBACK hook_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR id_subclass, DWORD_PTR ref_data)
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case 1070: // エイリアスを作成
							{
								MY_TRACE("エイリアスを作成します\n");

								if (!hive.use_common_dialog)
									break;

								if (magi.exin.get_object_selection_count() > 0)
								{
									auto object_index = magi.exin.get_object_selection(0);

									save_alias(hwnd, object_index, -2);
								}

								return 0;
							}
						}

						break;
					}
				}

				return ::DefSubclassProc(hwnd, message, wParam, lParam);
			}
		} exedit_window;

		//
		// このクラスは設定ダイアログをフックします。
		//
		struct {
			inline static LRESULT CALLBACK hook_proc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR id_subclass, DWORD_PTR ref_data)
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case 1109: // 『設定の保存』→『現在の設定でエイリアスを作成する』
							{
								MY_TRACE("エイリアスを作成します\n");

								if (!hive.use_common_dialog)
									break;

								auto object_index = magi.exin.get_current_object_index();
								auto filter_index = magi.exin.get_current_filter_index();

								if (filter_index <= 0)
								{
									save_alias(hwnd, object_index, -1);
								}
								else
								{
									save_alias(hwnd, object_index, filter_index);
								}

								return 0;
							}
						}

						break;
					}
				case WM_DROPFILES:
					{
						MY_TRACE("WM_DROPFILES\n");

						clear_alias_file_names();
						auto result = FALSE;

						auto drop = (HDROP)wParam;
						auto c = ::DragQueryFile(drop, 0xFFFFFFFF, nullptr, 0);
						for (decltype(c) i = 0; i < c; i++)
						{
							std::string file_name(MAX_PATH, '\0');
							auto length = ::DragQueryFileA(drop, i, file_name.data(), file_name.size());
							file_name.resize(length);
							MY_TRACE_STR(file_name);

							if (std::filesystem::path(file_name).extension() == ".exa")
								result |= add_alias_file(file_name.c_str());
						}
						//::DragFinish(drop); // これを呼ぶとデフォルトの処理が実行できなくなります。

						if (result)
						{
							load_alias();

							return 0;
						}

						break;
					}
				}

				return ::DefSubclassProc(hwnd, message, wParam, lParam);
			}
		} setting_dialog;

		//
		// このクラスは拡張編集のadd_alias()をフックします。
		//
		struct {
			inline static BOOL CDECL hook_proc(LPCSTR file_name, BOOL flag1, BOOL flag2, int object_index)
			{
				MY_TRACE_FUNC("{/}, {/}, {/}, {/}", file_name, flag1, flag2, object_index);

				if (hook_add_alias)
				{
					// フラグが立っている場合はエイリアスファイル名の配列を使用して
					// アイテムに複数のエイリアスを追加します。

					auto ret_value = FALSE;

					for (const auto& file_name : alias_file_names)
					{
						MY_TRACE_STR(file_name);

						ret_value |= orig_proc(file_name.c_str(), flag1, flag2, object_index);

						::DeleteFileA(file_name.c_str());
					}

					return ret_value;
				}

				return orig_proc(file_name, flag1, flag2, object_index);
			}
			inline static decltype(&hook_proc) orig_proc = nullptr;
		} add_alias;
	} hook_manager;
}
