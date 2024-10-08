import dark
import core
import design.web as base

#
# スキームのAPIを取得しておきます。
#
stuff = dark.scheme.dark_get_stuff

#
# このクラスはウェブスキンです。
#
class WebSkin():
	#
	# コンストラクタです。
	#
	def __init__(self, args):
		# デザインの定義を開始します。
		manager = core.Manager()

		# メニューバーを定義します。
		menu_bar = manager.add_control(base.MenuBar())
		menu_bar.set_stuff(base.STUFF_ACTIVE, stuff("menu_active"))
		menu_bar.set_stuff(base.STUFF_INACTIVE, stuff("menu_inactive"))
		menu_bar.set_stuff(base.STUFF_BACKGROUND, stuff("menu_background"))
		menu_bar.set_stuff(base.STUFF_GUTTER, stuff("menu_gutter"))
		menu_bar.set_stuff(base.STUFF_SEPARATOR, stuff("menu_separator"))
		menu_bar.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		menu_bar.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		menu_bar.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		menu_bar.set_stuff(base.STUFF_PUSHED, stuff("dialog_pressed"))

		# ポップアップメニューを定義します。
		menu_popup = manager.add_control(base.MenuPopup())
		menu_popup.set_stuff(base.STUFF_ACTIVE, stuff("menu_active"))
		menu_popup.set_stuff(base.STUFF_INACTIVE, stuff("menu_inactive"))
		menu_popup.set_stuff(base.STUFF_BACKGROUND, stuff("menu_background"))
		menu_popup.set_stuff(base.STUFF_GUTTER, stuff("menu_gutter"))
		menu_popup.set_stuff(base.STUFF_SEPARATOR, stuff("menu_separator"))
		menu_popup.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		menu_popup.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		menu_popup.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		menu_popup.set_stuff(base.STUFF_PUSHED, stuff("dialog_pressed"))

		# メニューアイコンを定義します。
		menu_icon = manager.add_control(base.MenuIcon())
		menu_icon.set_stuff(base.STUFF_NORMAL, stuff("menu_normal"))
		menu_icon.set_stuff(base.STUFF_DISABLED, stuff("menu_disabled"))

		# スクロールバーを定義します。
		scrollbar = manager.add_control(base.ScrollBar())
		scrollbar.set_stuff(base.STUFF_BACKGROUND, stuff("scrollbar_background"))
		scrollbar.set_stuff(base.STUFF_NORMAL, stuff("scrollbar_normal"))
		scrollbar.set_stuff(base.STUFF_HOVER, stuff("scrollbar_hover"))
		scrollbar.set_stuff(base.STUFF_HOT, stuff("scrollbar_hot"))
		scrollbar.set_stuff(base.STUFF_PRESSED, stuff("scrollbar_pressed"))
		scrollbar.set_stuff(base.STUFF_DISABLED, stuff("scrollbar_disabled"))

		# スタティックコントロールを定義します。
		static = manager.add_control(base.Static())
		static.set_stuff(base.STUFF_NORMAL, stuff("static_normal"))
		static.set_stuff(base.STUFF_DISABLED, stuff("static_disabled"))

		# ボタンのスタッフを補正します。
		stuff("button_checked").text.shadow_color = stuff("dialog_normal").text.shadow_color

		# ボタンを定義します。
		button = manager.add_control(base.Button())
		button.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		button.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		button.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		button.set_stuff(base.STUFF_PRESSED, stuff("dialog_pressed"))
		button.set_stuff(base.STUFF_CHECKED, stuff("button_checked"))

		# エディットボックスを定義します。
		edit = manager.add_control(base.Edit())
		edit.set_stuff(base.STUFF_NORMAL, stuff("edit_normal"))
		edit.set_stuff(base.STUFF_DISABLED, stuff("edit_disabled"))
		edit.set_stuff(base.STUFF_SELECTED, stuff("edit_selected"))
		edit.set_stuff(base.STUFF_READONLY, stuff("edit_readonly"))

		# リストボックスを定義します。
		listbox = manager.add_control(base.ListBox())
		listbox.set_stuff(base.STUFF_NORMAL, stuff("listbox_normal"))
		listbox.set_stuff(base.STUFF_DISABLED, stuff("listbox_disabled"))
		listbox.set_stuff(base.STUFF_SELECTED, stuff("listbox_selected"))
		listbox.set_stuff(base.STUFF_READONLY, stuff("listbox_readonly"))

		# コンボボックスを定義します。
		combobox = manager.add_control(base.ComboBox())
		combobox.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		combobox.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		combobox.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		combobox.set_stuff(base.STUFF_PRESSED, stuff("dialog_pressed"))

		# ツールチップを定義します。
		tooltip = manager.add_control(base.ToolTip())
		tooltip.set_stuff(base.STUFF_NORMAL, stuff("tooltip_normal"))
		tooltip.set_stuff(base.STUFF_DISABLED, stuff("tooltip_disabled"))

		# トラックバーを定義します。
		trackbar = manager.add_control(base.TrackBar())
		trackbar.set_stuff(base.STUFF_BACKGROUND, stuff("trackbar_background"))
		trackbar.set_stuff(base.STUFF_TRACK, stuff("trackbar_track"))
		trackbar.set_stuff(base.STUFF_NORMAL, stuff("trackbar_normal"))
		trackbar.set_stuff(base.STUFF_HOT, stuff("trackbar_hot"))
		trackbar.set_stuff(base.STUFF_PRESSED, stuff("trackbar_pressed"))
		trackbar.set_stuff(base.STUFF_DISABLED, stuff("trackbar_disabled"))
		trackbar.set_stuff(base.STUFF_SELECTED, stuff("trackbar_selected"))

		# スピンボタンを定義します。
		spin = manager.add_control(base.Spin())
		spin.set_stuff(base.STUFF_BACKGROUND, stuff("dialog_background"))
		spin.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		spin.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		spin.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		spin.set_stuff(base.STUFF_PRESSED, stuff("dialog_pressed"))

		# ヘッダーコントロールを定義します。
		header = manager.add_control(base.Header())
		header.set_stuff(base.STUFF_BACKGROUND, stuff("dialog_background"))
		header.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		header.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		header.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		header.set_stuff(base.STUFF_PRESSED, stuff("dialog_pressed"))

		# リストビューを定義します。
		listview = manager.add_control(base.ListView())
		listview.set_stuff(base.STUFF_SEPARATOR, stuff("listview_separator"))
		listview.set_stuff(base.STUFF_BACKGROUND, stuff("listview_background"))
		listview.set_stuff(base.STUFF_NORMAL, stuff("listview_normal"))
		listview.set_stuff(base.STUFF_DISABLED, stuff("listview_disabled"))
		listview.set_stuff(base.STUFF_HOT, stuff("listview_hot"))
		listview.set_stuff(base.STUFF_SELECTED, stuff("listview_selected"))

		# ツリービューを定義します。
		treeview = manager.add_control(base.TreeView())
		treeview.set_stuff(base.STUFF_NORMAL, stuff("treeview_normal"))
		treeview.set_stuff(base.STUFF_DISABLED, stuff("treeview_disabled"))
		treeview.set_stuff(base.STUFF_HOT, stuff("treeview_hot"))
		treeview.set_stuff(base.STUFF_SELECTED, stuff("treeview_selected"))

		# ツールバーを定義します。
		toolbar = manager.add_control(base.ToolBar())
		toolbar.set_stuff(base.STUFF_BACKGROUND, stuff("dialog_normal"))
		toolbar.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		toolbar.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		toolbar.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		toolbar.set_stuff(base.STUFF_PRESSED, stuff("dialog_pressed"))
		toolbar.set_stuff(base.STUFF_CHECKED, stuff("button_checked"))

		# タブコントロールを定義します。
		tab = manager.add_control(base.Tab())
		tab.set_stuff(base.STUFF_BACKGROUND, stuff("dialog_background"))
		tab.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		tab.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))
		tab.set_stuff(base.STUFF_HOT, stuff("dialog_hot"))
		tab.set_stuff(base.STUFF_PRESSED, stuff("dialog_pressed"))
		tab.set_stuff(base.STUFF_SELECTED, stuff("dialog_hot"))

		# コマンドモジュールを定義します。
		command_module = manager.add_control(base.CommandModule())
		command_module.set_stuff(base.STUFF_BACKGROUND, stuff("command_module_background"))
		command_module.set_stuff(base.STUFF_NORMAL, stuff("command_module_normal"))
		command_module.set_stuff(base.STUFF_DISABLED, stuff("command_module_disabled"))
		command_module.set_stuff(base.STUFF_HOT, stuff("command_module_hot"))
		command_module.set_stuff(base.STUFF_PRESSED, stuff("command_module_pressed"))
		command_module.set_stuff(base.STUFF_CHECKED, stuff("command_module_checked"))

		# プレビューペインを定義します。
		preview_pane = manager.add_control(base.PreviewPane())
		preview_pane.set_stuff(base.STUFF_BACKGROUND, stuff("preview_pane_background"))

		# コモンアイテムズダイアログを定義します。
		common_items_dialog = manager.add_control(base.CommonItemsDialog())
		common_items_dialog.set_stuff(base.STUFF_BACKGROUND, stuff("common_items_dialog_background"))

		# ダイアログを定義します。
		dialog = manager.add_control(base.Dialog())
		dialog.set_stuff(base.STUFF_NORMAL, stuff("dialog_normal"))
		dialog.set_stuff(base.STUFF_DISABLED, stuff("dialog_disabled"))

		# ウィンドウフェイスを定義します。
		window_face = manager.add_control(base.WindowFace())
		window_face.set_stuff(base.STUFF_BACKGROUND, stuff("window_face_background"))

		# ペインキャプションを定義します。
		pane_caption = manager.add_control(base.WindowCaption())
		pane_caption.set_stuff(base.STUFF_ACTIVE, stuff("pane_caption_active"))
		pane_caption.set_stuff(base.STUFF_INACTIVE, stuff("pane_caption_inactive"))
		pane_caption.set_stuff(base.STUFF_DISABLED, stuff("pane_caption_disabled"))

		# ペインセパレータを定義します。
		pane_separator = manager.add_control(base.WindowSeparator())
		pane_separator.set_stuff(base.STUFF_ACTIVE, stuff("pane_separator_active"))
		pane_separator.set_stuff(base.STUFF_INACTIVE, stuff("pane_separator_inactive"))
		pane_separator.set_stuff(base.STUFF_DISABLED, stuff("pane_separator_disabled"))

		# 拡張編集を定義します。
		exedit = manager.add_control(base.ExEdit())
		exedit.set_stuff(base.STUFF_EXEDIT_BACKGROUND, stuff("exedit_background"))
		exedit.set_stuff(base.STUFF_EXEDIT_DUMMY, stuff("exedit_dummy"))
		exedit.set_stuff(base.STUFF_EXEDIT_SELECTION_FILL, stuff("exedit_selection_fill"))
		exedit.set_stuff(base.STUFF_EXEDIT_SELECTION_EDGE, stuff("exedit_selection_edge"))
		exedit.set_stuff(base.STUFF_EXEDIT_SELECTION_BACKGROUND, stuff("exedit_selection_background"))
		exedit.set_stuff(base.STUFF_EXEDIT_SCALE_PRIMARY, stuff("exedit_scale_primary"))
		exedit.set_stuff(base.STUFF_EXEDIT_SCALE_SECONDARY, stuff("exedit_scale_secondary"))
		exedit.set_stuff(base.STUFF_EXEDIT_SCENE_BUTTON_EDGE, stuff("exedit_scene_button_edge"))
		exedit.set_stuff(base.STUFF_EXEDIT_SCENE_BUTTON, stuff("exedit_scene_button"))
		exedit.set_stuff(base.STUFF_EXEDIT_EVEN_LAYER_BUTTON_EDGE, stuff("exedit_even_layer_button_edge"))
		exedit.set_stuff(base.STUFF_EXEDIT_EVEN_LAYER_BUTTON_ACTIVE, stuff("dialog_normal"))
		exedit.set_stuff(base.STUFF_EXEDIT_EVEN_LAYER_BUTTON_INACTIVE, stuff("dialog_disabled"))
		exedit.set_stuff(base.STUFF_EXEDIT_EVEN_LAYER_BACKGROUND_ACTIVE, stuff("exedit_even_layer_background_active"))
		exedit.set_stuff(base.STUFF_EXEDIT_EVEN_LAYER_BACKGROUND_INACTIVE, stuff("exedit_even_layer_background_inactive"))
		exedit.set_stuff(base.STUFF_EXEDIT_ODD_LAYER_BUTTON_EDGE, stuff("exedit_odd_layer_button_edge"))
		exedit.set_stuff(base.STUFF_EXEDIT_ODD_LAYER_BUTTON_ACTIVE, stuff("dialog_normal"))
		exedit.set_stuff(base.STUFF_EXEDIT_ODD_LAYER_BUTTON_INACTIVE, stuff("dialog_disabled"))
		exedit.set_stuff(base.STUFF_EXEDIT_ODD_LAYER_BACKGROUND_ACTIVE, stuff("exedit_odd_layer_background_active"))
		exedit.set_stuff(base.STUFF_EXEDIT_ODD_LAYER_BACKGROUND_INACTIVE, stuff("exedit_odd_layer_background_inactive"))
		exedit.set_stuff(base.STUFF_EXEDIT_LAYER_LINE_SEPARATOR, stuff("exedit_layer_line_separator"))
		exedit.set_stuff(base.STUFF_EXEDIT_LAYER_LINE_LEFT, stuff("exedit_layer_line_left"))
		exedit.set_stuff(base.STUFF_EXEDIT_LAYER_LINE_TOP, stuff("exedit_layer_line_top"))
		exedit.set_stuff(base.STUFF_EXEDIT_LAYER_LINE_RIGHT, stuff("exedit_layer_line_right"))
		exedit.set_stuff(base.STUFF_EXEDIT_LAYER_LINE_BOTTOM, stuff("exedit_layer_line_bottom"))

		# アイコンを定義します。
		icon = dark.exports.icon_manager
		icon.change_color("", core.RGB(0, 0, 0), stuff("dialog_hot").text.color)

		# デザインの定義が完了したので、このスキンに結び付けます。
		self.manager = manager

	#
	# 各描画関数を登録します。
	# この関数群はダークモジュールから呼ばれます。
	#
	def register(self):
		global dark_draw_figure, dark_draw_text, dark_text_out
		dark_draw_figure, dark_draw_text, dark_text_out = self.manager.register()
		return True

#
# 初期化関数です。
# ダークモジュールから呼ばれます。
#
def dark_init(args):
	return WebSkin(args).register()
