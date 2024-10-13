import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

#
# ライトトーンスタッフを作成します。
#
def create_light_tone_stuffs(fill_tones, border_tones, text_tones):
	# トーンを登録しておきます。
	utils.set_tone_array("fill", fill_tones)
	utils.set_tone_array("border", border_tones)
	utils.set_tone_array("text", text_tones)

	#
	# トーンから色を算出して返します。
	#
	def primary_fill(ratio): return fill_tones[0].blend(ratio)
	def secondary_fill(ratio): return fill_tones[1].blend(ratio)
	def tertiary_fill(ratio): return fill_tones[2].blend(ratio)

	def primary_border(ratio): return border_tones[0].blend(ratio)
	def secondary_border(ratio): return border_tones[1].blend(ratio)
	def tertiary_border(ratio): return border_tones[2].blend(ratio)

	def primary_text(ratio): return text_tones[0].blend(ratio)
	def secondary_text(ratio): return text_tones[1].blend(ratio)
	def tertiary_text(ratio): return text_tones[2].blend(ratio)

	#
	# 文字の影の色を算出して返します。
	#
	def text_shadow(stuff, ratio = 0.9):
		return utils.lerp_color(stuff.text.color, stuff.fill.color, ratio)

	#
	# 無効状態の文字の色を算出して返します。
	#
	def disabled_text(stuff, ratio = 0.4):
		return utils.lerp_color(primary_text(-0.7), stuff.fill.color, ratio)

	# よく使う変数を定義します。
	fill_mode = dark.Stuff.Fill.Mode.c_top_left

	# ダークモジュールの設定値を取得します。
	ellipse = dark.hive.ellipse
	border_width = dark.hive.border_width

	# ダイアログ

	## ダイアログの背景
	stuff = utils.create_stuff("dialog_background")
	if (stuff):
		stuff.fill.color = primary_fill(0.9)

	## ダイアログの通常状態
	stuff = utils.create_stuff("dialog_normal", "dialog_background")
	if (stuff):
		stuff.text.color = primary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)

	## ダイアログの無効状態
	stuff = utils.create_stuff("dialog_disabled", "dialog_background")
	if (stuff):
		stuff.text.color = disabled_text(stuff)
		stuff.text.shadow_color = text_shadow(stuff)

	## ダイアログのホット状態
	stuff = utils.create_stuff("dialog_hot", "dialog_background")
	if (stuff):
		stuff.text.color = secondary_text(0)
		stuff.text.shadow_color = text_shadow(stuff)

	## ダイアログのプレス状態
	stuff = utils.create_stuff("dialog_pressed", "dialog_background")
	if (stuff):
		stuff.text.color = secondary_text(0)
		stuff.text.shadow_color = text_shadow(stuff)

	# ウィンドウキャプション

	## ウィンドウキャプションのアクティブ状態
	stuff = utils.create_stuff("window_caption_active")
	if (stuff):
		stuff.fill.color = secondary_fill(0.8)
		stuff.text.color = secondary_text(-0.8)
		stuff.border.color = secondary_border(0.2)
		stuff.border.width = 1

	## ウィンドウキャプションの非アクティブ状態
	stuff = utils.create_stuff("window_caption_inactive")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)
		stuff.text.color = disabled_text(stuff)
		stuff.border.color = primary_border(0.2)
		stuff.border.width = 1

	## ウィンドウキャプションの無効状態
	stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

	# ペインキャプション

	## ペインキャプションのアクティブ状態
	stuff = utils.create_stuff("pane_caption_active")
	if (stuff):
		stuff.fill.color = secondary_fill(0.8)
		stuff.fill.sub_color = secondary_fill(0.7)
		stuff.fill.mode = fill_mode
		stuff.text.color = secondary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = secondary_border(0.9)
		stuff.border.width = border_width

	## ペインキャプションの非アクティブ状態
	stuff = utils.create_stuff("pane_caption_inactive")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)
		stuff.fill.sub_color = primary_fill(0.7)
		stuff.fill.mode = fill_mode
		stuff.text.color = primary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = primary_border(0.9)
		stuff.border.width = border_width

	## ペインキャプションの無効状態
	stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

	# ペインセパレータ

	## ペインセパレータのアクティブ状態
	stuff = utils.create_stuff("pane_separator_active")
	if (stuff):
		stuff.fill.color = secondary_fill(0.4)

	## ペインセパレータの非アクティブ状態
	stuff = utils.create_stuff("pane_separator_inactive")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)

	## ペインセパレータの無効状態
	stuff = utils.create_stuff("pane_separator_disabled")
	if (stuff):
		stuff.fill.color = primary_fill(0.9)

	# メニュー

	## メニューの背景
	stuff = utils.create_stuff("menu_background")
	if (stuff):
		stuff.fill.color = primary_fill(0.9)
		stuff.text.color = primary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = primary_border(0.4)

	## メニューのアクティブ状態
	stuff = utils.create_stuff("menu_active", "menu_background")

	## メニューの非アクティブ状態
	stuff = utils.create_stuff("menu_inactive", "menu_background")

	## メニューの通常状態
	stuff = utils.create_stuff("menu_normal", "menu_background")

	## メニューの無効状態
	stuff = utils.create_stuff("menu_disabled", "menu_background")
	if (stuff):
		stuff.text.color = disabled_text(stuff)
		stuff.text.shadow_color = text_shadow(stuff)

	## メニューのホット状態
	stuff = utils.create_stuff("menu_hot")
	if (stuff):
		stuff.fill.color = secondary_fill(0.7)
		stuff.fill.sub_color = secondary_fill(0.5)
		stuff.fill.mode = fill_mode
		stuff.text.color = secondary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = secondary_border(-0.2)
		stuff.border.width = border_width
		stuff.etc.ellipse = ellipse

	## メニューのガター
	stuff = utils.create_stuff("menu_gutter")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)

	## メニューのセパレータ
	stuff = utils.create_stuff("menu_separator")
	if (stuff):
		stuff.fill.color = primary_border(0.2)

	# エディットボックス

	## エディットボックスの通常状態
	stuff = utils.create_stuff("edit_normal")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)
		stuff.text.color = primary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)

	## エディットボックスの無効状態
	stuff = utils.create_stuff("edit_disabled")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)
		stuff.text.color = disabled_text(stuff)
		stuff.text.shadow_color = text_shadow(stuff)

	## エディットボックスの選択状態
	stuff = utils.create_stuff("edit_selected")
	if (stuff):
		stuff.fill.color = secondary_fill(0.6)
		stuff.text.color = secondary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)

	# スクロールバー

	## スクロールバーの背景
	stuff = utils.create_stuff("scrollbar_background")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)

	## スクロールバーの通常状態
	stuff = utils.create_stuff("scrollbar_normal")
	if (stuff):
		stuff.fill.color = primary_fill(0.6)
		stuff.fill.sub_color = primary_fill(0.4)
		stuff.fill.mode = fill_mode
		stuff.border.color = primary_border(-0.2)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	## スクロールバーの無効状態
	stuff = utils.create_stuff("scrollbar_disabled")
	if (stuff):
		stuff.fill.color = primary_fill(0)

	## スクロールバーのホバー状態
	stuff = utils.create_stuff("scrollbar_hover")
	if (stuff):
		stuff.fill.color = primary_fill(0.5)
		stuff.fill.sub_color = primary_fill(0.3)
		stuff.fill.mode = fill_mode
		stuff.border.color = primary_border(-0.3)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	## スクロールバーのホット状態
	stuff = utils.create_stuff("scrollbar_hot")
	if (stuff):
		stuff.fill.color = secondary_fill(0.4)
		stuff.fill.sub_color = secondary_fill(0.2)
		stuff.fill.mode = fill_mode
		stuff.border.color = secondary_border(-0.4)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	## スクロールバーのプレス状態
	stuff = utils.create_stuff("scrollbar_pressed")
	if (stuff):
		stuff.fill.color = secondary_fill(0.4)
		stuff.fill.sub_color = secondary_fill(0.2)
		stuff.fill.mode = fill_mode
		stuff.border.color = secondary_border(-0.4)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	# トラックバー

	## トラックバーの背景
	stuff = utils.create_stuff("trackbar_background", "dialog_background")

	## トラックバーのトラック
	stuff = utils.create_stuff("trackbar_track")
	if (stuff):
		stuff.fill.color = primary_fill(0.6)
		stuff.border.color = primary_border(-0.2)
		stuff.border.width = 1

	## トラックバーの選択部分
	stuff = utils.create_stuff("trackbar_selected")
	if (stuff):
		stuff.fill.color = secondary_fill(0)

	## トラックバーの通常状態
	stuff = utils.create_stuff("trackbar_normal")
	if (stuff):
		stuff.fill.color = primary_fill(0.9)
		stuff.fill.sub_color = primary_fill(0.7)
		stuff.fill.mode = fill_mode
		stuff.border.color = primary_border(-0.6)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	## トラックバーの無効状態
	stuff = utils.create_stuff("trackbar_disabled")
	if (stuff):
		stuff.fill.color = primary_fill(0.5)
		stuff.fill.sub_color = primary_fill(0.3)
		stuff.fill.mode = fill_mode
		stuff.border.color = primary_border(-0.6)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	## トラックバーのホット状態
	stuff = utils.create_stuff("trackbar_hot")
	if (stuff):
		stuff.fill.color = secondary_fill(0.7)
		stuff.fill.sub_color = secondary_fill(0.5)
		stuff.fill.mode = fill_mode
		stuff.border.color = primary_border(-0.6)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	## トラックバーのプレス状態
	stuff = utils.create_stuff("trackbar_pressed")
	if (stuff):
		stuff.fill.color = secondary_fill(0.5)
		stuff.fill.sub_color = secondary_fill(0.3)
		stuff.fill.mode = fill_mode
		stuff.border.color = primary_border(-0.6)
		stuff.border.width = 1
		stuff.etc.ellipse = ellipse

	# ボタン

	## ボタンの通常状態
	stuff = utils.create_stuff("button_normal")
	if (stuff):
		stuff.fill.color = primary_fill(0.7)
		stuff.fill.sub_color = primary_fill(0.6)
		stuff.fill.mode = fill_mode
		stuff.text.color = primary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = primary_border(0.2)
		stuff.border.width = border_width
		stuff.etc.ellipse = ellipse

	## ボタンの無効状態
	stuff = utils.create_stuff("button_disabled")
	if (stuff):
		stuff.fill.color = primary_fill(0.7)
		stuff.fill.sub_color = primary_fill(0.6)
		stuff.fill.mode = fill_mode
		stuff.text.color = disabled_text(stuff)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = primary_border(0.2)
		stuff.border.width = border_width
		stuff.etc.ellipse = ellipse

	## ボタンのホット状態
	stuff = utils.create_stuff("button_hot")
	if (stuff):
		stuff.fill.color = secondary_fill(0.7)
		stuff.fill.sub_color = secondary_fill(0.6)
		stuff.fill.mode = fill_mode
		stuff.text.color = secondary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = secondary_border(-0.2)
		stuff.border.width = border_width
		stuff.etc.ellipse = ellipse

	## ボタンのプレス状態
	stuff = utils.create_stuff("button_pressed")
	if (stuff):
		stuff.fill.color = secondary_fill(0.6)
		stuff.fill.sub_color = secondary_fill(0.5)
		stuff.fill.mode = fill_mode
		stuff.text.color = secondary_text(-0.9)
		stuff.text.shadow_color = text_shadow(stuff)
		stuff.border.color = secondary_border(-0.3)
		stuff.border.width = border_width
		stuff.etc.ellipse = ellipse

	## ボタンのチェック状態
	stuff = utils.create_stuff("button_checked", "button_normal")
	if (stuff):
		stuff.text.color = secondary_text(-0.4)
		stuff.text.shadow_color = text_shadow(stuff)

	# エッジ

	edges = [
		primary_border(0.5),
		primary_border(0.6),
		primary_border(0.6),
		primary_border(0.7) ]

	# 拡張編集のタイムライン

	bound_color = [ edges[3], edges[0] ]

	## 偶数レイヤー背景のアクティブ状態
	stuff = utils.create_stuff("exedit_even_layer_background_active")
	if (stuff):
		stuff.fill.color = primary_fill(0.8)

	## 偶数レイヤー背景の非アクティブ状態
	stuff = utils.create_stuff("exedit_even_layer_background_inactive")
	if (stuff):
		stuff.fill.color = primary_fill(-0.4)

	## 奇数レイヤー背景のアクティブ状態
	stuff = utils.create_stuff("exedit_odd_layer_background_active")
	if (stuff):
		stuff.fill.color = primary_fill(0.7)

	## 奇数レイヤー背景の非アクティブ状態
	stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
	if (stuff):
		stuff.fill.color = primary_fill(-0.5)

	## レイヤーとレイヤーボタンの境界
	stuff = utils.create_stuff("exedit_layer_line_separator")
	if (stuff):
		stuff.fill.color = bound_color[0]

	## レイヤーの左側
	stuff = utils.create_stuff("exedit_layer_line_left")
	if (stuff):
		stuff.fill.color = bound_color[0]

	## レイヤーの上側
	stuff = utils.create_stuff("exedit_layer_line_top")
	if (stuff):
		stuff.fill.color = bound_color[0]

	## レイヤーの右側
	stuff = utils.create_stuff("exedit_layer_line_right")
	if (stuff):
		stuff.fill.color = bound_color[1]

	## レイヤーの下側
	stuff = utils.create_stuff("exedit_layer_line_bottom")
	if (stuff):
		stuff.fill.color = bound_color[1]

	# 拡張編集のその他の定義

	exedit = dark.exports.config_manager.exedit
	exedit.group.color = primary_fill(0)
	exedit.group.alpha = 0.15

	# アイコン

	icon = dark.exports.icon_manager
	icon.change_color("", CLR(0), primary_text(-0.9))
	icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
	icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

	# まだ定義されていない残りのスタッフを
	# 標準的なスタッフとして作成します。
	utils.create_standard_stuffs(edges)
