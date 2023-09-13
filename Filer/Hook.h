#pragma once

namespace fgo::filer
{
	//
	// このクラスはフック処理を担当します。
	//
	struct Hook {

		//
		// 独自のエイリアス読み込み処理を行うためのフラグです。
		//
		inline static BOOL hookAddAlias = FALSE;

		//
		// エイリアス読み込みに使用するファイル名の配列です。
		//
		inline static std::vector<std::string> aliasFileNames;

		//
		// コンストラクタです。
		//
		Hook()
		{
			MY_TRACE_FUNC("");

			// 拡張編集のアドレスを取得します。
			ExEditWindowProc.orig = magi.auin.GetExEditWindowProc();
			SettingDialogProc.orig = magi.auin.GetSettingDialogProc();
			AddAlias.orig = magi.auin.GetAddAlias();

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			Tools::attach(ExEditWindowProc);
			Tools::attach(SettingDialogProc);
			Tools::attach(AddAlias);

			if (DetourTransactionCommit() == NO_ERROR)
			{
				MY_TRACE(_T("拡張編集のフックに成功しました\n"));
			}
			else
			{
				MY_TRACE(_T("拡張編集のフックに失敗しました\n"));
			}
		}

		//
		// デストラクタです。
		//
		~Hook()
		{
			MY_TRACE_FUNC("");
		}

		//
		// エイリアスファイル名の配列を消去します。
		//
		inline static void clearAliasFile()
		{
			aliasFileNames.clear();
		}

		//
		// エイリアスファイルを読み込み、フィルタ毎に分解します。
		// そして、それぞれのフィルタを一時ファイルに保存し、そのファイル名を配列に追加します。
		//
		inline static BOOL addAliasFile(LPCSTR fileName)
		{
			// 戻り値です。
			BOOL retValue = FALSE;

			// テンポラリフォルダのパスです。
			char tempPath[MAX_PATH] = {};
			::GetTempPathA(MAX_PATH, tempPath);
			MY_TRACE_STR(tempPath);

			// カレントプロセスのIDです。
			DWORD pid = ::GetCurrentProcessId();
			MY_TRACE_INT(pid);

			// iniファイル内のセクションを読み込むためのバッファです。
			std::vector<char> section;

			for (int i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				// セクションのappNameを取得します。
				char appName[MAX_PATH] = {};
				::StringCchPrintfA(appName, std::size(appName), "vo.%d", i);
				MY_TRACE_STR(appName);

				// セクション内の_nameを取得します。
				char name[MAX_PATH] = {};
				::GetPrivateProfileStringA(appName, "_name", "", name, MAX_PATH, fileName);
				MY_TRACE_STR(name);

				// 読み込み可能なフィルタかチェックします。
				if (::lstrlenA(name) == 0) continue;
				else if (::lstrcmpA(name, "動画ファイル") == 0) continue;
				else if (::lstrcmpA(name, "画像ファイル") == 0) continue;
				else if (::lstrcmpA(name, "音声ファイル") == 0) continue;
				else if (::lstrcmpA(name, "テキスト") == 0) continue;
				else if (::lstrcmpA(name, "図形") == 0) continue;
				else if (::lstrcmpA(name, "フレームバッファ") == 0) continue;
				else if (::lstrcmpA(name, "音声波形") == 0) continue;
				else if (::lstrcmpA(name, "シーン") == 0) continue;
				else if (::lstrcmpA(name, "シーン(音声)") == 0) continue;
				else if (::lstrcmpA(name, "直前オブジェクト") == 0) continue;
				else if (::lstrcmpA(name, "標準描画") == 0) continue;
				else if (::lstrcmpA(name, "拡張描画") == 0) continue;
				else if (::lstrcmpA(name, "標準再生") == 0) continue;
				else if (::lstrcmpA(name, "パーティクル出力") == 0) continue;
				else if (::lstrcmpA(name, "カスタムオブジェクト") == 0) continue;
				else if (::lstrcmpA(name, "時間制御") == 0) continue;
				else if (::lstrcmpA(name, "グループ制御") == 0) continue;
				else if (::lstrcmpA(name, "カメラ制御") == 0) continue;

				// セクションデータを取得します。
				section.clear();
				section.resize(32767, 0);
				DWORD size = ::GetPrivateProfileSectionA(appName, section.data(), section.size(), fileName);
				section.resize(size);
				std::replace(section.begin(), section.end(), '\0', '\n');

				// セクションのヘッダーを構築します。
				char sectionHeader[MAX_PATH] = {};
				::StringCchPrintfA(sectionHeader, std::size(sectionHeader), "[vo.0]\n");
				MY_TRACE_STR(sectionHeader);

				// 一時ファイルのファイル名を構築します。
				char tempFileName[MAX_PATH] = {};
				::StringCchPrintfA(tempFileName, std::size(tempFileName), "%hs\\AviUtl_Filer_%d_%d.exa", tempPath, pid, i);
				MY_TRACE_STR(tempFileName);

				// 一時ファイルにセクションヘッダーとセクションデータを書き込みます。
				Handle file = createFileForWrite((_bstr_t)tempFileName);
				writeFile(file, sectionHeader);
				writeFile(file, section.data(), section.size());

				// 配列に一時ファイルのファイル名を追加します。
				aliasFileNames.push_back(tempFileName);

				retValue = TRUE;
			}

			return retValue;
		}

		//
		// アイテムにエイリアスを追加します。
		// WM_DROPFILESのタイミングで呼ばれます。
		//
		inline static void loadAlias()
		{
			MY_TRACE_FUNC("");

			hookAddAlias = TRUE;
			// この中でAddAlias()が呼ばれるのでフックします。
			// 1036はエイリアスを追加するコマンドです。1はエイリアスのインデックスです。
			::SendMessage(magi.auin.GetExEditWindow(), WM_COMMAND, 1036, 1);
			hookAddAlias = FALSE;
		}

		//
		// フィルタの名前を返します。
		//
		inline static LPCSTR getName(ExEdit::Object* object, int filterIndex)
		{
			if (filterIndex < 0)
			{
				if (object->dispname[0])
					return object->dispname;

				filterIndex = 0;
			}

			ExEdit::Filter* filter = magi.auin.GetFilter(object, filterIndex);
			if (!filter) return "";

			int id = object->filter_param[filterIndex].id;
			if (id == 79) // アニメーション効果
			{
				BYTE* exdata = magi.auin.GetExdata(object, filterIndex);
				LPCSTR name = (LPCSTR)(exdata + 0x04);
				if (!name[0])
				{
					WORD type = *(WORD*)(exdata + 0);
					MY_TRACE_HEX(type);

					WORD filter = *(WORD*)(exdata + 2);
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
				BYTE* exdata = magi.auin.GetExdata(object, filterIndex);
				LPCSTR name = (LPCSTR)(exdata + 0x04);
				if (!name[0]) name = "カスタムオブジェクト";
				return name;
			}
			else
			{
				return filter->name;
			}
		}

		//
		// fileNameの中の使用できない文字を'_'に置き換えます。
		//
		inline static void replaceInvalidChar(LPSTR fileName)
		{
			int c = strlen(fileName);
			for (int i = 0; i < c; i++)
			{
				char ch = fileName[i];
				UINT type = ::PathGetCharType(ch);
				switch (type)
				{
				case GCT_INVALID:
				case GCT_WILD:
					{
						fileName[i] = '_';
						break;
					}
				}
			}
		}

		//
		// ファイル選択ダイアログを出してフィルタをエイリアスファイルに保存します。
		//
		inline static BOOL saveAlias(HWND hwnd, int objectIndex, int filterIndex)
		{
			MY_TRACE_FUNC("0x%08X, %d, %d", hwnd, objectIndex, filterIndex);

			// オブジェクトを取得します。
			ExEdit::Object* object = magi.auin.GetObject(objectIndex);
			if (!object) return FALSE;

			// ダイアログのタイトルとファイル名を取得します。
			char title[MAX_PATH] = {};
			char fileName[MAX_PATH] = {};
			{
				LPCSTR name = getName(object, filterIndex);

				switch (filterIndex)
				{
				case -2:
					{
						::StringCchCopyA(title, std::size(title), "オブジェクトの保存");
						::StringCchPrintfA(fileName, std::size(fileName), "%hs.exa", name);

						break;
					}
				case -1:
					{
						::StringCchCopyA(title, std::size(title), "全フィルタの保存");
						::StringCchPrintfA(fileName, std::size(fileName), "%hsのフィルタ全体.exa", object->dispname);

						break;
					}
				default:
					{
						::StringCchCopyA(title, std::size(title), "フィルタの保存");
						::StringCchPrintfA(fileName, std::size(fileName), "%hs.exa", name);

						break;
					}
				}

				replaceInvalidChar(fileName);
			}

			// フォルダ名を取得します。
			char folderName[MAX_PATH] = {};
			::GetWindowTextA(Share::Filer::FilerWindow::getBrowser(hive.activeWindow), folderName, MAX_PATH);

			OPENFILENAMEA ofn = { sizeof(ofn) };
			ofn.hwndOwner = hwnd;
			ofn.Flags = OFN_OVERWRITEPROMPT;
			ofn.lpstrTitle = title;
			ofn.lpstrInitialDir = folderName;
			ofn.lpstrFile = fileName;
			ofn.nMaxFile = std::size(fileName);
			ofn.lpstrFilter = "エイリアスファイル (*.exa)\0*.exa\0" "すべてのファイル (*.*)\0*.*\0";
			ofn.lpstrDefExt = "exa";

			if (!::GetSaveFileNameA(&ofn))
				return FALSE;

			return magi.auin.SaveFilterAlias(objectIndex, filterIndex, fileName);
		}

		//
		// このクラスは拡張編集のExEditWindowProc()をフックします。
		//
		struct
		{
			inline static BOOL CDECL hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin* fp)
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case 1070: // エイリアスを作成
							{
								MY_TRACE(_T("エイリアスを作成します\n"));

								if (!hive.useCommonDialog)
									break;

								if (magi.auin.GetSelectedObjectCount() > 0)
								{
									int objectIndex = magi.auin.GetSelectedObject(0);

									saveAlias(hwnd, objectIndex, -2);
								}

								return FALSE;
							}
						}

						break;
					}
				}

				return orig(hwnd, message, wParam, lParam, editp, fp);
			}
			inline static decltype(&hook) orig = 0;
		} ExEditWindowProc;

		//
		// このクラスは拡張編集のSettingDialogProc()をフックします。
		//
		struct
		{
			inline static LRESULT WINAPI hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case 1109: // 『設定の保存』→『現在の設定でエイリアスを作成する』
							{
								MY_TRACE(_T("エイリアスを作成します\n"));

								if (!hive.useCommonDialog)
									break;

								int objectIndex = magi.auin.GetCurrentObjectIndex();
								int filterIndex = magi.auin.GetCurrentFilterIndex();

								if (filterIndex <= 0)
								{
									saveAlias(hwnd, objectIndex, -1);
								}
								else
								{
									saveAlias(hwnd, objectIndex, filterIndex);
								}

								return 0;
							}
						}

						break;
					}
				case WM_DROPFILES:
					{
						MY_TRACE(_T("WM_DROPFILES\n"));

						clearAliasFile();
						BOOL result = FALSE;

						HDROP drop = (HDROP)wParam;
						int c = ::DragQueryFile(drop, 0xFFFFFFFF, 0, 0);
						for (int i = 0; i < c; i++)
						{
							char fileName[MAX_PATH] = {};
							::DragQueryFileA(drop, i, fileName, MAX_PATH);
							MY_TRACE_STR(fileName);

							LPCSTR extension = ::PathFindExtensionA(fileName);
							if (_strcmpi(extension, ".exa") == 0)
								result |= addAliasFile(fileName);
						}
						//::DragFinish(drop); // これを呼ぶとデフォルトの処理が実行できなくなる。

						if (result)
						{
							loadAlias();
							return 0;
						}

						break;
					}
				}

				return orig(hwnd, message, wParam, lParam);
			}
			inline static decltype(&hook) orig = 0;
		} SettingDialogProc;

		//
		// このクラスは拡張編集のAddAlias()をフックします。
		//
		struct
		{
			inline static BOOL CDECL hook(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex)
			{
				MY_TRACE_FUNC("%hs, %d, %d, %d", fileName, flag1, flag2, objectIndex);

				if (hookAddAlias)
				{
					// フラグが立っている場合はエイリアスファイル名の配列を使用して
					// アイテムに複数のエイリアスを追加します。

					BOOL retValue = FALSE;

					for (auto fileName : aliasFileNames)
					{
						MY_TRACE_STR(fileName.c_str());

						retValue |= orig(fileName.c_str(), flag1, flag2, objectIndex);

						::DeleteFileA(fileName.c_str());
					}

					return retValue;
				}

				return orig(fileName, flag1, flag2, objectIndex);
			}
			inline static decltype(&hook) orig = 0;
		} AddAlias;
	};
}
