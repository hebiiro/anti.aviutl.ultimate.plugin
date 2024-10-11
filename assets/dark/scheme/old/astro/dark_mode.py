import dark
import core
import utils
from core import CLR_NONE
from utils import CLR
from utils import add_color as ADD

# 『[旧形式] astro』のダークモードの設定です。
# はくしんさんの『AstroDark』を元に作成しています。

#
# 文字の影の色を算出して返します。
#
def text_shadow(stuff, ratio = 0.9):
	return utils.lerp_color(stuff.text.color, stuff.fill.color, ratio)

# ダークモジュールの設定値を取得します。
ellipse = dark.hive.ellipse
border_width = dark.hive.border_width

# ダイアログ

## ダイアログの背景
stuff = utils.create_stuff("dialog_background")
if (stuff):
	stuff.fill.color = CLR("#232125")

## ダイアログの通常状態
stuff = utils.create_stuff("dialog_normal")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#EFDAB9")
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログの無効状態
stuff = utils.create_stuff("dialog_disabled")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.text.color = CLR("#555255")
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのホット状態
stuff = utils.create_stuff("dialog_hot")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.text.color = CLR("#81C08B")
	stuff.text.shadow_color = text_shadow(stuff)

## ダイアログのプレス状態
stuff = utils.create_stuff("dialog_pressed")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.text.color = CLR("#8CC081")
	stuff.text.shadow_color = text_shadow(stuff)

# ウィンドウキャプション

## ウィンドウキャプションのアクティブ状態
stuff = utils.create_stuff("window_caption_active")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.text.color = CLR("#EFDAB9")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1

## ウィンドウキャプションの非アクティブ状態
stuff = utils.create_stuff("window_caption_inactive")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#555255")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1

## ウィンドウキャプションの無効状態
stuff = utils.create_stuff("window_caption_disabled", "window_caption_inactive")

# ペインキャプション

## ペインキャプションのアクティブ状態
stuff = utils.create_stuff("pane_caption_active")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#EFDAB9")
	stuff.text.shadow_color = text_shadow(stuff)

## ペインキャプションの非アクティブ状態
stuff = utils.create_stuff("pane_caption_inactive", "pane_caption_active")

## ペインキャプションの無効状態
stuff = utils.create_stuff("pane_caption_disabled", "pane_caption_inactive")

# ペインセパレータ

## ペインセパレータのアクティブ状態
stuff = utils.create_stuff("pane_separator_active")
if (stuff):
	stuff.fill.color = CLR("#3E5849")

## ペインセパレータの非アクティブ状態
stuff = utils.create_stuff("pane_separator_inactive")
if (stuff):
	stuff.fill.color = CLR("#313934")

## ペインセパレータの無効状態
stuff = utils.create_stuff("pane_separator_disabled")
if (stuff):
	stuff.fill.color = CLR("#232125")

# メニュー

## メニューの背景
stuff = utils.create_stuff("menu_background")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#EFDAB9")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.border.color = CLR("#000")

## メニューのアクティブ状態
stuff = utils.create_stuff("menu_active", "menu_background")

## メニューの非アクティブ状態
stuff = utils.create_stuff("menu_inactive", "menu_background")

## メニューの通常状態
stuff = utils.create_stuff("menu_normal", "menu_background")

## メニューの無効状態
stuff = utils.create_stuff("menu_disabled", "menu_background")
if (stuff):
	stuff.text.color = CLR("#555255")
	stuff.text.shadow_color = text_shadow(stuff)

## メニューのホット状態
stuff = utils.create_stuff("menu_hot")
if (stuff):
	stuff.fill.color = CLR("#4E7A55")
	stuff.text.color = CLR("#FFFFFF")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.etc.ellipse = ellipse

## メニューのガター
stuff = utils.create_stuff("menu_gutter")
if (stuff):
	stuff.fill.color = CLR("#2A272B")

## メニューのセパレータ
stuff = utils.create_stuff("menu_separator")
if (stuff):
	stuff.fill.color = CLR("#313934")

# エディットボックス

## エディットボックスの通常状態
stuff = utils.create_stuff("edit_normal")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#EFDAB9")
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの無効状態
stuff = utils.create_stuff("edit_disabled")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#555255")
	stuff.text.shadow_color = text_shadow(stuff)

## エディットボックスの選択状態
stuff = utils.create_stuff("edit_selected")
if (stuff):
	stuff.fill.color = CLR("#4E7A55")
	stuff.text.color = CLR("#FFFFFF")
	stuff.text.shadow_color = text_shadow(stuff)

# スクロールバー

## スクロールバーの背景
stuff = utils.create_stuff("scrollbar_background")
if (stuff):
	stuff.fill.color = CLR("#232125")

## スクロールバーの通常状態
stuff = utils.create_stuff("scrollbar_normal")
if (stuff):
	stuff.fill.color = CLR("#4F4D51")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーの無効状態
stuff = utils.create_stuff("scrollbar_disabled")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのホバー状態
stuff = utils.create_stuff("scrollbar_hover")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのホット状態
stuff = utils.create_stuff("scrollbar_hot")
if (stuff):
	stuff.fill.color = CLR("#7B7A7C")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

## スクロールバーのプレス状態
stuff = utils.create_stuff("scrollbar_pressed")
if (stuff):
	stuff.fill.color = CLR("#81C08B")
	stuff.border.color = CLR("#000")
	stuff.border.width = 1
	stuff.etc.ellipse = ellipse

# トラックバー

## トラックバーの背景
stuff = utils.create_stuff("trackbar_background", "dialog_background")

## トラックバーのトラック
stuff = utils.create_stuff("trackbar_track")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.border.color = CLR("#2A272B")
	stuff.border.width = 1

## トラックバーの選択部分
stuff = utils.create_stuff("trackbar_selected")
if (stuff):
	stuff.fill.color = CLR("#81C08B")

## トラックバーの通常状態
stuff = utils.create_stuff("trackbar_normal", "scrollbar_normal")

## トラックバーの無効状態
stuff = utils.create_stuff("trackbar_disabled", "scrollbar_disabled")

## トラックバーのホット状態
stuff = utils.create_stuff("trackbar_hot", "scrollbar_hot")

## トラックバーのプレス状態
stuff = utils.create_stuff("trackbar_pressed", "scrollbar_pressed")

# ボタン

## ボタンの通常状態
stuff = utils.create_stuff("button_normal")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.text.color = CLR("#EFDAB9")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.etc.ellipse = ellipse

## ボタンの無効状態
stuff = utils.create_stuff("button_disabled")
if (stuff):
	stuff.fill.color = CLR("#313934")
	stuff.text.color = CLR("#555255")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.etc.ellipse = ellipse

## ボタンのホット状態
stuff = utils.create_stuff("button_hot")
if (stuff):
	stuff.fill.color = CLR("#3E5849")
	stuff.text.color = CLR("#FFFFFF")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.etc.ellipse = ellipse

## ボタンのプレス状態
stuff = utils.create_stuff("button_pressed")
if (stuff):
	stuff.fill.color = CLR("#3E5849")
	stuff.text.color = CLR("#8CC081")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.etc.ellipse = ellipse

## ボタンのチェック状態
stuff = utils.create_stuff("button_checked", "button_normal")
if (stuff):
	stuff.text.color = CLR("#6f6")
	stuff.text.shadow_color = text_shadow(stuff)

# エッジ

edges = (
	CLR("#111"),
	CLR("#232125"),
	CLR("#313934"),
	CLR("#313934"))

# 拡張編集のタイムライン

## 偶数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_active")
if (stuff):
	stuff.fill.color = CLR("#232125")

## 偶数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_even_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR("#111")

## 奇数レイヤー背景のアクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_active")
if (stuff):
	stuff.fill.color = ADD(CLR("#232125"), +7)

## 奇数レイヤー背景の非アクティブ状態
stuff = utils.create_stuff("exedit_odd_layer_background_inactive")
if (stuff):
	stuff.fill.color = CLR("#111")

## 拡張編集の背景
stuff = utils.create_stuff("exedit_background")
if (stuff):
	stuff.fill.color = CLR("#232125")

## 拡張編集のシーン選択ボタン
stuff = utils.create_stuff("exedit_scene_button")
if (stuff):
	stuff.fill.color = CLR("#232125")
	stuff.text.color = CLR("#EFDAB9")
	stuff.text.shadow_color = text_shadow(stuff)
	stuff.etc.ellipse = ellipse

## 拡張編集の選択範囲
stuff = utils.new_stuff("exedit_selection_fill")
if (stuff):
	stuff.fill.color = CLR("#555255")

## 拡張編集の選択範囲の縁
stuff = utils.new_stuff("exedit_selection_edge")
if (stuff):
	stuff.fill.color = CLR("#313934")

## 拡張編集の選択範囲の背景(非選択範囲)
stuff = utils.new_stuff("exedit_selection_background")
if (stuff):
	stuff.fill.color = CLR("#232125")

## 拡張編集の主目盛り
stuff = utils.new_stuff("exedit_scale_primary")
if (stuff):
	stuff.fill.color = CLR("#EFDAB9")
	stuff.text.color = CLR("#EFDAB9")

## 拡張編集の副目盛り
stuff = utils.new_stuff("exedit_scale_secondary")
if (stuff):
	stuff.fill.color = CLR("#EFDAB9")
	stuff.text.color = CLR("#EFDAB9")

## レイヤーとレイヤーボタンの境界
stuff = utils.create_stuff("exedit_layer_line_separator")
if (stuff):
	stuff.fill.color = CLR("#313934")

## レイヤーの左側
stuff = utils.create_stuff("exedit_layer_line_left")
if (stuff):
	stuff.fill.color = CLR("#313934")

## レイヤーの上側
stuff = utils.create_stuff("exedit_layer_line_top")
if (stuff):
	stuff.fill.color = CLR("#313934")

## レイヤーの右側
stuff = utils.create_stuff("exedit_layer_line_right")
if (stuff):
	stuff.fill.color = CLR("#000")

## レイヤーの下側
stuff = utils.create_stuff("exedit_layer_line_bottom")
if (stuff):
	stuff.fill.color = CLR("#000")

# 拡張編集のその他の定義

exedit = dark.exports.config_manager.exedit
exedit.group.color = CLR("#ccf")
exedit.group.alpha = 0.15
exedit.xor_pen.style = 0
exedit.xor_pen.width = 2
exedit.xor_pen.color = CLR("#81C08A")

# アイコン

icon = dark.exports.icon_manager
icon.change_color("", CLR("#000"), CLR("#EFDAB9"))
icon.change_color("EXEDIT_ICON_MA", CLR_NONE, CLR_NONE) # マウスのアイコンは変更しません
icon.change_color("EXEDIT_ICON_SP", CLR_NONE, CLR_NONE) # スポイトのアイコンは変更しません

# まだ定義されていない残りのスタッフを
# 標準的なスタッフとして作成します。
utils.create_standard_stuffs(edges)