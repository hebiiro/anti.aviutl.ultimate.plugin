#pragma once

class AviUtlInternal
{
private:

	uintptr_t m_aviutl = 0;
	AviUtl::ExFunc* m_exfunc = 0;

	typedef BOOL (CDECL* Type_get_sys_info)(void* editp, AviUtl::SysInfo* sip);
	Type_get_sys_info m_get_sys_info = 0;

public:

	uintptr_t m_exedit = 0;
	HWND* m_aviutlWindow = 0; // AviUtl ウィンドウ。
	HWND* m_exeditWindow = 0; // 拡張編集ウィンドウ。
	HWND* m_settingDialog = 0; // 設定ダイアログ。
	ExEdit::Object** m_objectTable = 0;
	ExEdit::Filter** m_filterTable = 0;
	int* m_currentSceneIndex = 0; // カレントシーンのインデックス。
	int* m_objectIndex = 0; // カレントオブジェクトのインデックスへのポインタ。
	int* m_filterIndex = 0; // カレントフィルタのインデックスへのポインタ。
	int* m_objectCount = 0; // オブジェクトの数。
	int* m_currentSceneObjectCount = 0; // カレントシーンのオブジェクトの数。
	ExEdit::Object** m_objectData = 0; // オブジェクトデータへのポインタ。
	BYTE** m_objectExdata = 0; // オブジェクト拡張データへのポインタ。
	int* m_nextObject = 0; // 次のオブジェクトの配列。
	int* m_selectedObjects = 0; // 選択オブジェクトの配列。
	int* m_selectedObjectCount = 0; // 選択オブジェクトの数。
	ExEdit::SceneSetting* m_sceneSettingTable = 0; // シーン設定の配列。
	AviUtl::EditHandle** m_editp = 0; // editp へのポインタ。
	int* m_layerWidth = 0; // レイヤーの幅。
	int* m_layerHeight = 0; // レイヤーの高さ。
	int* m_layerVisibleCount = 0; // UI 上で表示されているレイヤーの数。
	ExEdit::LayerSetting** m_layerSettingArray = 0; // レイヤー設定の配列。
	int* m_aviutlFrameNumber = 0; // AviUtl の最終フレーム番号。
	int* m_exeditFrameNumber = 0; // 拡張編集の最終フレーム番号。
	int* m_exeditCurrentFrame = 0; // 拡張編集の現在フレーム。
	HMENU* m_settingDialogMenu[5] = {}; // 設定ダイアログのコンテキストメニュー。
	BYTE* m_positionDataArray = 0;

	using Type_ExEditWindowProc = decltype(AviUtl::FilterPlugin::func_WndProc);
//	typedef BOOL (CDECL* Type_ExEditWindowProc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam, AviUtl::EditHandle* editp, AviUtl::FilterPlugin *fp);
	typedef WNDPROC Type_SettingDialogProc;
//	typedef LRESULT (WINAPI* Type_SettingDialogProc)(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	typedef __int64 (CDECL* Type_FrameToX)(int frame);
	Type_FrameToX m_FrameToX = 0;

	typedef void (CDECL* Type_PushUndo)();
	Type_PushUndo m_PushUndo = 0;

	typedef void (CDECL* Type_CreateUndo)(int objectIndex, UINT flags);
	Type_CreateUndo m_CreateUndo = 0;

	typedef void (CDECL* Type_HideControls)();
	Type_HideControls m_HideControls = 0;

	typedef BOOL (CDECL* Type_ShowControls)(int objectIndex);
	Type_ShowControls m_ShowControls = 0;

	typedef void (CDECL* Type_DrawObject)(HDC dc, int objectIndex);
	Type_DrawObject m_DrawObject = 0;

	typedef void (CDECL* Type_DrawSettingDialog)(int objectIndex);
	Type_DrawSettingDialog m_DrawSettingDialog = 0;

	typedef void (CDECL* Type_DeleteFilter)(int objectIndex, int filterIndex);
	Type_DeleteFilter m_DeleteFilter = 0;

	typedef void (CDECL* Type_SwapFilter)(int objectIndex, int filterIndex, int relativeIndex);
	Type_SwapFilter m_SwapFilter = 0;

	typedef void (CDECL* Type_Unknown1)(int objectIndex, int filterIndex);
	Type_Unknown1 m_Unknown1 = 0;

	typedef LPCSTR (CDECL* Type_GetAliasFileName)(int aliasId);
	Type_GetAliasFileName m_GetAliasFileName = 0;

	typedef int (CDECL* Type_AddAlias)(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex);
	Type_AddAlias m_AddAlias = 0;

	typedef BOOL (CDECL* Type_SaveFilterAlias)(int objectIndex, int filterIndex, LPCSTR fileName);
	Type_SaveFilterAlias m_SaveFilterAlias = 0;

	typedef BOOL (CDECL* Type_LoadExo)(LPCSTR fileName, int frame, int layer, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp);
	Type_LoadExo m_LoadExo = 0;

	typedef BOOL (CDECL* Type_SaveExo)(LPCSTR fileName);
	Type_SaveExo m_SaveExo = 0;

	typedef BOOL (CDECL* Type_SetScene)(int sceneIndex, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp);
	Type_SetScene m_SetScene = 0;

	typedef void (CDECL* Type_RedrawLayer)(int layerIndex);
	Type_RedrawLayer m_RedrawLayer = 0;

	typedef void (CDECL* Type_RedrawLayers)(int flags[]);
	Type_RedrawLayers m_RedrawLayers = 0;

	typedef int (CDECL* Type_DeleteMidPoint)(int objectIndex, int frame);
	Type_DeleteMidPoint m_DeleteMidPoint = 0;

public:

	BOOL initAviUtlAddress()
	{
		m_aviutl = (uintptr_t)::GetModuleHandle(0);

		if (!m_aviutl)
			return FALSE;

		m_exfunc = (AviUtl::ExFunc*)(m_aviutl + 0xA8C78);
		m_get_sys_info = (Type_get_sys_info)(m_aviutl + 0x22120);

		return TRUE;
	}

	BOOL initExEditAddress()
	{
		m_exedit = (uintptr_t)::GetModuleHandle(_T("exedit.auf"));

		if (!m_exedit)
			return FALSE;

		m_aviutlWindow = (HWND*)(m_exedit + 0x135C6C);
		m_exeditWindow = (HWND*)(m_exedit + 0x177A44);
		m_settingDialog = (HWND*)(m_exedit + 0x1539C8);
		m_objectTable = (ExEdit::Object**)(m_exedit + 0x168FA8);
		m_filterTable = (ExEdit::Filter**)(m_exedit + 0x187C98);
		m_currentSceneIndex = (int*)(m_exedit + 0x1A5310);
		m_objectIndex = (int*)(m_exedit + 0x177A10);
		m_filterIndex = (int*)(m_exedit + 0x14965C);
		m_objectCount = (int*)(m_exedit + 0x146250);
		m_currentSceneObjectCount = (int*)(m_exedit + 0x15918C);
		m_objectData = (ExEdit::Object**)(m_exedit + 0x1E0FA4);
		m_objectExdata = (BYTE**)(m_exedit + 0x1E0FA8);
		m_nextObject = (int*)(m_exedit + 0x1592d8);
		m_selectedObjects = (int*)(m_exedit + 0x179230);
		m_selectedObjectCount = (int*)(m_exedit + 0x167D88);
		m_sceneSettingTable = (ExEdit::SceneSetting*)(m_exedit + 0x177A50);
		m_editp = (AviUtl::EditHandle**)(m_exedit + 0x1a532c);
		m_layerWidth = (int*)(m_exedit + 0x1A52FC);
		m_layerHeight = (int*)(m_exedit + 0xA3E20);
		m_layerVisibleCount = (int*)(m_exedit + 0xA3FBC);
		m_layerSettingArray = (ExEdit::LayerSetting**)(m_exedit + 0xA4058);
		m_aviutlFrameNumber = (int*)(m_exedit + 0x14D3A0);
		m_exeditFrameNumber = (int*)(m_exedit + 0x1A5318);
		m_exeditCurrentFrame = (int*)(m_exedit + 0x1A5304);

		m_settingDialogMenu[0] = (HMENU*)(m_exedit + 0x158D20); // 映像フィルタオブジェクト
		m_settingDialogMenu[1] = (HMENU*)(m_exedit + 0x158D24); // 音声メディアオブジェクト
		m_settingDialogMenu[2] = (HMENU*)(m_exedit + 0x158D2C); // 映像メディアオブジェクト＆グループ制御
		m_settingDialogMenu[3] = (HMENU*)(m_exedit + 0x167D40); // カメラ制御＆時間制御
		m_settingDialogMenu[4] = (HMENU*)(m_exedit + 0x167D44); // 音声フィルタオブジェクト

		m_positionDataArray = (BYTE*)(m_exedit + 0x146270);

		m_PushUndo = (Type_PushUndo)(m_exedit + 0x0008D150);
		m_CreateUndo = (Type_CreateUndo)(m_exedit + 0x0008D290);
		m_HideControls = (Type_HideControls)(m_exedit + 0x00030500);
		m_ShowControls = (Type_ShowControls)(m_exedit + 0x000305E0);
		m_FrameToX = (Type_FrameToX)(m_exedit + 0x00032BD0);
		m_DrawObject = (Type_DrawObject)(m_exedit + 0x00037060);
		m_DrawSettingDialog = (Type_DrawSettingDialog)(m_exedit + 0x00039490);
		m_DeleteFilter = (Type_DeleteFilter)(m_exedit + 0x33D20);
		m_SwapFilter = (Type_SwapFilter)(m_exedit + 0x33B30);
		m_Unknown1 = (Type_Unknown1)(m_exedit + 0x34FF0);
		m_GetAliasFileName = (Type_GetAliasFileName)(m_exedit + 0x43FD0);
		m_AddAlias = (Type_AddAlias)(m_exedit + 0x29DC0);
		m_SaveFilterAlias = (Type_SaveFilterAlias)(m_exedit + 0x28CA0);
		m_LoadExo = (Type_LoadExo)(m_exedit + 0x4DCA0);
		m_SaveExo = (Type_SaveExo)(m_exedit + 0x284D0);
		m_SetScene = (Type_SetScene)(m_exedit + 0x2BA60);
		m_RedrawLayer = (Type_RedrawLayer)(m_exedit + 0x39290);
		m_RedrawLayers = (Type_RedrawLayers)(m_exedit + 0x392F0);
		m_DeleteMidPoint = (Type_DeleteMidPoint)(m_exedit + 0x34A30);

		return TRUE;
	}

	Type_ExEditWindowProc HookExEditWindowProc(AviUtl::FilterPlugin* fp, Type_ExEditWindowProc newProc)
	{
		AviUtl::FilterPlugin* exedit = GetFilter(fp, "拡張編集");
		Type_ExEditWindowProc oldProc = exedit->func_WndProc;
		exedit->func_WndProc = newProc;
		return oldProc;
	}

	Type_SettingDialogProc HookSettingDialogProc(Type_SettingDialogProc newProc)
	{
		return writeAbsoluteAddress(m_exedit + 0x2E800 + 4, newProc);
	}

public: // AviUtl の変数。

	uintptr_t GetAviUtl() { return m_aviutl; }
	AviUtl::ExFunc* GetExFunc() { return m_exfunc; }

public: // AviUtl の関数。

	Type_get_sys_info Get_get_sys_info() { return m_get_sys_info; }
	BOOL get_sys_info(void* editp, AviUtl::SysInfo* sip) { return m_get_sys_info(editp, sip); }

	static BOOL isPlaying(AviUtl::FilterProcInfo* fpip)
	{
		return !!((UINT)fpip->editp->aviutl_window_info.flag & 0x00040000);
	}

public:

	uintptr_t GetExEdit()
	{
		return m_exedit;
	}

	HWND GetAviUtlWindow()
	{
		return *m_aviutlWindow;
	}

	HWND GetExEditWindow()
	{
		return *m_exeditWindow;
	}

	HWND GetSettingDialog()
	{
		return *m_settingDialog;
	}

	int GetCurrentSceneIndex()
	{
		return *m_currentSceneIndex;
	}

	int GetCurrentObjectIndex()
	{
		return *m_objectIndex;
	}

	int GetCurrentFilterIndex()
	{
		return *m_filterIndex;
	}

	int GetObjectCount()
	{
		return *m_objectCount;
	}

	int GetCurrentSceneObjectCount()
	{
		return *m_currentSceneObjectCount;
	}

	ExEdit::Object* GetObject(int objectIndex)
	{
		return *m_objectData + objectIndex;
	}

	ExEdit::Object* GetSortedObject(int objectIndex)
	{
		return m_objectTable[objectIndex];
	}

	ExEdit::Filter* GetFilter(int filterId)
	{
		return m_filterTable[filterId];
	}

	ExEdit::Filter* GetFilter(ExEdit::Object* object, int filterIndex)
	{
		if (!object) return 0;
		int id = object->filter_param[filterIndex].id;
		if (id < 0) return 0;
		return GetFilter(id);
	}

	int GetNextObjectIndex(int objectIndex)
	{
		return m_nextObject[objectIndex];
	}

	int GetSelectedObject(int i)
	{
		return m_selectedObjects[i];
	}

	int GetSelectedObjectCount()
	{
		return *m_selectedObjectCount;
	}

	ExEdit::SceneSetting* GetSceneSetting(int sceneIndex)
	{
		return m_sceneSettingTable + sceneIndex;
	}

	AviUtl::EditHandle* GetEditp()
	{
		return *m_editp;
	}

	int GetLayerWidth()
	{
		return *m_layerWidth;
	}

	int GetLayerHeight()
	{
		return *m_layerHeight;
	}

	int GetLayerVisibleCount()
	{
		return *m_layerVisibleCount;
	}

	ExEdit::LayerSetting* GetLayerSetting(int layerIndex)
	{
		return (*m_layerSettingArray) + layerIndex;
	}

	int GetAviUtlFrameNumber()
	{
		return *m_aviutlFrameNumber;
	}

	int GetExEditFrameNumber()
	{
		return *m_exeditFrameNumber;
	}

	int GetExEditCurrentFrame()
	{
		return *m_exeditCurrentFrame;
	}

	void SetExEditCurrentFrame(int frame)
	{
		*m_exeditCurrentFrame = frame;
	}

	HMENU GetSettingDialogMenu(int index)
	{
		return *m_settingDialogMenu[index];
	}

	int GetSettingDialogMenuCount()
	{
		return sizeof(m_settingDialogMenu) / sizeof(m_settingDialogMenu[0]);
	}

	BYTE* GetPositionDataArray()
	{
		return m_positionDataArray;
	}

	BYTE* GetExdata(ExEdit::Object* object, int filterIndex)
	{
		BYTE* exdataTable = *m_objectExdata;
		DWORD offset = ExdataOffset(object, filterIndex);
		return exdataTable + offset + 0x0004;
	}

public:

	static int GetFilterIndex(ExEdit::Object* object, int filterId)
	{
		if (!object) return -1;

		int c = object->countFilters();

		for (int i = 0; i < c; i++)
		{
			if (object->filter_param[i].id == filterId)
				return i;
		}

		return -1;
	}

	static DWORD ExdataOffset(ExEdit::Object* object, int idx)
	{
		return object->exdata_offset + object->filter_param[idx].exdata_offset;
	}

	static BOOL IsMoveable(ExEdit::Object* object, int filterIndex)
	{
		int id = object->filter_param[filterIndex].id;
		switch (id)
		{
		case 0x00: // 動画ファイル
		case 0x01: // 画像ファイル
		case 0x02: // 音声ファイル
		case 0x03: // テキスト
		case 0x04: // 図形
		case 0x05: // フレームバッファ
		case 0x06: // 音声波形
		case 0x07: // シーン
		case 0x08: // シーン(音声)
		case 0x09: // 直前オブジェクト
		case 0x0A: // 標準描画
		case 0x0B: // 拡張描画
		case 0x0C: // 標準再生
		case 0x0D: // パーティクル出力
		case 0x50: // カスタムオブジェクト
		case 0x5D: // 時間制御
		case 0x5E: // グループ制御
		case 0x5F: // カメラ制御
		case -1:
			{
				return FALSE;
			}
		}
		return TRUE;
	}

	static int GetMoveableFilterCount(ExEdit::Object* object)
	{
		for (int i = 0; i < ExEdit::Object::MAX_FILTER; i++)
		{
			if (object->filter_param[i].id < 0)
				return i;

			if (i != 0 && !IsMoveable(object, i))
				return i;
		}
		return ExEdit::Object::MAX_FILTER;
	}

	static int GetFilterNumber(AviUtl::FilterPlugin *fp)
	{
		AviUtl::SysInfo si;
		fp->exfunc->get_sys_info(NULL, &si);
		return si.filter_n;
	}

	static AviUtl::FilterPlugin* GetFilter(AviUtl::FilterPlugin *fp, LPCSTR name)
	{
		for (int i = GetFilterNumber(fp) - 1; i >= 0; i--)
		{
			AviUtl::FilterPlugin* temp = (AviUtl::FilterPlugin*)fp->exfunc->get_filterp(i);
			if (!strcmp(temp->name, name)) return temp;
		}
		return NULL;
	}

public:

	Type_FrameToX GetFrameToX()
	{
		return m_FrameToX;
	}

	Type_PushUndo GetPushUndo()
	{
		return m_PushUndo;
	}

	Type_CreateUndo GetCreateUndo()
	{
		return m_CreateUndo;
	}

	Type_HideControls GetHideControls()
	{
		return m_HideControls;
	}

	Type_ShowControls GetShowControls()
	{
		return m_ShowControls;
	}

	Type_DrawObject GetDrawObject()
	{
		return m_DrawObject;
	}

	Type_DrawSettingDialog GetDrawSettingDialog()
	{
		return m_DrawSettingDialog;
	}

	Type_DeleteFilter GetDeleteFilter()
	{
		return m_DeleteFilter;
	}

	Type_SwapFilter GetSwapFilter()
	{
		return m_SwapFilter;
	}

	Type_Unknown1 GetUnknown1()
	{
		return m_Unknown1;
	}

	Type_GetAliasFileName GetGetAliasFileName()
	{
		return m_GetAliasFileName;
	}

	Type_AddAlias GetAddAlias()
	{
		return m_AddAlias;
	}

	Type_SaveFilterAlias GetSaveFilterAlias()
	{
		return m_SaveFilterAlias;
	}

	Type_LoadExo GetLoadExo()
	{
		return m_LoadExo;
	}

	Type_SaveExo GetSaveExo()
	{
		return m_SaveExo;
	}

	Type_SetScene GetSetScene()
	{
		return m_SetScene;
	}

	Type_RedrawLayer GetRedrawLayer()
	{
		return m_RedrawLayer;
	}

	Type_RedrawLayers GetRedrawLayers()
	{
		return m_RedrawLayers;
	}

	Type_DeleteMidPoint GetDeleteMidPoint()
	{
		return m_DeleteMidPoint;
	}

public:

	__int64 FrameToX(int frame)
	{
		return m_FrameToX(frame);
	}

	void PushUndo()
	{
		m_PushUndo();
	}

	void CreateUndo(int objectIndex, UINT flags)
	{
		m_CreateUndo(objectIndex, flags);
	}

	void HideControls()
	{
		m_HideControls();
	}

	BOOL ShowControls(int objectIndex)
	{
		return m_ShowControls(objectIndex);
	}

	void DrawObject(HDC dc, int objectIndex)
	{
		m_DrawObject(dc, objectIndex);
	}

	void DrawSettingDialog(int objectIndex)
	{
		m_DrawSettingDialog(objectIndex);
	}

	void DeleteFilter(int objectIndex, int filterIndex)
	{
		m_DeleteFilter(objectIndex, filterIndex);
	}

	void SwapFilter(int objectIndex, int filterIndex, int relativeIndex)
	{
		m_SwapFilter(objectIndex, filterIndex, relativeIndex);
	}

	void Unknown1(int objectIndex, int filterIndex)
	{
		m_Unknown1(objectIndex, filterIndex);
	}

	LPCSTR GetAliasFileName(int aliasId)
	{
		return m_GetAliasFileName(aliasId);
	}

	int AddAlias(LPCSTR fileName, BOOL flag1, BOOL flag2, int objectIndex)
	{
		return m_AddAlias(fileName, flag1, flag2, objectIndex);
	}

	BOOL SaveFilterAlias(int objectIndex, int filterIndex, LPCSTR fileName)
	{
		return m_SaveFilterAlias(objectIndex, filterIndex, fileName);
	}

	BOOL LoadExo(LPCSTR fileName, int frame, int layer, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
	{
		return m_LoadExo(fileName, frame, layer, fp, editp);
	}

	BOOL SaveExo(LPCSTR fileName)
	{
		return m_SaveExo(fileName);
	}

	void SetScene(int sceneIndex, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp)
	{
		m_SetScene(sceneIndex, fp, editp);
	}

	void RedrawLayer(int layerIndex)
	{
		m_RedrawLayer(layerIndex);
	}

	void RedrawLayers(int flags[])
	{
		m_RedrawLayers(flags);
	}

	int DeleteMidPoint(int objectIndex, int frame)
	{
		return m_DeleteMidPoint(objectIndex, frame);
	}

public:

#ifdef _INC_SHELLAPI
	void voice(LPCTSTR wavFileName)
	{
		// ファイルが存在するなら
		if (::GetFileAttributes(wavFileName) != INVALID_FILE_ATTRIBUTES)
		{
			// WavPlayer のパスを取得する。
			TCHAR exeFileName[MAX_PATH] = {};
			::GetModuleFileName(0, exeFileName, MAX_PATH);
			::PathRemoveFileSpec(exeFileName);
			::PathAppend(exeFileName, _T("WavPlayer.exe"));
			::PathQuoteSpaces(exeFileName);
			MY_TRACE_TSTR(exeFileName);

			// ファイルが存在するなら
			if (::GetFileAttributes(exeFileName) != INVALID_FILE_ATTRIBUTES)
			{
				// wav ファイルを再生する。

				SHELLEXECUTEINFO sei = { sizeof(sei) };
				sei.lpFile = exeFileName;
				sei.lpParameters = wavFileName;
				BOOL result = ::ShellExecuteEx(&sei);
				MY_TRACE_HEX(result);
			}
		}
	}
#endif
};
