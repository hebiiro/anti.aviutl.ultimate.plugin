#pragma once
#include "Tools/edit_predicates.h"
#include "Tools/Hook.h"
#include "Hive.h"
#include "Servant.h"

namespace aut
{
	//
	// このクラスはエディットボックスを微調整するサーヴァントです。
	//
	inline struct FilterCopy : Servant
	{
		//
		// このサーヴァントを識別するための名前を返します。
		//
		inline static LPCWSTR getServantNameStatic() { return L"FilterCopy"; }

		//
		// コンストラクタです。
		// コンフィグの初期値を設定します。
		//
		FilterCopy()
		{
		}

		//
		// マスターから呼ばれます。
		//
		BOOL on_init() override
		{
			if (!load()) return FALSE;
			if (!init()) return FALSE;
			return TRUE;
		}

		//
		// マスターから呼ばれます。
		//
		BOOL on_exit() override
		{
			if (!save()) return FALSE;
			if (!exit()) return FALSE;
			return TRUE;
		}

		//
		// 設定をコンフィグファイルから読み込みます。
		//
		BOOL load()
		{
			return load(hive.getConfigFileName(L"FilterCopy.ini").c_str());
		}

		//
		// 設定をコンフィグファイルから読み込みます。
		//
		BOOL load(LPCWSTR path)
		{
			return TRUE;
		}

		//
		// 設定をコンフィグファイルに保存します。
		//
		BOOL save()
		{
			return save(hive.getConfigFileName(L"FilterCopy.ini").c_str());
		}

		//
		// 設定をコンフィグファイルに保存します。
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
			using namespace Tools;

			DetourTransactionBegin();
			DetourUpdateThread(::GetCurrentThread());

			settingDialogProc.orig = hive.auin.HookSettingDialogProc(settingDialogProc.hook);
			addAlias.attach(hive.auin.GetAddAlias());

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
		// AddAlias() のフック関数で参照されます。
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
				objectIndex = hive.auin.GetCurrentObjectIndex();
				MY_TRACE_INT(objectIndex);
				if (objectIndex < 0) return FALSE;

				// オブジェクトを取得すします。
				object = hive.auin.GetObject(objectIndex);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// テンポラリフォルダのパスを取得します。
				::GetTempPathA(MAX_PATH, tempFolderPath);
				MY_TRACE_STR(tempFolderPath);

				// カレントプロセスの ID を取得します。
				pid = ::GetCurrentProcessId();
				MY_TRACE_INT(pid);

				return TRUE;
			}
		};

		//
		// このクラスはオーダーです。
		//
		struct GrandOrder {
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
				MY_TRACE(_T("GrandOrder::init()\n"));

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
				MY_TRACE(_T("GrandOrder::order(%d)\n"), filterIndex);

				if (filterIndex == 0)
					return FALSE; // 先頭のフィルタはコピーしません。

				if (!hive.auin.IsMoveable(prep.object, filterIndex))
					return FALSE; // 移動不可能なフィルタはコピーしません。

				// フィルタを取得する。
				ExEdit::Filter* filter = hive.auin.GetFilter(prep.object, filterIndex);
				if (!filter) return FALSE;

				// 一時ファイルのファイル名を構築します。
				char tempFileName[MAX_PATH] = {};
				::StringCbPrintfA(tempFileName, sizeof(tempFileName),
					"%s\\AviUtl_FilterCopy_%d_%d.exa",
					prep.tempFolderPath, prep.pid, filterIndex);
				MY_TRACE_STR(tempFileName);

				// 一時ファイルにフィルタのエイリアスを保存します。
				if (!hive.auin.SaveFilterAlias(prep.objectIndex, filterIndex, tempFileName))
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
				int objectIndex = hive.auin.GetCurrentObjectIndex();
				MY_TRACE_INT(objectIndex);
				if (objectIndex < 0) return FALSE;

				// オブジェクトを取得します。
				ExEdit::Object* object = hive.auin.GetObject(objectIndex);
				MY_TRACE_HEX(object);
				if (!object) return FALSE;

				// 中間点がある場合は先頭オブジェクトのインデックスを使用します。
				if (object->index_midpt_leader != -1)
					objectIndex = object->index_midpt_leader;

				hive.auin.PushUndo();
				hive.auin.CreateUndo(objectIndex, 1);
				for (int i = order.filters.size() - 1; i >= 0; i--)
					hive.auin.DeleteFilter(objectIndex, order.filters[i].index);
				hive.auin.DrawSettingDialog(objectIndex);
				hive.auin.HideControls();
				hive.auin.ShowControls(hive.auin.GetCurrentObjectIndex());
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
			int objectIndex = hive.auin.GetCurrentObjectIndex();
			MY_TRACE_INT(objectIndex);
			if (objectIndex < 0) return FALSE;

			// オブジェクトを取得します。
			ExEdit::Object* object = hive.auin.GetObject(objectIndex);
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
			// この中で hive.auin.AddAlias() が呼ばれるので、フラグを立ててフックします。
			// 1036 はエイリアスを追加するコマンド、0 はエイリアスのインデックスです。
			::SendMessage(hive.auin.GetExEditWindow(), WM_COMMAND, 1036, 0);
			flagPasteFilter = FALSE;

			return TRUE;
		}

		struct SettingDialogProc {
			//
			// 拡張編集内の設定ダイアログのウィンドウプロシージャと置き換えられます。
			//
			static LRESULT WINAPI hook(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
			{
				switch (message)
				{
				case WM_CREATE:
					{
						MY_TRACE(_T("SettingDialogProc(WM_CREATE)\n"));

						// 設定ダイアログのコンテキストメニューを拡張します。
						for (int i = 0; i < hive.auin.GetSettingDialogMenuCount(); i++)
						{
							HMENU menu = hive.auin.GetSettingDialogMenu(i);
							HMENU subMenu = ::GetSubMenu(menu, 0);
							::AppendMenu(subMenu, MF_SEPARATOR, 0, 0);
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_CUT_FILTER, _T("このフィルタを切り取り"));
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_CUT_FILTER_ABOVE, _T("このフィルタ以上を切り取り"));
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_CUT_FILTER_BELOW, _T("このフィルタ以下を切り取り"));
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_COPY_FILTER, _T("このフィルタをコピー"));
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_COPY_FILTER_ABOVE, _T("このフィルタ以上をコピー"));
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_COPY_FILTER_BELOW, _T("このフィルタ以下をコピー"));
							::AppendMenu(subMenu, MF_STRING, Hive::CommandID::SettingDialog::ID_PASTE_FILTER, _T("フィルタを貼り付け"));
						}

						break;
					}
				case WM_COMMAND:
					{
						switch (wParam)
						{
						case Hive::CommandID::SettingDialog::ID_CUT_FILTER:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_CUT_FILTER\n"));

								int filterIndex = hive.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									filter_copy.copyFilter(filterIndex, 0, TRUE);
								break;
							}
						case Hive::CommandID::SettingDialog::ID_CUT_FILTER_ABOVE:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_CUT_FILTER_ABOVE\n"));

								int filterIndex = hive.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									filter_copy.copyFilter(filterIndex, -1, TRUE);
								break;
							}
						case Hive::CommandID::SettingDialog::ID_CUT_FILTER_BELOW:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_CUT_FILTER_BELOW\n"));

								int filterIndex = hive.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									filter_copy.copyFilter(filterIndex, 1, TRUE);
								break;
							}
						case Hive::CommandID::SettingDialog::ID_COPY_FILTER:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_COPY_FILTER\n"));

								int filterIndex = hive.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									filter_copy.copyFilter(filterIndex, 0, FALSE);
								break;
							}
						case Hive::CommandID::SettingDialog::ID_COPY_FILTER_ABOVE:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_COPY_FILTER_ABOVE\n"));

								int filterIndex = hive.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									filter_copy.copyFilter(filterIndex, -1, FALSE);
								break;
							}
						case Hive::CommandID::SettingDialog::ID_COPY_FILTER_BELOW:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_COPY_FILTER_BELOW\n"));

								int filterIndex = hive.auin.GetCurrentFilterIndex();
								if (filterIndex >= 0)
									filter_copy.copyFilter(filterIndex, 1, FALSE);
								break;
							}
						case Hive::CommandID::SettingDialog::ID_PASTE_FILTER:
							{
								MY_TRACE(_T("Hive::CommandID::SettingDialog::ID_PASTE_FILTER\n"));

								filter_copy.pasteFilter();

								break;
							}
						}

						break;
					}
				}

				return filter_copy.settingDialogProc.orig(hwnd, message, wParam, lParam);
			}
		}; Tools::Hook<SettingDialogProc> settingDialogProc;

		struct AddAlias {
			//
			// 拡張編集内の AddAlias() 関数と置き換えられます。
			//
			static BOOL CDECL hook(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex)
			{
				return filter_copy.onAddAlias(fileName, flag1, flag2, objectIndex);
			}
		}; Tools::Hook<AddAlias> addAlias;

		BOOL onAddAlias(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex)
		{
			MY_TRACE(_T("AddAlias(%s, %d, %d, %d)\n"), fileName, flag1, flag2, objectIndex);

			if (!flagPasteFilter) // フラグが立っていない場合はデフォルトの処理を行います。
				return addAlias.orig(fileName, flag1, flag2, objectIndex);

			// オブジェクトを取得します。
			ExEdit::Object* object = hive.auin.GetObject(objectIndex);
			MY_TRACE_HEX(object);
			if (!object) return FALSE;

			// カレントフィルタを取得します。
			int filterIndex = hive.auin.GetCurrentFilterIndex();
			MY_TRACE_INT(filterIndex);
			if (filterIndex < 0) return FALSE;

			int insertPos = filterIndex; // フィルタを挿入する位置です。
			BOOL retValue = FALSE; // 戻り値です。

			for (const auto& filter : order.filters)
			{
				MY_TRACE_STR(filter.fileName.c_str());

				// フィルタを末尾に追加します。
				int result = addAlias.orig(filter.fileName.c_str(), flag1, flag2, objectIndex);

				int c = hive.auin.GetMoveableFilterCount(object);

				// 末尾に追加されたフィルタを挿入位置まで移動します。
				for (int i = c - 1; i > insertPos + 1; i--)
				{
					ExEdit::Filter* filter = hive.auin.GetFilter(object, i);

					hive.auin.SwapFilter(objectIndex, i, -1);
				}

				insertPos++;
				retValue = TRUE;
			}

			return retValue;
		}
	} filter_copy;
}
