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
				HWND* aviutl_window = nullptr;
				HWND* exedit_window = nullptr;
				HWND* setting_dialog = nullptr;
				ExEdit::Object** object_table = nullptr;
				ExEdit::Object** sorted_object_table = nullptr;
				ExEdit::Filter** filter_table = nullptr;
				int32_t* current_scene_index = nullptr;
				int32_t* current_object_index = nullptr;
				int32_t* current_filter_index = nullptr;
				int32_t* object_count = nullptr;
				int32_t* sorted_object_count = nullptr;
				uint8_t** exdata_table = nullptr;
				int32_t* next_object_table = nullptr;
				int32_t* object_slection_table = nullptr;
				int32_t* object_selection_count = nullptr;
				ExEdit::LayerSetting* all_layer_setting_table = nullptr;
				ExEdit::LayerSetting** current_layer_setting_table = nullptr;
				ExEdit::SceneSetting* scene_setting_table = nullptr;
				AviUtl::EditHandle** editp = nullptr;
				int32_t* layer_width = nullptr;
				int32_t* layer_height = nullptr;
				int32_t* layer_visible_count = nullptr;
				int32_t* aviutl_frame_number = nullptr;
				int32_t* exedit_frame_number = nullptr;
				int32_t* exedit_current_frame = nullptr;
				HMENU* setting_dialog_menus[5] = {};
				int32_t* selection_visual_count = nullptr;
				SelectionVisual* selection_visual = nullptr;
				HWND* font_combobox = nullptr;
				HWND* text_editbox = nullptr;
				HWND* script_editbox = nullptr;
				int32_t* bpm = nullptr;
				LPCSTR animation_effect_table = {};
				LPCSTR custom_object_table = {};
				LPCSTR camera_effect_table = {};
			} variable;

			//
			// このクラスは関数のアドレスを保有します。
			//
			struct Function
			{
				int32_t (CDECL *show_color_dialog)(uint32_t u1, COLORREF* color, uint32_t u3) = nullptr;
				int64_t (CDECL *frame_to_x)(int32_t frame) = nullptr;
				void (CDECL* push_undo)() = nullptr;
				void (CDECL* create_undo)(int32_t object_index, uint32_t flags) = nullptr;
				void (CDECL* hide_controls)() = nullptr;
				BOOL (CDECL* show_controls)(int32_t object_index) = nullptr;
				void (CDECL* erase_filter)(int32_t object_index, int32_t filter_index) = nullptr;
				void (CDECL* swap_filter)(int32_t object_index, int32_t filter_index, int32_t relative_index) = nullptr;
				void (CDECL* unknown1)(int32_t object_index, int32_t filter_index) = nullptr;
				LPCSTR (CDECL* get_alias_file_name)(int32_t alias_id) = nullptr;
				int32_t (CDECL* add_alias)(LPCSTR file_name, int32_t frame, int32_t layer, int32_t object_index) = nullptr;
				BOOL (CDECL* save_filter_alias)(int32_t object_index, int32_t filter_index, LPCSTR file_name) = nullptr;
				BOOL (CDECL* load_exo)(LPCSTR file_name, int32_t frame, int32_t layer, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) = nullptr;
				BOOL (CDECL* save_exo)(LPCSTR file_name) = nullptr;
				BOOL (CDECL* set_current_object)(int32_t object_index) = nullptr;
				BOOL (CDECL* set_scene)(int32_t scene_index, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) = nullptr;
				void (CDECL* draw_item)(HDC dc, int32_t object_index) = nullptr;
				void (CDECL* redraw_setting_dialog)(int32_t object_index) = nullptr;
				void (CDECL* redraw_layer)(int32_t layer_index) = nullptr;
				void (CDECL* redraw_layers)(int32_t flags[]) = nullptr;
				void (CDECL* redraw_timeline)() = nullptr;
				int32_t (CDECL* erase_midpt)(int32_t object_index, int32_t frame) = nullptr;
				int32_t (CDECL* update_object_table)() = nullptr;
			} function;
		} address;

		//
		// AviUtlが再生中ならTRUEを返します。
		//
		inline static BOOL is_playing(AviUtl::FilterProcInfo* fpip)
		{
			return !!((uint32_t)fpip->editp->aviutl_window_info.flag & 0x00040000);
		}

		//
		// 指定されたアドレスを指定された変数に代入します。
		//
		template <typename T>
		inline static void assign_addr(T& p, addr_t address)
		{
			p = (T)address;
		}

		//
		// 初期化を実行します。
		//
		BOOL init()
		{
			exedit = (addr_t)::GetModuleHandleW(L"exedit.auf");
			if (!exedit) return FALSE;

			assign_addr(address.variable.aviutl_window, exedit + 0x00135C6C);
			assign_addr(address.variable.exedit_window, exedit + 0x00177A44);
			assign_addr(address.variable.setting_dialog, exedit + 0x001539C8);
			assign_addr(address.variable.object_table, exedit + 0x001E0FA4);
			assign_addr(address.variable.sorted_object_table, exedit + 0x00168FA8);
			assign_addr(address.variable.filter_table, exedit + 0x00187C98);
			assign_addr(address.variable.current_scene_index, exedit + 0x001A5310);
			assign_addr(address.variable.current_object_index, exedit + 0x00177A10);
			assign_addr(address.variable.current_filter_index, exedit + 0x0014965C);
			assign_addr(address.variable.object_count, exedit + 0x00146250);
			assign_addr(address.variable.sorted_object_count, exedit + 0x0015918C);
			assign_addr(address.variable.exdata_table, exedit + 0x001E0FA8);
			assign_addr(address.variable.next_object_table, exedit + 0x001592d8);
			assign_addr(address.variable.object_slection_table, exedit + 0x00179230);
			assign_addr(address.variable.object_selection_count, exedit + 0x00167D88);
			assign_addr(address.variable.all_layer_setting_table, exedit + 0x00188498);
			assign_addr(address.variable.current_layer_setting_table, exedit + 0x000A4058);
			assign_addr(address.variable.scene_setting_table, exedit + 0x00177A50);
			assign_addr(address.variable.editp, exedit + 0x001A532C);
			assign_addr(address.variable.layer_width, exedit + 0x001A52FC);
			assign_addr(address.variable.layer_height, exedit + 0x000A3E20);
			assign_addr(address.variable.layer_visible_count, exedit + 0x000A3FBC);
			assign_addr(address.variable.aviutl_frame_number, exedit + 0x0014D3A0);
			assign_addr(address.variable.exedit_frame_number, exedit + 0x001A5318);
			assign_addr(address.variable.exedit_current_frame, exedit + 0x001A5304);
			assign_addr(address.variable.setting_dialog_menus[0], exedit + 0x00158D20); // 映像フィルタオブジェクト
			assign_addr(address.variable.setting_dialog_menus[1], exedit + 0x00158D24); // 音声メディアオブジェクト
			assign_addr(address.variable.setting_dialog_menus[2], exedit + 0x00158D2C); // 映像メディアオブジェクト＆グループ制御
			assign_addr(address.variable.setting_dialog_menus[3], exedit + 0x00167D40); // カメラ制御＆時間制御
			assign_addr(address.variable.setting_dialog_menus[4], exedit + 0x00167D44); // 音声フィルタオブジェクト
			assign_addr(address.variable.selection_visual_count, exedit + 0x0017921C);
			assign_addr(address.variable.selection_visual, exedit + 0x00146270 + 0x04);
			assign_addr(address.variable.font_combobox, exedit + 0x0023630C);
			assign_addr(address.variable.text_editbox, exedit + 0x00236328);
			assign_addr(address.variable.script_editbox, exedit + 0x00230C78);
			assign_addr(address.variable.bpm, exedit + 0x00159190);
			assign_addr(address.variable.animation_effect_table, exedit + 0xC1F08);
			assign_addr(address.variable.custom_object_table, exedit + 0xCE090);
//			assign_addr(address.variable.camera_effect_table, exedit + 0x);

			assign_addr(address.function.show_color_dialog, exedit + 0x0004D2A0);
			assign_addr(address.function.frame_to_x, exedit + 0x00032BD0);
			assign_addr(address.function.push_undo, exedit + 0x0008D150);
			assign_addr(address.function.create_undo, exedit + 0x0008D290);
			assign_addr(address.function.hide_controls, exedit + 0x00030500);
			assign_addr(address.function.show_controls, exedit + 0x000305E0);
			assign_addr(address.function.erase_filter, exedit + 0x00033D20);
			assign_addr(address.function.swap_filter, exedit + 0x00033B30);
			assign_addr(address.function.unknown1, exedit + 0x00034FF0);
			assign_addr(address.function.get_alias_file_name, exedit + 0x00043FD0);
			assign_addr(address.function.add_alias, exedit + 0x00029DC0);
			assign_addr(address.function.save_filter_alias, exedit + 0x00028CA0);
			assign_addr(address.function.load_exo, exedit + 0x0004DCA0);
			assign_addr(address.function.save_exo, exedit + 0x000284D0);
			assign_addr(address.function.set_current_object, exedit + 0x000305E0);
			assign_addr(address.function.set_scene, exedit + 0x0002BA60);
			assign_addr(address.function.draw_item, exedit + 0x00037060);
			assign_addr(address.function.redraw_setting_dialog, exedit + 0x00039490);
			assign_addr(address.function.redraw_layer, exedit + 0x00039290);
			assign_addr(address.function.redraw_layers, exedit + 0x000392F0);
			assign_addr(address.function.redraw_timeline, exedit + 0x00039230);
			assign_addr(address.function.erase_midpt, exedit + 0x00034A30);
			assign_addr(address.function.update_object_table, exedit + 0x0002B0F0);

			return TRUE;
		}

		//
		// 拡張編集のベースアドレスを返します。
		//
		addr_t get_exedit() { return exedit; }

		//
		// aviutlウィンドウを返します。
		//
		HWND get_aviutl_window() { return *address.variable.aviutl_window; }

		//
		// 拡張編集ウィンドウを返します。
		//
		HWND get_exedit_window() { return *address.variable.exedit_window; }

		//
		// 設定ダイアログを返します。
		//
		HWND get_setting_dialog() { return *address.variable.setting_dialog; }

		//
		// カレントシーンのインデックスを返します。
		//
		int32_t get_current_scene_index() { return *address.variable.current_scene_index; }

		//
		// カレントアイテムのインデックスを返します。
		//
		int32_t get_current_object_index() { return *address.variable.current_object_index; }

		//
		// カレントフィルタのインデックスを返します。
		//
		int32_t get_current_filter_index() { return *address.variable.current_filter_index; }

		//
		// 全アイテムの総数を返します。
		//
		int32_t get_object_count() { return *address.variable.object_count; }

		//
		// カレントシーンのアイテムの総数を返します。
		//
		int32_t get_sorted_object_count() { return *address.variable.sorted_object_count; }

		//
		// アイテムを返します。
		//
		ExEdit::Object* get_object(int32_t object_index) { return *address.variable.object_table + object_index; }

		//
		// カレントシーンのアイテムを返します。
		//
		ExEdit::Object* get_sorted_object(int32_t object_index) { return address.variable.sorted_object_table[object_index]; }

		//
		// フィルタを返します。
		//
		ExEdit::Filter* get_filter(int32_t filter_id) { return address.variable.filter_table[filter_id]; }

		//
		// 指定されたアイテムの指定されたインデックスにあるフィルタを返します。
		//
		ExEdit::Filter* get_filter(ExEdit::Object* object, int32_t filter_index)
		{
			if (!object) return 0;
			int32_t id = object->filter_param[filter_index].id;
			if (id < 0) return 0;
			return get_filter(id);
		}

		//
		// 次の中間点アイテムのインデックスを返します。
		//
		int32_t get_next_object_index(int32_t object_index) { return address.variable.next_object_table[object_index]; }

		//
		// 選択アイテムのインデックスを返します。
		//
		int32_t get_object_selection(int32_t i) { return address.variable.object_slection_table[i]; }

		//
		// 選択アイテムの数を返します。
		//
		int32_t get_object_selection_count() { return *address.variable.object_selection_count; }

		//
		// 指定されたシーンの指定されたレイヤーの設定を返します。
		//
		ExEdit::LayerSetting* get_layer_setting(int32_t scene_index, int32_t layer_index) { return address.variable.all_layer_setting_table + scene_index * 100 + layer_index; }

		//
		// カレントシーンの指定されたレイヤーの設定を返します。
		//
		ExEdit::LayerSetting* get_layer_setting(int32_t layer_index) { return (*address.variable.current_layer_setting_table) + layer_index; }

		//
		// 指定されたシーンの設定を返します。
		//
		ExEdit::SceneSetting* get_scene_setting(int32_t scene_index) { return address.variable.scene_setting_table + scene_index; }

		//
		// aviutlの編集用ハンドルを返します。
		//
		AviUtl::EditHandle* get_editp() { return *address.variable.editp; }

		//
		// 拡張編集ウィンドウ内でのレイヤーの幅(px)を返します。
		//
		int32_t get_layer_width() { return *address.variable.layer_width; }

		//
		// 拡張編集ウィンドウ内でのレイヤーの高さ(px)を返します。
		//
		int32_t get_layer_height() { return *address.variable.layer_height; }

		//
		// 拡張編集ウィンドウ内でのレイヤーの表示数を返します。
		//
		int32_t get_layer_visible_count() { return *address.variable.layer_visible_count; }

		//
		// aviutlのフレーム総数を返します。
		//
		int32_t get_aviutl_frame_number() { return *address.variable.aviutl_frame_number; }

		//
		// 拡張編集のフレーム総数を返します。
		//
		int32_t get_exedit_frame_number() { return *address.variable.exedit_frame_number; }

		//
		// 拡張編集の現在フレームを返します。
		//
		int32_t get_exedit_current_frame() { return *address.variable.exedit_current_frame; }

		//
		// 拡張編集のフレームを変更します。
		//
		void set_exedit_current_frame(int32_t frame) { *address.variable.exedit_current_frame = frame; }

		//
		// 設定ダイアログのコンテキストメニューを返します。
		//
		HMENU get_setting_dialog_menu(int32_t index) { return *address.variable.setting_dialog_menus[index]; }

		//
		// 設定ダイアログのコンテキストメニューの数を返します。
		//
		int32_t get_setting_dialog_menu_count() { return std::size(address.variable.setting_dialog_menus); }

		//
		// 選択アイテムの視覚情報の数を返します。
		//
		int32_t get_selection_visual_count() { return *address.variable.selection_visual_count; }

		//
		// 選択アイテムの視覚情報を返します。
		//
		SelectionVisual* get_selection_visual(int32_t index) { return &address.variable.selection_visual[index - 1]; }

		//
		// フォント選択コンボボックスを返します。
		//
		HWND get_font_combobox() { return *address.variable.font_combobox; }

		//
		// テキスト編集エディットボックスを返します。
		//
		HWND get_text_editbox() { return *address.variable.text_editbox; }

		//
		// スクリプト編集エディットボックスを返します。
		//
		HWND get_script_editbox() { return *address.variable.script_editbox; }

		//
		// 拡張編集のBPMを返します。
		// ※ユーザーが指定した数値の10,000倍になっています。
		//
		int32_t get_bpm() { return *address.variable.bpm; }

		//
		// アニメーション効果のテーブルを返します。
		//
		LPCSTR get_animation_effect_table() { return address.variable.animation_effect_table; }

		//
		// カスタムオブジェクトのテーブルを返します。
		//
		LPCSTR get_custom_object_table() { return address.variable.custom_object_table; }

		//
		// カメラ効果のテーブルを返します。
		//
		LPCSTR get_camera_effect_table() { return address.variable.camera_effect_table; }

		//
		// 拡張編集の「色の選択」ダイアログを表示します。
		//
		int32_t show_color_dialog(DWORD u1, COLORREF* color, DWORD u3) { return address.function.show_color_dialog(u1, color, u3); }

		//
		// フレーム番号を拡張編集ウィンドウ内のX座標(ピクセル単位)に変換します。
		//
		int64_t frame_to_x(int32_t frame) { return address.function.frame_to_x(frame); }

		//
		// 新しいアンドゥを作成します。
		//
		void push_undo() { address.function.push_undo(); }

		//
		// 指定されたアイテムを現在のアンドゥに追加します。
		//
		void create_undo(int32_t object_index, uint32_t flags) { address.function.create_undo(object_index, flags); }

		//
		// 設定ダイアログのコントロールを非表示にすると思われます。
		//
		void hide_controls() { address.function.hide_controls(); }

		//
		// 設定ダイアログのコントロールを表示にすると思われます。
		//
		BOOL show_controls(int32_t object_index) { return address.function.show_controls(object_index); }

		//
		// 指定されたアイテムの指定されたフィルタを削除します。
		//
		void erase_filter(int32_t object_index, int32_t filter_index) { address.function.erase_filter(object_index, filter_index); }

		//
		// 指定されたアイテムの指定されたフィルタを入れ替えます。
		//
		void swap_filter(int32_t object_index, int32_t filter_index, int32_t relative_index) { address.function.swap_filter(object_index, filter_index, relative_index); }

		//
		// 不明な関数です。
		//
		void unknown1(int32_t object_index, int32_t filter_index) { address.function.unknown1(object_index, filter_index); }

		//
		// 指定されたエイリアスのファイル名を返します。
		//
		LPCSTR get_alias_file_name(int32_t alias_id) { return address.function.get_alias_file_name(alias_id); }

		//
		// 指定されたアイテムに指定されたエイリアスを追加します。
		//
		int32_t add_alias(LPCSTR file_name, BOOL flag1, BOOL flag2, int32_t object_index) { return address.function.add_alias(file_name, flag1, flag2, object_index); }

		//
		// 指定されたアイテムの指定されたフレームにある中間点を削除します。
		//
		int32_t erase_midpt(int32_t object_index, int32_t frame) { return address.function.erase_midpt(object_index, frame); }

		//
		// ソート済みアイテムを再構築します。
		//
		int32_t update_object_table() { return address.function.update_object_table(); }

		//
		// 指定されたアイテムの指定されたフィルタをエイリアスとして指定されたファイルに書き込みます。
		//
		BOOL save_filter_alias(int32_t object_index, int32_t filter_index, LPCSTR file_name) { return address.function.save_filter_alias(object_index, filter_index, file_name); }

		//
		// 指定された場所に指定されたexoファイルを読み込みます。
		//
		BOOL load_exo(LPCSTR file_name, int32_t frame, int32_t layer, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) { return address.function.load_exo(file_name, frame, layer, fp, editp); }

		//
		// カレントシーンを指定されたexoファイルに書き込みます。
		//
		BOOL save_exo(LPCSTR file_name) { return address.function.save_exo(file_name); }

		//
		// カレントアイテムを変更します。
		//
		void set_current_object(int32_t object_index) { address.function.set_current_object(object_index); }

		//
		// カレントシーンを変更します。
		//
		void set_scene(int32_t scene_index, AviUtl::FilterPlugin* fp, AviUtl::EditHandle* editp) { address.function.set_scene(scene_index, fp, editp); }

		//
		// アイテムを描画します。
		//
		void draw_item(HDC dc, int32_t object_index) { address.function.draw_item(dc, object_index); }

		//
		// 設定ダイアログを再描画します。
		//
		void redraw_setting_dialog(int32_t object_index) { address.function.redraw_setting_dialog(object_index); }

		//
		// 指定されたレイヤーを再描画します。
		//
		void redraw_layer(int32_t layer_index) { address.function.redraw_layer(layer_index); }

		//
		// 指定された複数のレイヤーを再描画します。
		//
		void redraw_layers(int32_t flags[]) { address.function.redraw_layers(flags); }

		//
		// 拡張編集ウィンドウを再描画します。
		//
		void redraw_timeline() { address.function.redraw_timeline(); }

		//
		// 拡張編集ウィンドウを再描画します。
		//
		BOOL invalidate(LPCRECT rc = nullptr)
		{
			return ::InvalidateRect(get_exedit_window(), rc, FALSE);
		}

		//
		// 指定されたアイテムの指定されたフィルタの拡張データを返します。
		//
		uint8_t* get_exdata(ExEdit::Object* object, int32_t filter_index)
		{
			auto exdata_table = *address.variable.exdata_table;
			auto offset = get_exdata_offset(object, filter_index);
			return exdata_table + offset + 0x0004;
		}

		//
		// 指定されたオブジェクトのインデックスを返します。
		//
		int32_t get_object_index(ExEdit::Object* object)
		{
			if (!object) return -1;

			return (int32_t)(object - *address.variable.object_table);
		}

		//
		// 指定されたフィルタIDに対応するフィルタのインデックスを返します。
		//
		inline static int32_t get_filter_index(ExEdit::Object* object, int32_t filter_id)
		{
			if (!object) return -1;

			auto c = object->countFilters();

			for (decltype(c) i = 0; i < c; i++)
			{
				if (object->filter_param[i].id == filter_id)
					return i;
			}

			return -1;
		}

		//
		// 指定されたアイテムの指定されたフィルタの拡張データへのオフセットを返します。
		//
		inline static DWORD get_exdata_offset(ExEdit::Object* object, int32_t filter_index)
		{
			return object->exdata_offset + object->filter_param[filter_index].exdata_offset;
		}

		//
		// 指定されたフィルタが移動可能な場合はTRUEを返します。
		//
		inline static BOOL is_moveable(ExEdit::Object* object, int32_t filter_index)
		{
			auto id = object->filter_param[filter_index].id;
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
		inline static int32_t get_moveable_filter_count(ExEdit::Object* object)
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

		//
		// 全アイテム(のインデックス)の配列を返します。
		//
		auto get_object_indexes()
		{
			// 全アイテムの総数を取得します。
			auto c = get_object_count();

			// 配列を確保します。
			std::vector<decltype(c)> result(c);

			// 全アイテムを走査します。
			for (decltype(c) i = 0, index = 0; i < c; i++, index++)
			{
				// アイテムを取得します。
				auto object = get_object(index);

				// 無効なアイテムは無視します。
				while (!(object->flag & ExEdit::Object::Flag::Exist))
					object = get_object(++index);

				// インデックスを配列に格納します。
				result[i] = index;
			}

			// 配列を返します。
			return std::move(result);
		}
	};
}
