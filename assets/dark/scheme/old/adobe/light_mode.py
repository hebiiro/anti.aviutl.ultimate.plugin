import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

# 『[旧形式] adobe』のライトモードの設定です。

def KEY(color, add):
	return ADD(color, add, add, 0)

key_fill_color = CLR(240, 245, 255)
key_text_color = CLR(45, 140, 235)

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width

# メニュー

stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = CLR(255)
	stuff.text.color = CLR(60)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(160)

stuff = utils.create_stuff("menu_active", "menu_background")

stuff = utils.create_stuff("menu_inactive", "menu_background")

stuff = utils.create_stuff("menu_normal", "menu_background")

stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = CLR(120)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, -20)
	stuff.text.color = KEY(key_text_color, -40)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = CLR(240)

stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = CLR(160)

# ダイアログ

stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = CLR(255)

stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = CLR(60)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = CLR(150)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = KEY(key_text_color, 0)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = KEY(key_text_color, -20)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

# エディットボックス

stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = CLR(255)
	stuff.text.color = CLR(60)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = CLR(200)
	stuff.text.color = CLR(150)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, -20)
	stuff.text.color = KEY(key_text_color, -40)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

# スクロールバー

stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = CLR(255)

stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = CLR(250)
	stuff.border.color = CLR(100)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = CLR(160)
	stuff.border.color = CLR(80)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, -20)
	stuff.border.color = KEY(key_text_color, -40)
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
	stuff.fill.color = CLR(220)
	stuff.border.color = CLR(120)
	stuff.border.width = 1

# トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, -100)

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
	stuff.fill.color = CLR(255)
	stuff.text.color = KEY(key_text_color, -40)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("pane_caption_inactive")
if (stuff):
	stuff.fill.color = CLR(255)
	stuff.text.color = CLR(60)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = key_text_color

stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = CLR(240)

stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = CLR(255)

# ウィンドウキャプション

stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, 0)
	stuff.text.color = KEY(key_text_color, -40)
	stuff.border.color = KEY(key_fill_color, -60)
	stuff.border.width = 1

stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = CLR(255)
	stuff.text.color = CLR(60)
	stuff.border.color = CLR(160)
	stuff.border.width = 1

stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ボタン

stuff = utils.create_stuff("button_normal")
if (stuff):
	stuff.fill.color = CLR(240)
	stuff.text.color = CLR(60)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(140)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = CLR(240)
	stuff.text.color = CLR(120)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = CLR(180)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, -10)
	stuff.text.color = KEY(key_text_color, -40)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = KEY(key_fill_color, -100)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = KEY(key_fill_color, -20)
	stuff.text.color = KEY(key_text_color, -50)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)
	stuff.border.color = KEY(key_fill_color, -100)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = KEY(key_text_color, -20)
	stuff.text.shadow_color = ADD(stuff.fill.color, -20)

# エッジ

edges = [
	CLR(90),
	CLR(140),
	CLR(180),
	CLR(220) ]

# 拡張編集のタイムライン

bound_color = [ CLR(90), CLR(220) ]

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = CLR(255)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(100)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = key_fill_color

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(100)

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
exedit.group.color = KEY(key_text_color, -10)
exedit.group.alpha = 0.1
exedit.xor_pen.style = 0
exedit.xor_pen.width = 2
exedit.xor_pen.color = key_text_color

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR(0), CLR(60))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)
