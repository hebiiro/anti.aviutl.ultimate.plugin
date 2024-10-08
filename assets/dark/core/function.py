import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
from core.const import *

#
# デバッグ用のprint()です。
#
def debug_print(s): pass
#def debug_print(s): return print(s)

#
# RGBからCOLORREFを作成して返します。
#
def RGB(r, g, b):
	return win32api.RGB(int(r), int(g), int(b))

#
# 指定された値を文字列化して返します。
#
def str(x):
	if (not x): return "None"
	return dark.str(x)

#
# スタッフからペンを作成して返します。
#
def create_pen(stuff):
	if (stuff.edge_width > 0 and stuff.edge_color != CLR_NONE):
		return win32gui.CreatePen(win32con.PS_SOLID, stuff.edge_width, stuff.edge_color)
	else:
		return win32gui.GetStockObject(win32con.NULL_PEN)

#
# スタッフからブラシを作成して返します。
#
def create_solid_brush(stuff):
	if (stuff.fill_color != CLR_NONE):
		return win32gui.CreateSolidBrush(stuff.fill_color)
	else:
		return win32gui.GetStockObject(win32con.NULL_BRUSH)

#
# アイコンを描画します。
#
def draw_icon(args, stuff, font_name, icon_text, rc = None, font_weight = 0):
	if (not rc): rc = args.rc
	font_height = rc.bottom - rc.top
	dc = win32ui.CreateDCFromHandle(args.dc)
	font_data = { "name": font_name, "height": font_height, "weight": font_weight }
	font = win32ui.CreateFont(font_data)
	old_font = dc.SelectObject(font)
	dark.exports.painter.draw_text_with_shadow(args.dc, icon_text, 1, rc,
		win32con.DT_CENTER | win32con.DT_VCENTER | win32con.DT_SINGLELINE, False, stuff)
	dc.SelectObject(old_font)
	return True
