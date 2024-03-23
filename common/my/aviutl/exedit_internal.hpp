#pragma once

namespace my
{
#pragma pack(push)
#pragma pack(1)

	//
	// このクラスは選択アイテムの映像情報です。
	//
	struct SelectionVisual
	{
		int32_t object_index; // -30
		uint32_t u9; // -2C
		uint32_t u8; // -28
		uint32_t u7; // -24
		uint32_t u6; // -20
		uint32_t u5; // -1C
		int32_t x; // -18 映像内での座標とサイズ
		int32_t y; // -14
		int32_t w; // -10
		int32_t h; // -0C
		uint16_t u4; // -08
		uint16_t u3; // -06
		uint16_t u2; // -04
		uint16_t u1; // -02
		uint32_t flags; // 0x04ビットが選択アイテム
	};

#pragma pack(pop)

	//
	// このクラスは拡張編集にアクセスします。
	//
	struct ExEditInternal
	{
		//
		// 拡張編集のベースアドレスです。
		//
		addr_t exedit = 0;

		//
		// このクラスは拡張編集のアドレスを保有します。
		//
		struct Address
		{
			//
			// このクラスは変数のアドレスを保有します。
			//
			struct Variable
			{
				HWND* aviutl_window = 0;
				HWND* exedit_window = 0;
				HWND* setting_dialog = 0;
				ExEdit::Object** object_table = 0;
				ExEdit::Object** sorted_object_table = 0;
				ExEdit::Filter** filter_table = 0;
				int32_t* current_scene_index = 0;
				int32_t* current_object_index = 0;
				int32_t* current_filter_index = 0;
				int32_t* object_count = 0;
				int32_t* current_scene_object_count = 0;
				uint8_t** exdata_table = 0;
				int32_t* next_object_table = 0;
				int32_t* object_slection_table = 0;
				int32_t* object_selection_count = 0;
				ExEdit::SceneSetting* scene_setting_table = 0;
				AviUtl::EditHandle** editp = 0;
				int32_t* layer_width = 0;
				int32_t* layer_height = 0;
				int32_t* layer_visible_count = 0;
				ExEdit::LayerSetting** layer_setting_table = 0;
				int32_t* aviutl_frame_number = 0;
				int32_t* exedit_frame_number = 0;
				int32_t* exedit_current_frame = 0;
				HMENU* setting_dialog_menus[5] = {};
				int32_t* selection_visual_count = 0;
				SelectionVisual* selection_visual = 0;
				HWND* font_combobox = 0;
				int32_t* bpm = 0;
			} variable;

			//
			// このクラスは関数のアドレスを保有します。
			//
			struct Function
			{
				int32_t (CDECL *show_color_dialog)(uint32_t u1, COLORREF* color, uint32_t u3) = 0;
				int64_t (CDECL *frame_to_x)(int32_t frame) = 0;
				void (CDECL* push_undo)() = 0;
				void (CDECL* create_undo)(int32_t object_index, uint32_t flags) = 0;
				void (CDECL* hide_controls)() = 0;
				BOOL (CDECL* show_controls)(int32_t object_index) = 0;
				void (CDECL* erase_filter)(int32_t object_index, int32_t filter_index) = 0;
				void (CDECL* swap_filter)(int32_t object_index, int32_t filter_index, int32_t relative_index) = 0;
				void (CDECL* unknown1)(int32_t object_index, int32_t filter_index) = 0;
				LPCSTR (CDECL* get_alias_file_name)(int32_t alias_id) = 0;
				int32_t (CDECL* add_alias)(LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index) = 0;
				BOOL (CDECL* save_filter_alias)(int32_t object_index, int32_t filter_index, LPCSTR file_name) = 0;
				BOOL (CDECL* load_exo)(LPCSTR file_name, int32_t frame, int32_t layer, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) = 0;
				BOOL (CDECL* save_exo)(LPCSTR file_name) = 0;
				BOOL (CDECL* set_scene)(int32_t scene_index, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) = 0;
				void (CDECL* redraw_layer)(int32_t layer_index) = 0;
				void (CDECL* redraw_layers)(int32_t flags[]) = 0;
				int32_t (CDECL* erase_midpt)(int32_t object_index, int32_t frame) = 0;
				void (CDECL* draw_item)(HDC dc, int32_t object_index) = 0;
				void (CDECL* redraw_setting_dialog)(int32_t object_index) = 0;
			} function;
		} address;

		//
		// AviUtlが再生中ならTRUEを返します。
		//
		static BOOL is_playing(AviUtl::FilterProcInfo* fpip)
		{
			return !!((uint32_t)fpip->editp->aviutl_window_info.flag & 0x00040000);
		}

		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			exedit = (addr_t)::GetModuleHandleW(L"exedit.auf");
			if (!exedit) return FALSE;

			tools::assign(address.variable.aviutl_window, exedit + 0x00135C6C);
			tools::assign(address.variable.exedit_window, exedit + 0x00177A44);
			tools::assign(address.variable.setting_dialog, exedit + 0x001539C8);
			tools::assign(address.variable.object_table, exedit + 0x001E0FA4);
			tools::assign(address.variable.sorted_object_table, exedit + 0x00168FA8);
			tools::assign(address.variable.filter_table, exedit + 0x00187C98);
			tools::assign(address.variable.current_scene_index, exedit + 0x001A5310);
			tools::assign(address.variable.current_object_index, exedit + 0x00177A10);
			tools::assign(address.variable.current_filter_index, exedit + 0x0014965C);
			tools::assign(address.variable.object_count, exedit + 0x00146250);
			tools::assign(address.variable.current_scene_object_count, exedit + 0x0015918C);
			tools::assign(address.variable.exdata_table, exedit + 0x001E0FA8);
			tools::assign(address.variable.next_object_table, exedit + 0x001592d8);
			tools::assign(address.variable.object_slection_table, exedit + 0x00179230);
			tools::assign(address.variable.object_selection_count, exedit + 0x00167D88);
			tools::assign(address.variable.scene_setting_table, exedit + 0x00177A50);
			tools::assign(address.variable.editp, exedit + 0x001A532C);
			tools::assign(address.variable.layer_width, exedit + 0x001A52FC);
			tools::assign(address.variable.layer_height, exedit + 0x000A3E20);
			tools::assign(address.variable.layer_visible_count, exedit + 0x000A3FBC);
			tools::assign(address.variable.layer_setting_table, exedit + 0x000A4058);
			tools::assign(address.variable.aviutl_frame_number, exedit + 0x0014D3A0);
			tools::assign(address.variable.exedit_frame_number, exedit + 0x001A5318);
			tools::assign(address.variable.exedit_current_frame, exedit + 0x001A5304);
			tools::assign(address.variable.setting_dialog_menus[0], exedit + 0x00158D20); // 映像フィルタオブジェクト
			tools::assign(address.variable.setting_dialog_menus[1], exedit + 0x00158D24); // 音声メディアオブジェクト
			tools::assign(address.variable.setting_dialog_menus[2], exedit + 0x00158D2C); // 映像メディアオブジェクト＆グループ制御
			tools::assign(address.variable.setting_dialog_menus[3], exedit + 0x00167D40); // カメラ制御＆時間制御
			tools::assign(address.variable.setting_dialog_menus[4], exedit + 0x00167D44); // 音声フィルタオブジェクト
			tools::assign(address.variable.selection_visual_count, exedit + 0x0017921C);
			tools::assign(address.variable.selection_visual, exedit + 0x00146270 + 0x04);
			tools::assign(address.variable.font_combobox, exedit + 0x0023630C);
			tools::assign(address.variable.bpm, exedit + 0x00159190);

			tools::assign(address.function.show_color_dialog, exedit + 0x0004D2A0);
			tools::assign(address.function.frame_to_x, exedit + 0x00032BD0);
			tools::assign(address.function.push_undo, exedit + 0x0008D150);
			tools::assign(address.function.create_undo, exedit + 0x0008D290);
			tools::assign(address.function.hide_controls, exedit + 0x00030500);
			tools::assign(address.function.show_controls, exedit + 0x000305E0);
			tools::assign(address.function.erase_filter, exedit + 0x00033D20);
			tools::assign(address.function.swap_filter, exedit + 0x00033B30);
			tools::assign(address.function.unknown1, exedit + 0x00034FF0);
			tools::assign(address.function.get_alias_file_name, exedit + 0x00043FD0);
			tools::assign(address.function.add_alias, exedit + 0x00029DC0);
			tools::assign(address.function.save_filter_alias, exedit + 0x00028CA0);
			tools::assign(address.function.load_exo, exedit + 0x0004DCA0);
			tools::assign(address.function.save_exo, exedit + 0x000284D0);
			tools::assign(address.function.set_scene, exedit + 0x0002BA60);
			tools::assign(address.function.draw_item, exedit + 0x00037060);
			tools::assign(address.function.redraw_setting_dialog, exedit + 0x00039490);
			tools::assign(address.function.redraw_layer, exedit + 0x00039290);
			tools::assign(address.function.redraw_layers, exedit + 0x000392F0);
			tools::assign(address.function.erase_midpt, exedit + 0x00034A30);

			return TRUE;
		}

		//
		// 拡張編集のベースアドレスを返します。
		//
		// 戻り値
		// 拡張編集のベースアドレスです。
		//
		addr_t get_exedit() { return exedit; }

		//
		// AviUtlウィンドウを返します。
		//
		// 戻り値
		// AviUtlウィンドウのウィンドウハンドルです。
		//
		HWND get_aviutl_window() { return *address.variable.aviutl_window; }

		//
		// 拡張編集ウィンドウを返します。
		//
		// 戻り値
		// 拡張編集ウィンドウのウィンドウハンドルです。
		//
		HWND get_exedit_window() { return *address.variable.exedit_window; }

		//
		// 設定ダイアログを返します。
		//
		// 戻り値
		// 設定ダイアログのウィンドウハンドルです。
		//
		HWND get_setting_dialog() { return *address.variable.setting_dialog; }

		int32_t get_current_scene_index() { return *address.variable.current_scene_index; }
		int32_t get_current_object_index() { return *address.variable.current_object_index; }
		int32_t get_current_filter_index() { return *address.variable.current_filter_index; }
		int32_t get_object_count() { return *address.variable.object_count; }
		int32_t get_current_scene_object_count() { return *address.variable.current_scene_object_count; }
		ExEdit::Object* get_object(int32_t object_index) { return *address.variable.object_table + object_index; }
		ExEdit::Object* get_sorted_object(int32_t object_index) { return address.variable.sorted_object_table[object_index]; }
		ExEdit::Filter* get_filter(int32_t filter_id) { return address.variable.filter_table[filter_id]; }

		ExEdit::Filter* get_filter(ExEdit::Object* object, int32_t filter_index)
		{
			if (!object) return 0;
			int32_t id = object->filter_param[filter_index].id;
			if (id < 0) return 0;
			return get_filter(id);
		}

		int32_t get_next_object_index(int32_t object_index) { return address.variable.next_object_table[object_index]; }
		int32_t get_object_selection(int32_t i) { return address.variable.object_slection_table[i]; }
		int32_t get_object_selection_count() { return *address.variable.object_selection_count; }
		ExEdit::SceneSetting* get_scene_setting(int32_t scene_index) { return address.variable.scene_setting_table + scene_index; }
		AviUtl::EditHandle* get_editp() { return *address.variable.editp; }
		int32_t get_layer_width() { return *address.variable.layer_width; }
		int32_t get_layer_height() { return *address.variable.layer_height; }
		int32_t get_layer_visible_count() { return *address.variable.layer_visible_count; }
		ExEdit::LayerSetting* get_layer_setting(int32_t layer_index) { return (*address.variable.layer_setting_table) + layer_index; }
		int32_t get_aviUtl_frame_number() { return *address.variable.aviutl_frame_number; }
		int32_t get_exedit_frame_number() { return *address.variable.exedit_frame_number; }
		int32_t get_exedit_current_frame() { return *address.variable.exedit_current_frame; }
		void set_exedit_current_frame(int32_t frame) { *address.variable.exedit_current_frame = frame; }
		HMENU get_setting_dialog_menu(int32_t index) { return *address.variable.setting_dialog_menus[index]; }
		int32_t get_setting_dialog_menu_count() { return std::size(address.variable.setting_dialog_menus); }
		int32_t get_selection_visual_count() { return *address.variable.selection_visual_count; }
		SelectionVisual* get_selection_visual(int32_t index) { return &address.variable.selection_visual[index - 1]; }

		//
		// フォントコンボボックスを返します。
		//
		// 戻り値
		// フォントコンボボックスのウィンドウハンドルです。
		//
		HWND get_font_combobox() { return *address.variable.font_combobox; }

		//
		// 拡張編集のBPMを返します。
		// ※ユーザーが指定した数値の10,000倍になっています。
		//
		// 戻り値
		// 拡張編集のBPMです。
		//
		int32_t get_bpm() { return *address.variable.bpm; }

		//
		// 拡張編集の「色の選択」ダイアログを表示します。
		//
		// u1
		// 不明です。
		//
		// color [in, out]
		// 取得した色を格納するポインタです。
		//
		// u3
		// 不明です。
		//
		// 戻り値
		// IDOKやIDCANCELだと思われます。
		//
		int32_t show_color_dialog(DWORD u1, COLORREF* color, DWORD u3) { return address.function.show_color_dialog(u1, color, u3); }

		//
		// フレーム番号を拡張編集ウィンドウ内のX座標(ピクセル単位)に変換します。
		//
		// frame
		// フレーム番号です。
		//
		// 戻り値
		// X座標です。
		//
		int64_t frame_to_x(int32_t frame) { return address.function.frame_to_x(frame); }

		//
		// 新しいアンドゥを作成します。
		//
		void push_undo() { address.function.push_undo(); }

		//
		// 指定されたアイテムを現在のアンドゥに追加します。
		//
		// object_index
		// アンドゥの対象となるアイテムです。
		//
		// flags
		// 不明です。
		//
		void create_undo(int32_t object_index, uint32_t flags) { address.function.create_undo(object_index, flags); }

		void hide_controls() { address.function.hide_controls(); }
		BOOL show_controls(int32_t object_index) { return address.function.show_controls(object_index); }
		void draw_item(HDC dc, int32_t object_index) { address.function.draw_item(dc, object_index); }
		void redraw_setting_dialog(int32_t object_index) { address.function.redraw_setting_dialog(object_index); }
		void erase_filter(int32_t object_index, int32_t filter_index) { address.function.erase_filter(object_index, filter_index); }
		void swap_filter(int32_t object_index, int32_t filter_index, int32_t relative_index) { address.function.swap_filter(object_index, filter_index, relative_index); }
		void unknown1(int32_t object_index, int32_t filter_index) { address.function.unknown1(object_index, filter_index); }
		LPCSTR get_alias_file_name(int32_t alias_id) { return address.function.get_alias_file_name(alias_id); }
		int32_t add_alias(LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index) { return address.function.add_alias(file_name, flag1, flag2, object_index); }
		BOOL save_filter_alias(int32_t object_index, int32_t filter_index, LPCSTR file_name) { return address.function.save_filter_alias(object_index, filter_index, file_name); }
		BOOL load_exo(LPCSTR file_name, int32_t frame, int32_t layer, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) { return address.function.load_exo(file_name, frame, layer, fp, editp); }
		BOOL save_exo(LPCSTR file_name) { return address.function.save_exo(file_name); }
		void set_scene(int32_t scene_index, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) { address.function.set_scene(scene_index, fp, editp); }
		void redraw_layer(int32_t layer_index) { address.function.redraw_layer(layer_index); }
		void redraw_layers(int32_t flags[]) { address.function.redraw_layers(flags); }
		int32_t erase_midpt(int32_t object_index, int32_t frame) { return address.function.erase_midpt(object_index, frame); }

		//
		// 指定されたアイテムの指定されたフィルタの拡張データを返します。
		//
		// object [in]
		// 拡張編集のアイテムです。
		//
		// filter_index [in]
		// アイテム内のフィルタのインデックスです。
		//
		// 戻り値
		// 拡張データです。
		//
		uint8_t* get_exdata(ExEdit::Object* object, int32_t filter_index)
		{
			uint8_t* exdata_table = *address.variable.exdata_table;
			DWORD offset = get_exdata_offset(object, filter_index);
			return exdata_table + offset + 0x0004;
		}

		//
		// 指定されたフィルタIDに対応するフィルタのインデックスを返します。
		//
		// object [in]
		// 拡張編集のアイテムです。
		//
		// filter_id [in]
		// アイテム内のフィルタのIDです。
		//
		// 戻り値
		// フィルタのインデックスです。
		//
		static int32_t get_filter_index(ExEdit::Object* object, int32_t filter_id)
		{
			if (!object) return -1;

			int32_t c = object->countFilters();

			for (int32_t i = 0; i < c; i++)
			{
				if (object->filter_param[i].id == filter_id)
					return i;
			}

			return -1;
		}

		//
		// 指定されたアイテムの指定されたフィルタの拡張データへのオフセットを返します。
		//
		// object [in]
		// 拡張編集のアイテムです。
		//
		// filter_index [in]
		// アイテム内のフィルタのインデックスです。
		//
		// 戻り値
		// 拡張データへのオフセットです。
		//
		static DWORD get_exdata_offset(ExEdit::Object* object, int32_t filter_index)
		{
			return object->exdata_offset + object->filter_param[filter_index].exdata_offset;
		}

		//
		// 指定されたフィルタが移動可能な場合はTRUEを返します。
		//
		// object [in]
		// 拡張編集のアイテムです。
		//
		// filter_index [in]
		// アイテム内のフィルタのインデックスです。
		//
		// 戻り値
		// BOOLです。
		//
		static BOOL is_moveable(ExEdit::Object* object, int32_t filter_index)
		{
			int32_t id = object->filter_param[filter_index].id;
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

		//
		// 指定されたアイテムが保有する移動可能なフィルタの数を返します。
		//
		// object [in]
		// 拡張編集のアイテムです。
		//
		// 戻り値
		// 移動可能なフィルタの数です。
		//
		static int32_t get_moveable_filter_count(ExEdit::Object* object)
		{
			for (int32_t i = 0; i < ExEdit::Object::MAX_FILTER; i++)
			{
				if (object->filter_param[i].id < 0)
					return i;

				if (i != 0 && !is_moveable(object, i))
					return i;
			}

			return ExEdit::Object::MAX_FILTER;
		}
	};
}
