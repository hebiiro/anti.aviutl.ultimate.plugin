import dark
import core
import utils
from .guin import *

######################################################################
# 『[テーマ] GuiN』のライトモードの設定です。
######################################################################

# よく使う配色を定義します。

## ペンギンの白
normal = Palette(
	utils.Tone(CLR(255, 255, 255)),
	utils.Tone(CLR(255, 255, 255)),
	utils.Tone(CLR(0)),
	utils.Tone(CLR(160)),
	None)

## ペンギンの黄色～オレンジ
select = Palette(
	utils.Tone(CLR(255, 120, 0)),
	utils.Tone(CLR(255, 255, 0)),
	utils.Tone(CLR(0, 0, 0)),
	utils.Tone(CLR(255, 120, 0)),
	None)

## ペンギンの白～オレンジ
button = Palette(
	utils.Tone(CLR(255, 220, 100)),
	utils.Tone(CLR(255, 255, 220)),
	utils.Tone(CLR(0, 0, 0)),
	utils.Tone(CLR(255, 200, 150)),
	None)

## ゴーグルやヘッドホンで使用されている紫色
accent = utils.Tone(CLR(200, 50, 255))

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
	stuff.fill.color = translucent(select.fill.blend(0.4), 0)
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.border.color = translucent(select.border.blend(0), 0)
	stuff.border.width = 1

## ウィンドウキャプションの非アクティブ状態
stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.text.color = translucent(normal.text.blend(0), 0.5)
	stuff.border.color = translucent(normal.border.blend(0), 0)
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
	stuff.fill.color = translucent(select.fill.blend(0), 0.4)

## ペインセパレータの非アクティブ状態
stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.8)

## ペインセパレータの無効状態
stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 1)

# ダイアログ

## ダイアログの背景
stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)

## ダイアログの通常状態
stuff = utils.create_stuff("dialog_normal", "dialog_background")
if (stuff):
	stuff.text.color = translucent(normal.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログの無効状態
stuff = utils.create_stuff("dialog_disabled", "dialog_background")
if (stuff):
	stuff.text.color = translucent(normal.text.blend(0), 0.5)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot", "dialog_background")
if (stuff):
	stuff.text.color = translucent(select.fill.blend(-0.2), 0)
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed", "dialog_background")
if (stuff):
	stuff.text.color = translucent(select.fill.blend(-0.4), 0)
	stuff.text.shadow_color = text_shadow(stuff)

# ボタン

## ボタンの通常状態
stuff = utils.create_stuff("button_normal")
if (stuff):
	ratio = 0
	stuff.fill.color = translucent(button.fill.blend(0), ratio)
	stuff.fill.sub_color = translucent(button.fill_sub.blend(0), ratio)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(button.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(button.border.blend(0), ratio)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンの無効状態
stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.75)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.75)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(select.text.blend(0), 0.5)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(select.border.blend(0), 0.75)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのホット状態
stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.2)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.2)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(select.border.blend(0), 0.2)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのプレス状態
stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.1)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.1)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(select.border.blend(0), 0.1)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## ボタンのチェック状態
stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = translucent(accent.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

# エディットボックス

## エディットボックスの通常状態
stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.text.color = translucent(normal.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの無効状態
stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.text.color = translucent(normal.text.blend(0), 0.5)
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの選択状態
stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.5)
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)

# メニュー

## メニューの背景
stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.text.color = translucent(normal.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(normal.border.blend(0), 0)

## メニューのアクティブ状態
stuff = utils.create_stuff("menu_active", "menu_background")

## メニューの非アクティブ状態
stuff = utils.create_stuff("menu_inactive", "menu_background")

## メニューの通常状態
stuff = utils.create_stuff("menu_normal", "menu_background")

## メニューの無効状態
stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = translucent(normal.text.blend(0), 0.5)
	stuff.text.shadow_color = text_shadow(stuff)

## メニューのホット状態
stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.4)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.4)
	stuff.fill.mode = fill_mode
	stuff.text.color = translucent(select.text.blend(0), 0)
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = translucent(select.border.blend(0), 0)
	stuff.border.width = border_width
	stuff.etc.ellipse = ellipse

## メニューのガター
stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.5)

## メニューのセパレータ
stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.5)

# スクロールバー

## スクロールバーの背景
stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)

## スクロールバーの通常状態
stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.4)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.4)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(select.border.blend(-0.4), 0.4)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーの無効状態
stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.75)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.75)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(select.border.blend(-0.4), 0.75)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのホバー状態
stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.3)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.3)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(select.border.blend(-0.4), 0.3)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのホット状態
stuff = utils.create_stuff("scrollbar_hot")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0.2)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0.2)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(select.border.blend(-0.4), 0.2)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのプレス状態
stuff = utils.create_stuff("scrollbar_pressed")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0)
	stuff.fill.sub_color = translucent(select.fill_sub.blend(0), 0)
	stuff.fill.mode = fill_mode
	stuff.border.color = translucent(select.border.blend(-0.4), 0)
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

# トラックバー

## トラックバーの背景
stuff = utils.create_stuff("trackbar_background", "dialog_background")

## トラックバーのトラック
stuff = utils.create_stuff("trackbar_track")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)
	stuff.border.color = translucent(select.border.blend(-0.75), 0.75)
	stuff.border.width = 1

## トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = translucent(select.fill.blend(0), 0)

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
	normal.border.blend(-0.4),
	normal.border.blend(0),
	normal.border.blend(0.4),
	normal.border.blend(0.8),
	None ]

# 拡張編集のタイムライン

bound_color = [ edges[3], edges[0] ]

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(0), 0)

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(-0.8), 0)

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(-0.1), 0)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = translucent(normal.fill.blend(-0.8), 0)

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
