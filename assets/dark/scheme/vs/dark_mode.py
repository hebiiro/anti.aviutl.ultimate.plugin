import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

# 『[アプリ] visual studio』のダークモードの設定です。

#
# 指定されたスタッフから文字の影の色を算出して返します。
#
def text_shadow(stuff, add = -20):
	return ADD(stuff.fill.color, add)

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width

# ウィンドウキャプション

## ウィンドウキャプションのアクティブ状態
stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = CLR(61, 61, 61)
	stuff.text.color = CLR(214, 214, 214)
	stuff.border.color = CLR(61, 61, 61)
	stuff.border.width = 1

## ウィンドウキャプションの非アクティブ状態
stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)
	stuff.text.color = CLR(255, 255, 255)
	stuff.border.color = CLR(46, 46, 46)
	stuff.border.width = 1

## ウィンドウキャプションの無効状態
stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ペインキャプション

## ペインキャプションのアクティブ状態
stuff = utils.create_stuff("pane_caption_active")
if (stuff):
	stuff.fill.color = CLR(61, 61, 61)
	stuff.text.color = CLR(214, 214, 214)
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの非アクティブ状態
stuff = utils.create_stuff("pane_caption_inactive")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)
	stuff.text.color = CLR(178, 178, 178)
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの無効状態
stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

## ペインセパレータのアクティブ状態
stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = CLR(189, 189, 189)

## ペインセパレータの非アクティブ状態
stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)

## ペインセパレータの無効状態
stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)

# ダイアログ

## ダイアログの背景
stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = CLR(31, 31, 31)

## ダイアログの通常状態
stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = CLR(214, 214, 214)
	#stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログの無効状態
stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = ADD(CLR(92, 92, 92), +40) # オリジナルだと暗すぎるので補正しています。
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = CLR(250, 250, 250)
	#stuff.text.color = CLR(131, 190, 235)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = CLR(250, 250, 250)
	#stuff.text.color = CLR(131, 190, 235)
	stuff.text.shadow_color = text_shadow(stuff)

# ボタン

## ボタンの通常状態
stuff = utils.create_stuff("button_normal")
if (stuff):
	stuff.fill.color = CLR(56, 56, 56)
	stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR(66, 66, 66)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンの無効状態
stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = CLR(51, 51, 51)
	stuff.text.color = ADD(CLR(92, 92, 92), +40) # オリジナルだと暗すぎるので補正しています。
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR(61, 61, 61)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのホット状態
stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = CLR(77, 77, 77)
	stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR(68, 68, 68)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのプレス状態
stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = CLR(51, 51, 51)
	stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR(105, 105, 105)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのチェック状態
stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)

# エディットボックス

## エディットボックスの通常状態
stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = CLR(56, 56, 56)
	stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの無効状態
stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = CLR(56, 56, 56)
	stuff.text.color = CLR(214, 214, 214)
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの選択状態
stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = CLR(189, 189, 189)
	stuff.text.color = CLR(0, 0, 0)
	#stuff.fill.color = CLR(0, 108, 190)
	#stuff.text.color = CLR(255, 255, 255)
	stuff.text.shadow_color = text_shadow(stuff)

# メニュー

## メニューの背景
stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)
	stuff.text.color = CLR(214, 214, 214)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR(66, 66, 66)

## メニューのアクティブ状態
stuff = utils.create_stuff("menu_active", "menu_background")

## メニューの非アクティブ状態
stuff = utils.create_stuff("menu_inactive", "menu_background")

## メニューの通常状態
stuff = utils.create_stuff("menu_normal", "menu_background")

## メニューの無効状態
stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = ADD(CLR(92, 92, 92), +40) # オリジナルだと暗すぎるので補正しています。
	stuff.text.shadow_color = text_shadow(stuff)

## メニューのホット状態
stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = CLR(61, 61, 61)
	stuff.text.color = CLR(250, 250, 250)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR(112, 112, 112)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## メニューのガター
stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)

## メニューのセパレータ
stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = CLR(61, 61, 61)

# スクロールバー

## スクロールバーの背景
stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)

## スクロールバーの通常状態
stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = CLR(77, 77, 77)
	stuff.etc.ellipse = ellipse

## スクロールバーの無効状態
stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)
	stuff.etc.ellipse = ellipse

## スクロールバーのホバー状態
stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = CLR(128, 128, 128)
	stuff.etc.ellipse = ellipse

## スクロールバーのホット状態
stuff = utils.create_stuff("scrollbar_hot")
if (stuff):
	stuff.fill.color = CLR(104, 104, 104)
	stuff.etc.ellipse = ellipse

## スクロールバーのプレス状態
stuff = utils.create_stuff("scrollbar_pressed")
if (stuff):
	stuff.fill.color = CLR(104, 104, 104)
	stuff.etc.ellipse = ellipse

# トラックバー

## トラックバーの背景
stuff = utils.create_stuff("trackbar_background", "dialog_background")

## トラックバーのトラック
stuff = utils.create_stuff("trackbar_track")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

## トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = CLR(189, 189, 189)

## トラックバーの通常状態
stuff = utils.create_stuff("trackbar_normal", "scrollbar_normal")
if (stuff):
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

## トラックバーの無効状態
stuff = utils.create_stuff("trackbar_disabled", "scrollbar_disabled")
if (stuff):
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

## トラックバーのホット状態
stuff = utils.create_stuff("trackbar_hot", "scrollbar_hot")
if (stuff):
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

## トラックバーのプレス状態
stuff = utils.create_stuff("trackbar_pressed", "scrollbar_pressed")
if (stuff):
	stuff.border.color = CLR(0, 0, 0)
	stuff.border.width = 1

# エッジ

edges = (
	CLR(56, 56, 56),
	CLR(66, 66, 66),
	CLR(66, 66, 66),
	CLR(56, 56, 56))

# 拡張編集のタイムライン

bound_color = ( CLR(66, 66, 66), CLR(31, 31, 31) )

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = CLR(51, 51, 51)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(104, 104, 104)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = CLR(46, 46, 46)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR(91, 91, 91)

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
exedit.group.color = CLR(214, 214, 214)
exedit.group.alpha = 0.15

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR(0, 0, 0), CLR(250, 250, 250))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

# その他のコントロール

## ツールバーの背景
stuff = utils.create_stuff("toolbar_background", "button_normal")

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)
