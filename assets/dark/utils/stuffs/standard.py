import dark
import core
import utils

#
# 既に作成されているスタッフを元に
# 標準的なスタッフを作成します。
#
def create_standard_stuffs(edges):
	# ウィンドウキャプション

	## ウィンドウキャプションのアクティブ状態
	stuff = utils.get_stuff("window_caption_active")
	if (stuff):
		dwm = dark.exports.dwm_manager
		dwm.active_caption_color = stuff.fill.color
		dwm.active_text_color = stuff.text.color
		dwm.active_border_color = stuff.border.color
		#dwm.dark_mode = dark.hive.dark_mode
		dwm.corner_mode = 0

	## ウィンドウキャプションの非アクティブ状態
	stuff = utils.get_stuff("window_caption_inactive")
	if (stuff):
		dwm = dark.exports.dwm_manager
		dwm.inactive_caption_color = stuff.fill.color
		dwm.inactive_text_color = stuff.text.color
		dwm.inactive_border_color = stuff.border.color

	# メニュー

	## メニューの背景
	stuff = utils.get_stuff("menu_background")
	if (stuff):
		dwm = dark.exports.dwm_manager
		#dwm.popup_menu_corner_mode = 3
		dwm.popup_menu_border_color = stuff.border.color

	## メニューのプッシュ状態
	stuff = utils.new_stuff("menu_pushed", "menu_hot")

	# ウィンドウフェイス

	## ウィンドウフェイスの背景
	stuff = utils.new_stuff("window_face_background", "dialog_background")
	if (stuff):
		stuff.border = utils.get_stuff("window_caption_active").border

	# スタティックコントロール

	## スタティックコントロールの通常状態
	stuff = utils.new_stuff("static_normal", "dialog_normal")

	## スタティックコントロールの無効状態
	stuff = utils.new_stuff("static_disabled", "dialog_disabled")

	# エディットボックス

	## エディットボックスのリードオンリー状態
	stuff = utils.new_stuff("edit_readonly", "edit_normal")

	## エディットボックスのセパレータ
	stuff = utils.new_stuff("edit_separator", "menu_separator")

	# スクロールバー

	## スクロールバーの通常状態
	stuff = utils.get_stuff("scrollbar_normal")
	if (stuff):
		# サイズグリップ用の処理です。
		stuff.text.background_color = utils.get_stuff("dialog_background").fill.color

	# リストボックス
	stuff = utils.new_stuff("listbox_normal", "edit_normal")
	stuff = utils.new_stuff("listbox_disabled", "edit_disabled")
	stuff = utils.new_stuff("listbox_selected", "edit_selected")
	stuff = utils.new_stuff("listbox_readonly", "edit_readonly")

	# コンボボックス
	stuff = utils.new_stuff("combobox_normal", "button_normal")
	stuff = utils.new_stuff("combobox_disabled", "button_disabled")
	stuff = utils.new_stuff("combobox_hot", "button_hot")
	stuff = utils.new_stuff("combobox_pressed", "button_pressed")

	# スピンボタン
	stuff = utils.new_stuff("spin_background", "dialog_background")
	stuff = utils.new_stuff("spin_normal", "button_normal")
	stuff = utils.new_stuff("spin_disabled", "button_disabled")
	stuff = utils.new_stuff("spin_hot", "button_hot")
	stuff = utils.new_stuff("spin_pressed", "button_pressed")

	# ヘッダー
	stuff = utils.new_stuff("header_background", "dialog_background")
	stuff = utils.new_stuff("header_normal", "button_normal")
	stuff = utils.new_stuff("header_disabled", "button_disabled")
	stuff = utils.new_stuff("header_hot", "button_hot")
	stuff = utils.new_stuff("header_pressed", "button_hot")

	# リストビュー
	stuff = utils.new_stuff("listview_separator", "edit_separator")
	stuff = utils.new_stuff("listview_background", "edit_normal")
	stuff = utils.new_stuff("listview_normal", "edit_normal")
	stuff = utils.new_stuff("listview_disabled", "edit_disabled")
	stuff = utils.new_stuff("listview_hot", "edit_selected")
	stuff = utils.new_stuff("listview_selected", "edit_selected")

	# ツリービュー
	stuff = utils.new_stuff("treeview_normal", "edit_normal")
	stuff = utils.new_stuff("treeview_disabled", "edit_disabled")
	stuff = utils.new_stuff("treeview_hot", "edit_selected")
	stuff = utils.new_stuff("treeview_selected", "edit_selected")

	# ツールバー
	stuff = utils.new_stuff("toolbar_background", "dialog_background")
	stuff = utils.new_stuff("toolbar_normal", "button_normal")
	stuff = utils.new_stuff("toolbar_disabled", "button_disabled")
	stuff = utils.new_stuff("toolbar_hot", "button_hot")
	stuff = utils.new_stuff("toolbar_pressed", "button_pressed")
	stuff = utils.new_stuff("toolbar_checked", "button_checked")

	# タブコントロール
	stuff = utils.new_stuff("tab_background", "dialog_background")
	stuff = utils.new_stuff("tab_normal", "button_normal")
	stuff = utils.new_stuff("tab_disabled", "button_disabled")
	stuff = utils.new_stuff("tab_hot", "button_hot")
	stuff = utils.new_stuff("tab_pressed", "button_hot")
	stuff = utils.new_stuff("tab_selected", "button_hot")

	# ツールチップ

	## ツールチップの通常状態
	stuff = utils.new_stuff("tooltip_normal", "dialog_normal")
	if (stuff):
		stuff.border = utils.get_stuff("window_caption_active").border

	## ツールチップの無効状態
	stuff = utils.new_stuff("tooltip_disabled", "dialog_disabled")
	if (stuff):
		stuff.border = utils.get_stuff("window_caption_inactive").border

	# コマンドモジュール
	stuff = utils.new_stuff("command_module_background", "dialog_background")
	stuff = utils.new_stuff("command_module_normal", "button_normal")
	stuff = utils.new_stuff("command_module_disabled", "button_disabled")
	stuff = utils.new_stuff("command_module_hot", "button_hot")
	stuff = utils.new_stuff("command_module_pressed", "button_pressed")
	stuff = utils.new_stuff("command_module_checked", "button_checked")

	# プレビューペイン
	stuff = utils.new_stuff("preview_pane_background", "dialog_background")

	# コモンアイテムズダイアログ
	stuff = utils.new_stuff("common_items_dialog_background", "dialog_background")

	# 拡張編集

	## 拡張編集の背景
	stuff = utils.new_stuff("exedit_background", "dialog_background")

	## ダミー背景
	stuff = utils.new_stuff("exedit_dummy")
	if (stuff):
		stuff.fill.color = utils.dummy_color(utils.get_stuff("exedit_background").fill.color)

	## 拡張編集の選択範囲
	stuff = utils.new_stuff("exedit_selection_fill", "trackbar_selected")

	## 拡張編集の選択範囲の縁
	stuff = utils.new_stuff("exedit_selection_edge", "trackbar_selected")

	## 拡張編集の選択範囲の背景(非選択範囲)
	stuff = utils.new_stuff("exedit_selection_background", "dialog_background")

	## 拡張編集の主目盛り
	stuff = utils.new_stuff("exedit_scale_primary", "dialog_normal")
	if (stuff):
		stuff.fill.color = stuff.text.color

	## 拡張編集の副目盛り
	stuff = utils.new_stuff("exedit_scale_secondary", "dialog_normal")
	if (stuff):
		stuff.fill.color = stuff.text.color

	## シーンボタンの縁(描画しない)
	stuff = utils.new_stuff("exedit_scene_button_edge")

	## シーンボタン
	stuff = utils.new_stuff("exedit_scene_button", "dialog_normal")

	## 偶数レイヤーボタンの縁(描画しない)
	stuff = utils.new_stuff("exedit_even_layer_button_edge")

	## 奇数レイヤーボタンの縁(描画しない)
	stuff = utils.new_stuff("exedit_odd_layer_button_edge")

	## 偶数レイヤーボタンのアクティブ状態
	stuff = utils.new_stuff("exedit_even_layer_button_active", "button_normal")

	## 偶数レイヤーボタンの非アクティブ状態
	stuff = utils.new_stuff("exedit_even_layer_button_inactive", "button_disabled")

	## 奇数レイヤーボタンのアクティブ状態
	stuff = utils.new_stuff("exedit_odd_layer_button_active", "button_normal")

	## 奇数レイヤーボタンの非アクティブ状態
	stuff = utils.new_stuff("exedit_odd_layer_button_inactive", "button_disabled")

	## レイヤーとレイヤーボタンの境界
	stuff = utils.new_stuff("exedit_layer_line_separator")
	if (stuff):
		stuff.fill.color = utils.get_stuff("exedit_background").fill.color

	## レイヤーの左側
	stuff = utils.new_stuff("exedit_layer_line_left")
	if (stuff):
		stuff.fill.color = utils.get_stuff("exedit_background").fill.color

	## レイヤーの上側
	stuff = utils.new_stuff("exedit_layer_line_top")
	if (stuff):
		stuff.fill.color = utils.get_stuff("exedit_background").fill.color

	## レイヤーの右側
	stuff = utils.new_stuff("exedit_layer_line_right")
	if (stuff):
		stuff.fill.color = utils.get_stuff("exedit_background").fill.color

	## レイヤーの下側
	stuff = utils.new_stuff("exedit_layer_line_bottom")
	if (stuff):
		stuff.fill.color = utils.get_stuff("exedit_background").fill.color

	# エッジ
	if (edges):
		edge = dark.exports.config_manager.edges.light_raised
		if (edge.near_color == core.CLR_NONE): edge.near_color = edges[3]
		if (edge.far_color == core.CLR_NONE): edge.far_color = edges[0]

		edge = dark.exports.config_manager.edges.light_sunken
		if (edge.near_color == core.CLR_NONE): edge.near_color = edges[0]
		if (edge.far_color == core.CLR_NONE): edge.far_color = edges[3]

		edge = dark.exports.config_manager.edges.raised
		if (edge.inner.near_color == core.CLR_NONE): edge.inner.near_color = edges[3]
		if (edge.inner.far_color == core.CLR_NONE): edge.inner.far_color = edges[0]
		if (edge.outer.near_color == core.CLR_NONE): edge.outer.near_color = edges[2]
		if (edge.outer.far_color == core.CLR_NONE): edge.outer.far_color = edges[1]

		edge = dark.exports.config_manager.edges.sunken
		if (edge.inner.near_color == core.CLR_NONE): edge.inner.near_color = edges[0]
		if (edge.inner.far_color == core.CLR_NONE): edge.inner.far_color = edges[3]
		if (edge.outer.near_color == core.CLR_NONE): edge.outer.near_color = edges[1]
		if (edge.outer.far_color == core.CLR_NONE): edge.outer.far_color = edges[2]

		edge = dark.exports.config_manager.edges.bump
		if (edge.inner.near_color == core.CLR_NONE): edge.inner.near_color = edges[2]
		if (edge.inner.far_color == core.CLR_NONE): edge.inner.far_color = edges[1]
		if (edge.outer.near_color == core.CLR_NONE): edge.outer.near_color = edges[2]
		if (edge.outer.far_color == core.CLR_NONE): edge.outer.far_color = edges[1]

		edge = dark.exports.config_manager.edges.etched
		if (edge.inner.near_color == core.CLR_NONE): edge.inner.near_color = edges[3]
		if (edge.inner.far_color == core.CLR_NONE): edge.inner.far_color = edges[0]
		if (edge.outer.near_color == core.CLR_NONE): edge.outer.near_color = edges[0]
		if (edge.outer.far_color == core.CLR_NONE): edge.outer.far_color = edges[3]
