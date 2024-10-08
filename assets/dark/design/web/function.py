import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# 下線を描画します。
#
def draw_underline(args, stuff, rc = None):
	if (dark.hive.border_width <= 0): return False
	if (not rc):
		rc = dark.RECT(args.rc)
	else:
		rc = dark.RECT(rc)
	rc.top = rc.bottom - dark.hive.border_width
	dark.exports.painter.fill_rect(args.dc, rc, stuff.text.color)
	return True

#
# パターンを描画します。
#
def draw_pattern(args, stuff, rc = None):
	if (not rc):
		rc = dark.RECT(args.rc)
	else:
		rc = dark.RECT(rc)
	size = dark.get_height(rc)
	size = int(size / 2)
	rc.top = rc.bottom - size
	dark.exports.gdiplus.fill_rect(args.dc, rc, stuff.text.color, 40)
	"""
	rc.right = rc.left + size
	rc.bottom = rc.top + size
	dark.exports.gdiplus.fill_rect(args.dc, rc, stuff.text.color, 40)
	"""
	"""
	rc = dark.RECT(args.rc)
	rc.left = rc.right - size
	rc.top = rc.bottom - size
	dark.exports.gdiplus.fill_rect(args.dc, rc, stuff.text.color, 40)
	"""
	return True
