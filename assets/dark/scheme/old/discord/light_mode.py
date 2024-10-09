import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

# 『[旧形式] discord』のライトモードの設定です。

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width

# メニュー

stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = CLR(255, 255, 255)
	stuff.text.color = CLR(79, 86, 96)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(219, 222, 225)

stuff = utils.create_stuff("menu_active", "menu_background")

stuff = utils.create_stuff("menu_inactive", "menu_background")

stuff = utils.create_stuff("menu_normal", "menu_background")

stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = ADD(CLR(163, 165, 168), -20)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = ADD(CLR(71, 82, 196), 20)
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = CLR(242, 243, 245)

stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = CLR(219, 222, 225)

# ダイアログ

stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = CLR(255, 255, 255)

stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = ADD(CLR(163, 165, 168), -20)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

# エディットボックス

stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = CLR(235, 237, 239)
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = CLR(235, 237, 239)
	stuff.text.color = ADD(CLR(163, 165, 168), -20)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = CLR(88, 101, 242)
	stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

# エディットボックスのセパレータ
# (リストボックス・リストビューで使用されます)
stuff = utils.create_stuff("edit_separator")
if (stuff):
	stuff.fill.color = CLR(142, 143, 145)

# スクロールバー

stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = CLR(255, 255, 255)

stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = ADD(CLR(220, 221, 222), 20)
	stuff.border.color = ADD(CLR(142, 143, 145), 20)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = ADD(CLR(220, 221, 222), -10)
	stuff.border.color = ADD(CLR(142, 143, 145), -10)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = ADD(CLR(71, 82, 196), 80)
	stuff.border.color = ADD(CLR(142, 143, 145), -40)
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
	stuff.fill.color = ADD(CLR(220, 221, 222), 10)
	stuff.border.color = ADD(CLR(142, 143, 145), 10)
	stuff.border.width = 1

# トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = ADD(CLR(71, 82, 196), 50)

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
	stuff.fill.color = ADD(CLR(220, 221, 222), 20)
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("pane_caption_inactive")
if (stuff):
	stuff.fill.color = ADD(CLR(220, 221, 222), 30)
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = ADD(CLR(71, 82, 196), 50)

stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = ADD(CLR(220, 221, 222), 10)

stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = CLR(255, 255, 255)

# ウィンドウキャプション

stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = ADD(CLR(220, 221, 222), 20)
	stuff.text.color = CLR(6, 6, 7)
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = ADD(CLR(220, 221, 222), 30)
	stuff.text.color = CLR(79, 86, 96)
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ボタン

stuff = utils.create_stuff("button_normal")
if (stuff):
	stuff.fill.color = CLR(242, 243, 245)
	stuff.text.color = CLR(79, 86, 96)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(142, 143, 145)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = CLR(242, 243, 245)
	stuff.text.color = CLR(163, 165, 168)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(142, 143, 145)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = CLR(231, 233, 235)
	stuff.text.color = CLR(46, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(142, 143, 145)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = CLR(219, 222, 225)
	stuff.text.color = CLR(6, 6, 7)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(142, 143, 145)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = CLR(246, 51, 56)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

# エッジ

edges = [
	CLR(160),
	CLR(200),
	CLR(200),
	CLR(240) ]

# 拡張編集のタイムライン

bound_color = [ CLR(100), CLR(240) ]

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = ADD(CLR(255, 255, 255), -5)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(130, 130, 130)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = ADD(CLR(255, 255, 255), -10)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(130, 130, 130)

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
exedit.group.color = CLR(100, 110, 150)
exedit.group.alpha = 0.15

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR(0, 0, 0), CLR(79, 86, 96))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)
