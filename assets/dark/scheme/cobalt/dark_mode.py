import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

# 『[テーマ] コバルト』のダークモードの設定です。

#
# 指定されたキーカラーを補正して返します。
#
def KEY(color, add):
	return ADD(color, add, add, add)

#
# 指定されたスタッフから文字の影の色を算出して返します。
#
def text_shadow(stuff, add = -20):
	return ADD(stuff.fill.color, add)

#
# よく使う配色を定義します。
#
normal_fill_color = CLR(20, 30, 40)
normal_text_color = CLR(170)
normal_border_color = CLR(100)
disabled_fill_color = CLR(20, 40, 60)
disabled_text_color = CLR(120)
disabled_border_color = CLR(0, 0, 20)
key_fill_color = CLR(20, 40, 60)
key_text_color = CLR(255, 200, 0)
key_border_color = CLR(0, 10, 20)
accent_color = CLR(240, 0, 0)

# よく使う変数を定義します。
fill_mode = dark.Stuff.Fill.Mode.c_top_left

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width

# ウィンドウキャプション

## ウィンドウキャプションのアクティブ状態
stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = normal_fill_color
	stuff.text.color = normal_text_color
	stuff.border.color = ADD(normal_border_color, 30)
	stuff.border.width = 1

## ウィンドウキャプションの非アクティブ状態
stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = normal_fill_color
	stuff.text.color = disabled_text_color
	stuff.border.color = ADD(normal_border_color, 0)
	stuff.border.width = 1

## ウィンドウキャプションの無効状態
stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ペインキャプション

## ペインキャプションのアクティブ状態
stuff = utils.create_stuff("pane_caption_active")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, 20)
	stuff.fill.sub_color = ADD(normal_fill_color, 10)
	stuff.fill.mode = fill_mode
	stuff.text.color = normal_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの非アクティブ状態
stuff = utils.create_stuff("pane_caption_inactive")
if (stuff):
	stuff.fill.color = KEY(normal_fill_color, 0)
	stuff.text.color = disabled_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの無効状態
stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

## ペインセパレータのアクティブ状態
stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = key_fill_color

## ペインセパレータの非アクティブ状態
stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, -10)

## ペインセパレータの無効状態
stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = normal_fill_color

# ダイアログ

## ダイアログの背景
stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = normal_fill_color

## ダイアログの通常状態
stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = normal_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログの無効状態
stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = disabled_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = KEY(key_text_color, 0)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = KEY(key_text_color, -20)
	stuff.text.shadow_color = text_shadow(stuff)

# ボタン

## ボタンの通常状態
stuff = utils.create_stuff("button_normal")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 0)
	stuff.fill.sub_color = KEY(key_fill_color, -15)
	stuff.fill.mode = fill_mode
	stuff.text.color = KEY(key_text_color, 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = KEY(key_border_color, -15)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンの無効状態
stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 0)
	stuff.fill.sub_color = KEY(key_fill_color, -15)
	stuff.fill.mode = fill_mode
	stuff.text.color = disabled_text_color
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = KEY(key_border_color, -15)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのホット状態
stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 15)
	stuff.fill.sub_color = KEY(key_fill_color, 0)
	stuff.fill.mode = fill_mode
	stuff.text.color = KEY(key_text_color, 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = KEY(key_border_color, 0)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのプレス状態
stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 30)
	stuff.fill.sub_color = KEY(key_fill_color, 15)
	stuff.fill.mode = fill_mode
	stuff.text.color = KEY(key_text_color, 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = KEY(key_border_color, 0)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのチェック状態
stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = accent_color
	stuff.text.shadow_color = text_shadow(stuff)

# エディットボックス

## エディットボックスの通常状態
stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, -5)
	stuff.text.color = normal_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの無効状態
stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, -5)
	stuff.text.color = disabled_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの選択状態
stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 20)
	stuff.text.color = key_text_color
	stuff.text.shadow_color = text_shadow(stuff)

# メニュー

## メニューの背景
stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = normal_fill_color
	stuff.text.color = normal_text_color
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = normal_border_color

## メニューのアクティブ状態
stuff = utils.create_stuff("menu_active", "menu_background")

## メニューの非アクティブ状態
stuff = utils.create_stuff("menu_inactive", "menu_background")

## メニューの通常状態
stuff = utils.create_stuff("menu_normal", "menu_background")

## メニューの無効状態
stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = disabled_text_color
	stuff.text.shadow_color = text_shadow(stuff)

## メニューのホット状態
stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 0)
	stuff.fill.sub_color = KEY(key_fill_color, -15)
	stuff.fill.mode = fill_mode
	stuff.text.color = key_text_color
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = KEY(key_border_color, -15)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## メニューのガター
stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, 10)

## メニューのセパレータ
stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = ADD(normal_border_color, 0)

# スクロールバー

## スクロールバーの背景
stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = KEY(normal_fill_color, 0)

## スクロールバーの通常状態
stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 30)
	stuff.fill.sub_color = KEY(key_fill_color, 15)
	stuff.fill.mode = fill_mode
	stuff.border.color = KEY(key_border_color, 15)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーの無効状態
stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = KEY(disabled_fill_color, 0)
	stuff.border.color = KEY(disabled_border_color, 0)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのホバー状態
stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 40)
	stuff.fill.sub_color = KEY(key_fill_color, 25)
	stuff.fill.mode = fill_mode
	stuff.border.color = KEY(key_border_color, 15)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのホット状態
stuff = utils.create_stuff("scrollbar_hot")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 50)
	stuff.fill.sub_color = KEY(key_fill_color, 35)
	stuff.fill.mode = fill_mode
	stuff.border.color = KEY(key_border_color, 15)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのプレス状態
stuff = utils.create_stuff("scrollbar_pressed")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 60)
	stuff.fill.sub_color = KEY(key_fill_color, 45)
	stuff.fill.mode = fill_mode
	stuff.border.color = KEY(key_border_color, 15)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

# トラックバー

## トラックバーの背景
stuff = utils.create_stuff("trackbar_background", "dialog_background")

## トラックバーのトラック
stuff = utils.create_stuff("trackbar_track")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 0)
	stuff.border.color = KEY(key_border_color, 0)
	stuff.border.width = 1

## トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = KEY(key_text_color, 0)

## トラックバーの通常状態
stuff = utils.create_stuff("trackbar_normal", "scrollbar_normal")

## トラックバーの無効状態
stuff = utils.create_stuff("trackbar_disabled", "scrollbar_disabled")

## トラックバーのホット状態
stuff = utils.create_stuff("trackbar_hot", "scrollbar_hot")

## トラックバーのプレス状態
stuff = utils.create_stuff("trackbar_pressed", "scrollbar_pressed")

# エッジ

edges = [
	ADD(normal_fill_color, -30),
	ADD(normal_fill_color, -10),
	ADD(normal_fill_color, +30),
	ADD(normal_fill_color, +50) ]

# 拡張編集のタイムライン

bound_color = [ edges[3], edges[0] ]

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, 10)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, 80)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, 0)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = ADD(normal_fill_color, 70)

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
exedit.group.color = KEY(key_text_color, 0)
exedit.group.alpha = 0.15

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR(0), KEY(key_text_color, 0))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

# その他のコントロール

## ツールバーの背景
stuff = utils.create_stuff("toolbar_background", "button_normal")

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)
