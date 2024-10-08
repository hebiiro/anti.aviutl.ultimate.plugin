import dark
import core
import utils
from core import CLR_NONE
from utils import CLR

# 『[旧形式] discord』のダークモードの設定です。

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width

# メニュー

stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = CLR(24, 25, 28)
	stuff.text.color = CLR(185, 187, 190)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR(0, 0, 0)

stuff = utils.create_stuff("menu_active", "menu_background")

stuff = utils.create_stuff("menu_inactive", "menu_background")

stuff = utils.create_stuff("menu_normal", "menu_background")

stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = CLR(114, 116, 120)
	stuff.text.shadow_color = CLR(0, 0, 0)

stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = CLR(71, 82, 196)
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR(32, 34, 37)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = CLR(47, 49, 54)

stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = CLR(63, 66, 72)

# ダイアログ

stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)

stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = CLR(210, 211, 213)
	stuff.text.shadow_color = CLR(0, 0, 0)

stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = CLR(114, 116, 120)
	stuff.text.shadow_color = CLR(0, 0, 0)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = CLR(0, 0, 0)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = CLR(0, 0, 0)

# エディットボックス

stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = CLR(64, 68, 75)
	stuff.text.color = CLR(220, 221, 222)
	stuff.text.shadow_color = CLR(0, 0, 0)

stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)
	stuff.text.color = CLR(114, 116, 120)
	stuff.text.shadow_color = CLR(0, 0, 0)

stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = CLR(71, 82, 196)
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = CLR(0, 0, 0)

# スクロールバー

stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)
#	stuff.fill.color = CLR(63, 66, 72)

stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = utils.add_color(CLR(64, 68, 75), 20)
	stuff.border.color = utils.add_color(CLR(24, 25, 28), -10)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)
	stuff.border.color = utils.add_color(CLR(24, 25, 28), 10)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = CLR(71, 82, 196)
	stuff.border.color = utils.add_color(CLR(24, 25, 28), -10)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_hot", "scrollbar_hover")

stuff = utils.create_stuff("scrollbar_pressed", "scrollbar_hover")

# トラックバー

## トラックバーの背景
stuff = utils.create_stuff("trackbar_background", "dialog_background")

## トラックバーのトラック
stuff = utils.create_stuff("trackbar_track")
if (stuff):
	stuff.fill.color = CLR(32, 34, 37)
	stuff.border.color = CLR(63, 66, 72)
	stuff.border.width = 1

# トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = CLR(71, 82, 196)

# トラックバーの通常状態
stuff = utils.create_stuff("trackbar_normal", "scrollbar_normal")

# トラックバーの無効状態
stuff = utils.create_stuff("trackbar_disabled", "scrollbar_disabled")

# トラックバーのホット状態
stuff = utils.create_stuff("trackbar_hot", "scrollbar_hot")

# トラックバーのプレス状態
stuff = utils.create_stuff("trackbar_pressed", "scrollbar_pressed")

# ペインキャプション

stuff = utils.create_stuff("pane_caption_active")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)
	stuff.text.color = CLR(210, 211, 213)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR_NONE

stuff = utils.create_stuff("pane_caption_inactive")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)
	stuff.text.color = CLR(210, 211, 213)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR_NONE

stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = CLR(71, 82, 196)

stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = CLR(32, 34, 37)

stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = CLR(54, 57, 63)

# ウィンドウキャプション

stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = CLR(41, 43, 47)
	stuff.text.color = CLR(255, 255, 255)
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = CLR(32, 34, 37)
	stuff.text.color = CLR(185, 187, 190)
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ボタン

stuff = utils.create_stuff("button_normal")
if (stuff):
	stuff.fill.color = CLR(47, 49, 54)
	stuff.text.color = CLR(185, 187, 190)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR(32, 34, 37)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = CLR(47, 49, 54)
	stuff.text.color = CLR(114, 116, 120)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR(32, 34, 37)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = CLR(64, 68, 75)
	stuff.text.color = CLR(220, 221, 222)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR(32, 34, 37)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = CLR(69, 73, 80)
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = CLR(0, 0, 0)
	stuff.border.color = CLR(32, 34, 37)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_checked")
if (stuff):
	stuff.text.color = CLR(59, 165, 93)
	stuff.text.shadow_color = CLR(0, 0, 0)

# エッジ

edges = [
	utils.add_color(CLR(32, 34, 37), -30),
	utils.add_color(CLR(32, 34, 37), 0),
	utils.add_color(CLR(66, 70, 77), 0),
	utils.add_color(CLR(66, 70, 77), +30) ]

# 拡張編集のタイムライン

bound_color = [ CLR(66, 70, 77), CLR(32, 34, 37) ]

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = utils.add_color(CLR(47, 49, 54), 0)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(24, 25, 28)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = utils.add_color(CLR(47, 49, 54), -5)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(24, 25, 28)

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
exedit.group.color = CLR(200, 210, 255)
exedit.group.alpha = 0.15

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR(0, 0, 0), CLR(210, 211, 213))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)
