#pragma once

namespace fgo::filter_copy
{
	//
	// このクラスは拡張編集の設定ダイアログに
	// フィルタのコピー・アンド・ペースト機能を追加します。
	//
	inline struct FilterCopy : Servant, Magi::CommandID::SettingDialog
	{
		//
		// コンストラクタです。
		// コンフィグの初期値を設定します。
		//
		FilterCopy()
		{
		}

		//
		// この仮想関数は、このサーヴァントの識別名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_name() override
		{
			return L"FilterCopy";
		}

		//
		// この仮想関数は、このサーヴァントの表示名が必要なときに呼ばれます。
		//
		LPCWSTR get_servant_display_name() override
		{
			return L"フィルタのコピペ";
		}

		//
		// この仮想関数は、初期化のタイミングで呼ばれます。
		//
		BOOL on_init() override
		{
			if (::GetModuleHandleW(L"CopyFilter.auf"))
			{
				::MessageBoxW(magi.fp->hwnd,
					L"FilterCopyとCopyFilter.aufが競合しています"
					L"\nどちらかを除外してからAviUtlを再起動してください",
					get_servant_display_name(), MB_OK | MB_ICONWARNING);
			}

			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// この仮想関数は、後始末のタイミングで呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// コンフィグファイルのフルパスを返します。
		//
		inline static std::wstring getConfigFileName()
		{
			return magi.getConfigFileName(L"FilterCopy.ini");
		}

		//
		// コンフィグファイル名を取得し、設定を読み込みます。
		//
		BOOL load()
		{
			return load(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルから設定を読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// コンフィグファイル名を取得し、設定を保存します。
		//
		BOOL save()
		{
			return save(getConfigFileName().c_str());
		}

		//
		// コンフィグファイルに設定を保存します。
		//
		BOOL save(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// 初期化処理です。
		// 拡張編集の関数をフックします。
		//
		BOOL init()
		{
			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			settingDialogProc.orig = magi.auin.HookSettingDialogProc(settingDialogProc.hook);
			Tools::attach(addAlias, magi.auin.GetAddAlias());
			Tools::attach(unknown1, magi.auin.GetUnknown1());

			return DetourTransactionCommit() == NO_ERROR;
		}

		//
		// 後始末処理です。
		//
		BOOL exit()
		{
			return TRUE;
		}

		//
		// 独自のエイリアス読み込み処理を行うためのフラグです。
		// AddAlias()のフック関数で参照されます。
		//
		BOOL flagPasteFilter = FALSE;

		//
		// このクラスは前処理です。
		// 同じ処理を何度か繰り返すので、事前に変数をキャッシュして準備しておきます。
		//
		struct Prep {
			int objectIndex = -1;
			ExEdit::Object* object = 0;
			char tempFolderPath[MAX_PATH] = {};
			DWORD pid = 0;

			BOOL init()
			{
				// カレントオブジェクトのインデックスを取得します。
				objectIndex = magi.auin.GetCurrentObjectIndex();
				MY_TRACE_INT(objectIndex);
				if (objectIndex < 0) return FALSE;

				// オブジェクトを取得すします。
				object = magi.auin.GetObject(objectIndex);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// テンポラリフォルダのパスを取得します。
				::GetTempPathA(MAX_PATH, tempFolderPath);
				MY_TRACE_STR(tempFolderPath);

				// カレントプロセスのIDを取得します。
				pid = ::GetCurrentProcessId();
				MY_TRACE_INT(pid);

				return TRUE;
			}
		};

		//
		// このクラスはオーダーです。
		//
		struct Order {
			struct Filter {
				int index; // フィルタのインデックス。
				std::string fileName; // エイリアスのファイル名。
			};
			ExEdit::Object::Flag flag; // オブジェクトのフラグ。
			std::vector<Filter> filters; // オーダーされたフィルタのコレクション。

			//
			// オーダーを初期化します。
			//
			BOOL init(const Prep& prep)
			{
				MY_TRACE(_T("Order::init()\n"));

				flag = prep.object->flag;
				filters.clear();
				filters.reserve(ExEdit::Object::MAX_FILTER);

				return TRUE;
			}

			//
			// 指定されたフィルタのコピーをオーダーします。
			// 指定されたフィルタのエイリアスを一時ファイルに保存し、
			// ファイル名をコレクションに追加します。
			//
			BOOL add(const Prep& prep, int filterIndex)
			{
				MY_TRACE(_T("Order::order(%d)\n"), filterIndex);

				if (filterIndex == 0)
					return FALSE; // 先頭のフィルタはコピーしません。

				if (!magi.auin.IsMoveable(prep.object, filterIndex))
					return FALSE; // 移動不可能なフィルタはコピーしません。

				// フィルタを取得する。
				ExEdit::Filter* filter = magi.auin.GetFilter(prep.object, filterIndex);
				if (!filter) return FALSE;

				// 一時ファイルのファイル名を構築します。
				char tempFileName[MAX_PATH] = {};
				::StringCbPrintfA(tempFileName, sizeof(tempFileName),
					"%s\\AviUtl_FilterCopy_%d_%d.exa",
					prep.tempFolderPath, prep.pid, filterIndex);
				MY_TRACE_STR(tempFileName);

				// 一時ファイルにフィルタのエイリアスを保存します。
				if (!magi.auin.SaveFilterAlias(prep.objectIndex, filterIndex, tempFileName))
				{
					MY_TRACE(_T("SaveFilterAlias() が失敗しました\n"));

					return FALSE;
				}

				// フィルタをコレクションに追加します。
				filters.emplace_back(filterIndex, tempFileName);

				return TRUE;
			}
		} order;

		//
		// オブジェクトのタイプを返します。
		//
		static int getType(UINT flag)
		{
			if (flag & 0x00020000) {
				if (flag & 0x00050000)
					return 1; // 音声メディアオブジェクト
				else
					return 2; // 音声フィルタオブジェクト
			} else {
				if (flag & 0x00050000)
					return 3; // 映像メディアオブジェクト＆グループ制御
				else {
					if (flag & 0x00080000)
						return 4; // カメラ制御＆時間制御
					else
						return 5; // 映像フィルタオブジェクト
				}
			}
		}

		//
		// 指定されたフィルタのコピーを発注します。
		//
		BOOL copyFilter(int filterIndex, int flag, BOOL cut)
		{
			// 前準備を行います。
			Prep prep;
			if (!prep.init()) return FALSE;
			if (!order.init(prep)) return FALSE;

			// 戻り値です。
			BOOL retValue = FALSE;

			switch (flag)
			{
			case 0:
				{
					// 指定されたフィルタのコピーを発注します。
					retValue |= order.add(prep, filterIndex);

					break;
				}
			case -1:
				{
					// 指定されたフィルタより上にあるフィルタのコピーを発注します。
					for (int i = 0; i <= filterIndex; i++)
						retValue |= order.add(prep, i);

					break;
				}
			case 1:
				{
					// 指定されたフィルタより下にあるフィルタのコピーを発注します。
					for (int i = filterIndex; i < ExEdit::Object::MAX_FILTER; i++)
						retValue |= order.add(prep, i);

					break;
				}
			}

			if (cut) // カットする場合は、ここでフィルタを削除します。
			{
				// カレントオブジェクトのインデックスを取得します。
				int objectIndex = magi.auin.GetCurrentObjectIndex();
				MY_TRACE_INT(objectIndex);
				if (objectIndex < 0) return FALSE;

				// オブジェクトを取得します。
				ExEdit::Object* object = magi.auin.GetObject(objectIndex);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// 中間点がある場合は先頭オブジェクトのインデックスを使用します。
				if (object->index_midpt_leader != -1)
					objectIndex = object->index_midpt_leader;

				magi.auin.PushUndo();
				magi.auin.CreateUndo(objectIndex, 1);
				for (int i = order.filters.size() - 1; i >= 0; i--)
					magi.auin.DeleteFilter(objectIndex, order.filters[i].index);
				magi.auin.DrawSettingDialog(objectIndex);
				magi.auin.HideControls();
				magi.auin.ShowControls(magi.auin.GetCurrentObjectIndex());
			}

			return retValue;
		}

		//
		// カレントオブジェクトにフィルタを貼り付けます。
		//
		BOOL pasteFilter()
		{
			MY_TRACE(_T("pasteFilter()\n"));

			// カレントオブジェクトのインデックスを取得します。
			int objectIndex = magi.auin.GetCurrentObjectIndex();
			MY_TRACE_INT(objectIndex);
			if (objectIndex < 0) return FALSE;

			// オブジェクトを取得します。
			ExEdit::Object* object = magi.auin.GetObject(objectIndex);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			if (object->filter_param[0].id == 93) // 93 == 時間制御
				return FALSE; // 「時間制御」には貼り付けられるフィルタがないので何もしません。

			// オブジェクトのタイプを取得します。
			UINT srcType = getType((UINT)order.flag);
			UINT dstType = getType((UINT)object->flag);
			MY_TRACE_INT(srcType);
			MY_TRACE_INT(dstType);

			if (srcType != dstType)
				return FALSE; // オブジェクトのタイプが異なる場合は何もしません。

			flagPasteFilter = TRUE;
			// この中で magi.auin.AddAlias() が呼ばれるので、フラグを立ててフックします。
			// 1036 はエイリアスを追加するコマンド、0 はエイリアスのインデックスです。
			::SendMessage(magi.auin.GetExEditWindow(), WM_COMMAND, 1036, 0);
			flagPasteFilter = FALSE;

			return TRUE;
		}

		struct SettingDialogProc {
			//
			// 拡張編集内の設定ダイアログのウィンドウプロシージャと置き換えられます。
			//
			inline static LRESULT WINAPI hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
				case WM_CREATE:
					{
						MY_TRACE(_T("SettingDialogProc(WM_CREATE)\n"));

						// 設定ダイアログのコンテキストメニューを拡張します。
						for (int i = 0; i < magi.auin.GetSettingDialogMenuCount(); i++)
						{
							HMENU menu = magi.auin.GetSettingDialogMenu(i);
							HMENU subMenu = ::GetSubMenu(menu, 0);

							if (i == 2)
							{
								::AppendMenu(subMenu, MF_SEPARATOR, 0, 0);
								::AppendMenu(subMenu, MF_STRING, ID_CREATE_CLONE, _T("完全な複製を下に作成"));
								::AppendMenu(subMenu, MF_STRING, ID_CREATE_SAME_ABOVE, _T("同じフィルタ効果を上に作成"));
								::AppendMenu(subMenu, MF_STRING, ID_CREATE_SAME_BELOW, _T("同じフィルタ効果を下に作成"));
							}

							::AppendMenu(subMenu, MF_SEPARATOR, 0, 0);
							::AppendMenu(subMenu, MF_STRING, ID_CUT_FILTER, _T("このフィルタを切り取り"));
							::AppendMenu(subMenu, MF_STRING, ID_CUT_FILTER_ABOVE, _T("このフィルタ以上を切り取り"));
							::AppendMenu(subMenu, MF_STRING, ID_CUT_FILTER_BELOW, _T("このフィルタ以下を切り取り"));
							::AppendMenu(subMenu, MF_STRING, ID_COPY_FILTER, _T("このフィルタをコピー"));
							::AppendMenu(subMenu, MF_STRING, ID_COPY_FILTER_ABOVE, _T("このフィルタ以上をコピー"));
							::AppendMenu(subMenu, MF_STRING, ID_COPY_FILTER_BELOW, _T("このフィルタ以下をコピー"));
							::AppendMenu(subMenu, MF_STRING, ID_PASTE_FILTER, _T("フィルタを貼り付け"));
						}

						break;
					}
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case ID_CREATE_CLONE:
						case ID_CREATE_SAME_ABOVE:
						case ID_CREATE_SAME_BELOW:
							{
								// オブジェクトを取得する。
								ObjectHolder object(magi.auin.GetCurrentObjectIndex());
								MY_TRACE_OBJECT_HOLDER(object);
								if (!object.isValid()) break;

								// フィルタを取得する。
								FilterHolder filter = FilterHolder(object, magi.auin.GetCurrentFilterIndex());
								MY_TRACE_FILTER_HOLDER(filter);
								if (!filter.isValid()) break;
								MY_TRACE_STR(filter.getFilter()->name);
								MY_TRACE_HEX(filter.getFilter()->flag);

								// フィルタが複製できるものかどうかチェックする。
								if (!filter.isMoveable())
									break;

								// フィルタ ID を取得する。
								int filterId = object.getObject()->filter_param[filter.getFilterIndex()].id;
								MY_TRACE_HEX(filterId);
								if (filterId < 0) break;

								// フィルタを作成するコマンドを発行する。
								servant.unknown1.commandId = wParam;
								LRESULT result = servant.settingDialogProc.orig(hwnd, message, 2000 + filterId, lParam);
								servant.unknown1.commandId = 0;
								return result;
							}
						case ID_CUT_FILTER:
							{
								MY_TRACE(_T("ID_CUT_FILTER\n"));

								int filterIndex = magi.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									servant.copyFilter(filterIndex, 0, TRUE);
								break;
							}
						case ID_CUT_FILTER_ABOVE:
							{
								MY_TRACE(_T("ID_CUT_FILTER_ABOVE\n"));

								int filterIndex = magi.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									servant.copyFilter(filterIndex, -1, TRUE);
								break;
							}
						case ID_CUT_FILTER_BELOW:
							{
								MY_TRACE(_T("ID_CUT_FILTER_BELOW\n"));

								int filterIndex = magi.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									servant.copyFilter(filterIndex, 1, TRUE);
								break;
							}
						case ID_COPY_FILTER:
							{
								MY_TRACE(_T("ID_COPY_FILTER\n"));

								int filterIndex = magi.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									servant.copyFilter(filterIndex, 0, FALSE);
								break;
							}
						case ID_COPY_FILTER_ABOVE:
							{
								MY_TRACE(_T("ID_COPY_FILTER_ABOVE\n"));

								int filterIndex = magi.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									servant.copyFilter(filterIndex, -1, FALSE);
								break;
							}
						case ID_COPY_FILTER_BELOW:
							{
								MY_TRACE(_T("ID_COPY_FILTER_BELOW\n"));

								int filterIndex = magi.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									servant.copyFilter(filterIndex, 1, FALSE);
								break;
							}
						case ID_PASTE_FILTER:
							{
								MY_TRACE(_T("ID_PASTE_FILTER\n"));

								servant.pasteFilter();

								break;
							}
						}

						break;
					}
				}

				return orig(hwnd, message, wParam, lParam);
			}
			inline static decltype(&hook) orig = 0;
		} settingDialogProc;

		struct AddAlias {
			//
			// 拡張編集内の AddAlias() 関数と置き換えられます。
			//
			inline static BOOL CDECL hook(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex)
			{
				return servant.onAddAlias(fileName, flag1, flag2, objectIndex);
			}
			inline static decltype(&hook) orig = 0;
		} addAlias;

		BOOL onAddAlias(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex)
		{
			MY_TRACE(_T("AddAlias(%hs, %d, %d, %d)\n"), fileName, flag1, flag2, objectIndex);

			if (!flagPasteFilter) // フラグが立っていない場合はデフォルトの処理を行います。
				return addAlias.orig(fileName, flag1, flag2, objectIndex);

			// オブジェクトを取得します。
			ExEdit::Object* object = magi.auin.GetObject(objectIndex);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			// カレントフィルタを取得します。
			int filterIndex = magi.auin.GetCurrentFilterIndex();
			MY_TRACE_INT(filterIndex);
			if (filterIndex < 0) return FALSE;

			int insertPos = filterIndex; // フィルタを挿入する位置です。
			BOOL retValue = FALSE; // 戻り値です。

			for (const auto& filter : order.filters)
			{
				MY_TRACE_STR(filter.fileName.c_str());

				// フィルタを末尾に追加します。
				int result = addAlias.orig(filter.fileName.c_str(), flag1, flag2, objectIndex);

				int c = magi.auin.GetMoveableFilterCount(object);

				// 末尾に追加されたフィルタを挿入位置まで移動します。
				for (int i = c - 1; i > insertPos + 1; i--)
				{
					ExEdit::Filter* filter = magi.auin.GetFilter(object, i);

					magi.auin.SwapFilter(objectIndex, i, -1);
				}

				insertPos++;
				retValue = TRUE;
			}

			return retValue;
		}

		struct Unknown1
		{
			UINT commandId = 0;

			void createClone(int origObjectIndex, int newFilterIndex)
			{
				MY_TRACE(_T("複製を作成します\n"));

				int objectIndex = origObjectIndex;
				MY_TRACE_INT(objectIndex);

				int midptLeader = magi.auin.GetObject(objectIndex)->index_midpt_leader;
				MY_TRACE_INT(midptLeader);
				if (midptLeader >= 0)
					objectIndex = midptLeader; // 中間点がある場合は中間点元のオブジェクト ID を取得

				while (objectIndex >= 0)
				{
					// オブジェクトインデックスを取得する。
					MY_TRACE_INT(objectIndex);
					if (objectIndex < 0) break;

					// オブジェクトを取得する。
					ExEdit::Object* object = magi.auin.GetObject(objectIndex);
					MY_TRACE_HEX(object);
					if (!object) break;

					int midptLeader2 = object->index_midpt_leader;
					MY_TRACE_INT(midptLeader2);
					if (midptLeader2 != midptLeader) break;

					// コピー元フィルタのインデックスを取得する。
					int srcFilterIndex = magi.auin.GetCurrentFilterIndex();
					MY_TRACE_INT(srcFilterIndex);
					if (srcFilterIndex < 0) break;

					// コピー先フィルタのインデックスを取得する。
					int dstFilterIndex = newFilterIndex;
					MY_TRACE_INT(dstFilterIndex);
					if (dstFilterIndex < 0) break;

					// コピー元フィルタを取得する。
					ExEdit::Filter* srcFilter = magi.auin.GetFilter(object, srcFilterIndex);
					MY_TRACE_HEX(srcFilter);
					if (!srcFilter) break;

					// コピー先フィルタを取得する。
					ExEdit::Filter* dstFilter = magi.auin.GetFilter(object, dstFilterIndex);
					MY_TRACE_HEX(dstFilter);
					if (!dstFilter) break;

					if (commandId == ID_CREATE_CLONE)
					{
						// 拡張データをコピーする。
						BYTE* srcFilterExdata = magi.auin.GetExdata(object, srcFilterIndex);
						BYTE* dstFilterExdata = magi.auin.GetExdata(object, dstFilterIndex);
						memcpy(dstFilterExdata, srcFilterExdata, srcFilter->exdata_size);

						// トラックデータをコピーする。
						for (int i = 0; i < srcFilter->track_n; i++)
						{
							int srcTrackIndex = object->filter_param[srcFilterIndex].track_begin + i;
							int dstTrackIndex = object->filter_param[dstFilterIndex].track_begin + i;
							object->track_value_left[dstTrackIndex] = object->track_value_left[srcTrackIndex];
							object->track_value_right[dstTrackIndex] = object->track_value_right[srcTrackIndex];
							object->track_mode[dstTrackIndex] = object->track_mode[srcTrackIndex];
							object->track_param[dstTrackIndex] = object->track_param[srcTrackIndex];
						}

						// チェックデータをコピーする。
						for (int i = 0; i < srcFilter->check_n; i++)
						{
							int srcCheckIndex = object->filter_param[srcFilterIndex].check_begin + i;
							int dstCheckIndex = object->filter_param[dstFilterIndex].check_begin + i;
							object->check_value[dstCheckIndex] = object->check_value[srcCheckIndex];
						}
					}

					if (midptLeader < 0) break;

					objectIndex = magi.auin.GetNextObjectIndex(objectIndex);
				}

				// コピー元フィルタのインデックスを取得する。
				int srcFilterIndex = magi.auin.GetCurrentFilterIndex();
				MY_TRACE_INT(srcFilterIndex);
				if (srcFilterIndex < 0) return;

				// コピー先フィルタのインデックスを取得する。
				int dstFilterIndex = newFilterIndex;
				MY_TRACE_INT(dstFilterIndex);
				if (dstFilterIndex < 0) return;

				switch (commandId)
				{
				case ID_CREATE_SAME_ABOVE:
					{
						// コピー元のすぐ上に移動
						int c = dstFilterIndex - srcFilterIndex;
						for (int i = 0; i < c; i++)
							magi.auin.SwapFilter(origObjectIndex, dstFilterIndex--, -1);

						break;
					}
				case ID_CREATE_CLONE:
				case ID_CREATE_SAME_BELOW:
					{
						// コピー元のすぐ下に移動
						int c = dstFilterIndex - srcFilterIndex - 1;
						for (int i = 0; i < c; i++)
							magi.auin.SwapFilter(origObjectIndex, dstFilterIndex--, -1);

						break;
					}
				}
			}

			inline static void CDECL hook(int objectIndex, int filterIndex)
			{
				MY_TRACE(_T("Unknown1(%d, %d)\n"), objectIndex, filterIndex);

				orig(objectIndex, filterIndex);

				if (servant.unknown1.commandId)
					servant.unknown1.createClone(objectIndex, filterIndex);
			}
			inline static decltype(&hook) orig = 0;
		} unknown1;
	} servant;
}
