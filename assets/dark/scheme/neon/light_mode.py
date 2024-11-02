import dark
import core
import utils
from .neon import *

######################################################################
# 『[その他] ネオン』のライトモードの設定です。
######################################################################

# よく使う配色を定義します。

## メインカラー
key_tone = utils.Tone(CLR(255, 120, 160))

## 通常状態
normal = Palette(
	utils.Tone(CLR(255)),
	utils.Tone(CLR(230)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(100, 100, 200)),
	None)

## 選択状態
select = Palette(
	utils.Tone(CLR(255, 200, 255)),
	utils.Tone(CLR(255, 100, 255)),
	utils.Tone(CLR(100, 0, 0)),
	utils.Tone(CLR(255, 0, 0)),
	None)

## ボタンなどで使用する透明度
ratio_normal = 0.8
ratio_disabled = 0.8
ratio_hot = 0.8
ratio_pressed = 0.7

#
# 半透明の色を算出して返します。
#
def translucent(color, ratio, base_color = normal.fill.blend(0)):
	return utils.lerp_color(color, base_color, ratio)

#
# 文字の影の色を算出して返します。
#
def text_shadow(stuff, ratio = 0.9):
	return utils.light_shadow_color(stuff, shadow_density, ratio)

# ウィンドウキャプション

## ウィンドウキャプションのアクティブ状態
stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.4)
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.border.color = translucent(select.fill.blend(0), 0.4)
	stuff.border.width = 1

## ウィンドウキャプションの非アクティブ状態
stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.text.color = translucent(normal.text.blend(0), 0.5)
	stuff.border.color = translucent(normal.fill.blend(0.2), 0)
	stuff.border.width = 1

## ウィンドウキャプションの無効状態
stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ペインキャプション

## ペインキャプションのアクティブ状態
stuff = utils.create_stuff("pane_caption_active")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.4)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.4)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの非アクティブ状態
stuff = utils.create_stuff("pane_caption_inactive")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.fill.sub_color = translucent(normal.fill_sub.blend(0), 0)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(normal.text.blend(0), 0.5)
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの無効状態
stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

## ペインセパレータのアクティブ状態
stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = translucent(key_tone.blend(-0.2), 0)

## ペインセパレータの非アクティブ状態
stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(-0.1), 0)

## ペインセパレータの無効状態
stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)

# ダイアログ

## ダイアログの基本配色
dialog = normal

## ダイアログの強調表示
dialog_accent = Palette(
	utils.Tone(CLR(255)),
	None,
	utils.Tone(CLR(0, 150, 100)),
	None,
	None)

## ダイアログの背景
stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = translucent(dialog.fill.blend(0), 0)

## ダイアログの通常状態
stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = translucent(dialog.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログの無効状態
stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = translucent(dialog.text.blend(0), 0.5)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = translucent(dialog_accent.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = translucent(dialog_accent.text.blend(-0.2), 0)
	stuff.text.shadow_color = text_shadow(stuff)

# ボタン

## ボタンの基本配色
button = Palette(
	utils.Tone(CLR(200, 0, 200)),
	utils.Tone(CLR(0, 200, 255)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(0, 0, 255)),
	None)

## ボタンの強調表示
button_accent = Palette(
	utils.Tone(CLR(255, 100, 0)),
	utils.Tone(CLR(200, 200, 0)),
	utils.Tone(CLR(100, 0, 0)),
	utils.Tone(CLR(100, 0, 0)),
	None)

#
# ボタンの配色をスタッフに適用します。
#
def button_apply(stuff, palette, ratio, text_ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(blend), ratio)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(palette.text.blend(blend), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(palette.border.blend(blend), ratio)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンの通常状態
stuff = utils.create_stuff("button_normal")
if (stuff):
	button_apply(stuff, button, ratio_normal, 0)

## ボタンの無効状態
stuff = utils.create_stuff("button_disabled")
if (stuff):
	button_apply(stuff, button, ratio_disabled, 0.5, -0.5)

## ボタンのホット状態
stuff = utils.create_stuff("button_hot")
if (stuff):
	button_apply(stuff, button_accent, ratio_hot, 0)

## ボタンのプレス状態
stuff = utils.create_stuff("button_pressed")
if (stuff):
	button_apply(stuff, button_accent, ratio_pressed, 0)

## ボタンのチェック状態
stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = translucent(key_tone.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

# コンボボックス

## コンボボックスの基本配色
combobox = Palette(
	utils.Tone(CLR(100, 150, 255)),
	utils.Tone(CLR(50, 200, 150)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(50, 150, 150)),
	None)

## コンボボックスの強調表示
combobox_accent = Palette(
	utils.Tone(CLR(255, 100, 0)),
	utils.Tone(CLR(200, 200, 0)),
	utils.Tone(CLR(100, 0, 0)),
	utils.Tone(CLR(255, 0, 0)),
	None)

#
# コンボボックスの配色をスタッフに適用します。
#
def combobox_apply(stuff, palette, ratio, text_ratio):
	stuff.fill.color = translucent(palette.fill.blend(0), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(0), ratio)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(palette.text.blend(0), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(palette.border.blend(0), ratio)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## コンボボックスの通常状態
stuff = utils.create_stuff("combobox_normal")
if (stuff):
	combobox_apply(stuff, combobox, ratio_normal, 0)

## コンボボックスの無効状態
stuff = utils.create_stuff("combobox_disabled")
if (stuff):
	combobox_apply(stuff, combobox, ratio_disabled, 0.5)

## コンボボックスのホット状態
stuff = utils.create_stuff("combobox_hot")
if (stuff):
	combobox_apply(stuff, combobox_accent, ratio_hot, 0)

## コンボボックスのプレス状態
stuff = utils.create_stuff("combobox_pressed")
if (stuff):
	combobox_apply(stuff, combobox_accent, ratio_pressed, 0)

# スピンボタン

## スピンボタンの基本配色
spin = Palette(
	utils.Tone(CLR(100, 150, 255)),
	utils.Tone(CLR(50, 200, 150)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(0, 100, 200)),
	None)

## スピンボタンの強調表示
spin_accent = Palette(
	utils.Tone(CLR(255, 100, 0)),
	utils.Tone(CLR(200, 200, 0)),
	utils.Tone(CLR(100, 0, 0)),
	utils.Tone(CLR(255, 0, 0)),
	None)

#
# スピンボタンの配色をスタッフに適用します。
#
def spin_apply(stuff, palette, ratio, text_ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(blend), ratio)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(palette.text.blend(blend), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(palette.border.blend(blend), ratio)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## スピンボタンの通常状態
stuff = utils.create_stuff("spin_normal")
if (stuff):
	spin_apply(stuff, spin, ratio_normal, 0)

## スピンボタンの無効状態
stuff = utils.create_stuff("spin_disabled")
if (stuff):
	spin_apply(stuff, spin, ratio_disabled, 0.5)

## スピンボタンのホット状態
stuff = utils.create_stuff("spin_hot")
if (stuff):
	spin_apply(stuff, spin_accent, ratio_hot, 0)

## スピンボタンのプレス状態
stuff = utils.create_stuff("spin_pressed")
if (stuff):
	spin_apply(stuff, spin_accent, ratio_pressed, 0)

# タブ

## タブの基本配色
tab = Palette(
	utils.Tone(CLR(0, 150, 100)),
	utils.Tone(CLR(200, 0, 250)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(0, 0, 255)),
	None)

## タブの強調表示
tab_accent = Palette(
	utils.Tone(CLR(255, 100, 0)),
	utils.Tone(CLR(200, 200, 0)),
	utils.Tone(CLR(100, 0, 0)),
	utils.Tone(CLR(255, 0, 0)),
	None)

#
# タブの配色をスタッフに適用します。
#
def tab_apply(stuff, palette, ratio, text_ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(blend), ratio)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(palette.text.blend(blend), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(palette.border.blend(blend), ratio)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## タブの通常状態
stuff = utils.create_stuff("tab_normal")
if (stuff):
	tab_apply(stuff, tab, ratio_normal, 0)

## タブの無効状態
stuff = utils.create_stuff("tab_disabled")
if (stuff):
	tab_apply(stuff, tab, ratio_disabled, 0.5, -0.5)

## タブのホット状態
stuff = utils.create_stuff("tab_hot")
if (stuff):
	tab_apply(stuff, tab_accent, ratio_hot, 0)

## タブのプレス状態
stuff = utils.create_stuff("tab_pressed")
if (stuff):
	tab_apply(stuff, tab_accent, ratio_pressed, 0)

## タブの選択状態
stuff = utils.create_stuff("tab_selected", "tab_pressed")

# エディットボックス

## エディットボックスの基本配色
edit = Palette(
	utils.Tone(CLR(255)),
	None,
	utils.Tone(CLR(0)),
	None,
	None)

## エディットボックスの強調表示
edit_accent = Palette(
	utils.Tone(CLR(230, 255, 230)),
	None,
	utils.Tone(CLR(0, 100, 0)),
	None,
	None)

#
# エディットボックスの配色をスタッフに適用します。
#
def edit_apply(stuff, palette, ratio, text_ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.text.color = translucent(palette.text.blend(blend), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの通常状態
stuff = utils.create_stuff("edit_normal")
if (stuff):
	edit_apply(stuff, edit, 0, 0)

## エディットボックスの無効状態
stuff = utils.create_stuff("edit_disabled")
if (stuff):
	edit_apply(stuff, edit, 0, 0.5, -0.5)

## エディットボックスの選択状態
stuff = utils.create_stuff("edit_selected")
if (stuff):
	edit_apply(stuff, edit_accent, 0, 0)

# リストボックス

## リストボックスの基本配色
listbox = Palette(
	utils.Tone(CLR(255)),
	None,
	utils.Tone(CLR(60, 100, 200)),
	None,
	None)

## リストボックスの強調表示
listbox_accent = Palette(
	key_tone,
	None,
	utils.Tone(CLR(255)),
	None,
	None)

#
# リストボックスの配色をスタッフに適用します。
#
def listbox_apply(stuff, palette, ratio, text_ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.text.color = translucent(palette.text.blend(blend), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)

## リストボックスの通常状態
stuff = utils.create_stuff("listbox_normal")
if (stuff):
	listbox_apply(stuff, listbox, 0, 0)

## リストボックスの無効状態
stuff = utils.create_stuff("listbox_disabled")
if (stuff):
	listbox_apply(stuff, listbox, 0, 0.5, -0.5)

## リストボックスの選択状態
stuff = utils.create_stuff("listbox_selected")
if (stuff):
	listbox_apply(stuff, listbox_accent, 0.3, 0)

# メニュー

## メニューの基本配色
menu = Palette(
	normal.fill,
	normal.fill_sub,
	utils.Tone(CLR(0)),
	utils.Tone(CLR(50, 200, 100)),
	None)

## メニューの強調表示
menu_accent = Palette(
	utils.Tone(CLR(100, 150, 255)),
	utils.Tone(CLR(50, 200, 150)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(50, 150, 150)),
	None)

#
# メニューの配色をスタッフに適用します。
#
def menu_apply(stuff, palette, ratio, text_ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(blend), ratio)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(palette.text.blend(blend), text_ratio)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(palette.border.blend(blend), ratio)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## メニューの背景
stuff = utils.create_stuff("menu_background")
if (stuff):
	menu_apply(stuff, menu, 0, 0)

## メニューのアクティブ状態
stuff = utils.create_stuff("menu_active", "menu_background")

## メニューの非アクティブ状態
stuff = utils.create_stuff("menu_inactive", "menu_background")

## メニューの通常状態
stuff = utils.create_stuff("menu_normal", "menu_background")

## メニューの無効状態
stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	menu_apply(stuff, menu, 0.5, 0.5, -0.5)

## メニューのホット状態
stuff = utils.create_stuff("menu_hot")
if (stuff):
	menu_apply(stuff, menu_accent, 0.8, 0)

## メニューのガター
stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = translucent(key_tone.blend(0), 0.6)

## メニューのセパレータ
stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = translucent(key_tone.blend(0), 0.4)

# スクロールバー

## スクロールバーの基本配色
scrollbar = Palette(
	utils.Tone(CLR(50, 200, 150)),
	utils.Tone(CLR(100, 150, 255)),
	None,
	utils.Tone(CLR(0, 100, 0)),
	None)

## スクロールバーの強調表示
scrollbar_accent = Palette(
	utils.Tone(CLR(255, 100, 0)),
	utils.Tone(CLR(200, 200, 0)),
	None,
	utils.Tone(CLR(100, 0, 0)),
	None)

#
# スクロールバーの配色をスタッフに適用します。
#
def scrollbar_apply(stuff, palette, ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(blend), ratio)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(palette.border.blend(blend), ratio)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーの背景
stuff = utils.create_stuff("scrollbar_background", "dialog_background")

## スクロールバーの通常状態
stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	scrollbar_apply(stuff, scrollbar, 0.4)

## スクロールバーの無効状態
stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	scrollbar_apply(stuff, scrollbar, 0.5)

## スクロールバーのホバー状態
stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	scrollbar_apply(stuff, scrollbar, 0.3)

## スクロールバーのホット状態
stuff = utils.create_stuff("scrollbar_hot")
if (stuff):
	scrollbar_apply(stuff, scrollbar_accent, 0.2)

## スクロールバーのプレス状態
stuff = utils.create_stuff("scrollbar_pressed")
if (stuff):
	scrollbar_apply(stuff, scrollbar_accent, 0)

# トラックバー

## トラックバーの基本配色
trackbar = Palette(
	utils.Tone(CLR(200, 100, 200)),
	utils.Tone(CLR(255, 100, 255)),
	None,
	utils.Tone(CLR(50, 0, 50)),
	None)

## トラックバーの強調表示
trackbar_accent = Palette(
	utils.Tone(CLR(255, 100, 0)),
	utils.Tone(CLR(200, 200, 0)),
	None,
	utils.Tone(CLR(50, 0, 0)),
	None)

#
# トラックバーの配色をスタッフに適用します。
#
def trackbar_apply(stuff, palette, ratio, blend = 0):
	stuff.fill.color = translucent(palette.fill.blend(blend), ratio)
	stuff.fill.sub_color = translucent(palette.fill_sub.blend(blend), ratio)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(palette.border.blend(blend), ratio)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## トラックバーの背景
stuff = utils.create_stuff("trackbar_background", "dialog_background")

## トラックバーのトラック
stuff = utils.create_stuff("trackbar_track")
if (stuff):
	stuff.fill.color = translucent(CLR(100, 200, 255), 0.8)
	stuff.border.color = translucent(CLR(100, 200, 255), 0.6)
	stuff.border.width = 2

## トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = translucent(CLR(200, 0, 100), 0)

## トラックバーの通常状態
stuff = utils.create_stuff("trackbar_normal")
if (stuff):
	trackbar_apply(stuff, trackbar, 0.5)

## トラックバーの無効状態
stuff = utils.create_stuff("trackbar_disabled")
if (stuff):
	trackbar_apply(stuff, trackbar, 0.8)

## トラックバーのホット状態
stuff = utils.create_stuff("trackbar_hot")
if (stuff):
	trackbar_apply(stuff, trackbar_accent, 0.5)

## トラックバーのプレス状態
stuff = utils.create_stuff("trackbar_pressed")
if (stuff):
	trackbar_apply(stuff, trackbar_accent, 0.4)

# エッジ

edges = [
	key_tone.blend(0.5),
	key_tone.blend(0.6),
	key_tone.blend(0.7),
	key_tone.blend(0.8),
	None ]

# 拡張編集のタイムライン

bound_color = [ edges[3], edges[0] ]

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = translucent(CLR(240, 240, 255), 0)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = translucent(CLR(150), 0)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = translucent(CLR(255, 240, 240), 0)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = translucent(CLR(150), 0)

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
exedit.group.color = translucent(normal.fill.blend(-0.5), 0)
exedit.group.alpha = 0.15

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR(0), translucent(button.text.blend(0), 0))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しま?せん

# その他のコントロール

## ツールバーの背景
stuff = utils.create_stuff("toolbar_background", "button_normal")

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)
