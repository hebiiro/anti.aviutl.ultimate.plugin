import dark
import core
from functools import singledispatch

#
# 指定された値をクランプして返します。
#
def clamp(value, min_value, max_value):
	if value < min_value: return min_value
	if value > max_value: return max_value
	return value

#
# 指定されたカラーのダミーカラーを返します。
#
def dummy_color(color):
	if ((color & 0xff) == 0x00):
		return color + 1
	else:
		return color - 1

#
# 指定されたグレースケールをCOLORREFにして返します。
#
def gray(scale):
	return core.RGB(scale, scale, scale)

#
# 指定されたカラーコードをCOLORREFにして返します。
#
def html_color(s):
	s = s.lstrip("#")
	if (len(s) < 6):
		r = int(s[0:1], 16)
		g = int(s[1:2], 16)
		b = int(s[2:3], 16)
		r |= r << 4
		g |= g << 4
		b |= b << 4
		return core.RGB(r, g, b)
	else:
		r = int(s[0:2], 16)
		g = int(s[2:4], 16)
		b = int(s[4:6], 16)
		return core.RGB(r, g, b)

#
# カラーコードをCOLORREFに変換して返します。
#
@singledispatch
def CLR(color_code:str):
	return html_color(color_code)

#
# RGBまたはグレースケールをCOLORREFに変換して返します。
#
@CLR.register
def CLR_(r:int, g = None, b = None):
	if (b != None):
		return core.RGB(r, g, b)
	else:
		return gray(r)

#
# COLORREFをRGBに変換して返します。
#
def get_rgb(colorref):
	r = (colorref >>  0) & 0xff
	g = (colorref >>  8) & 0xff
	b = (colorref >> 16) & 0xff
	return (r, g, b)

#
# 与えられた値を補間して返します。
#
def lerp(x, y, t):
	return x * (1 - t) + y * t

#
# 与えられたCOLORREFを補間して返します。
#
def lerp_color(x, y, t):
	x = get_rgb(x)
	y = get_rgb(y)
	if (t < 0): x, y, t = y, x, abs(t)
	return core.RGB(
		lerp(x[0], y[0], t),
		lerp(x[1], y[1], t),
		lerp(x[2], y[2], t))

#
# 与えられたCOLORREFに値を加算して返します。
#
def add_color(color, r, g = None, b = None):
	if (g == None): g = r
	if (b == None): b = r
	rgb = get_rgb(color)
	return core.RGB(
		clamp(rgb[0] + r, 0, 255),
		clamp(rgb[1] + g, 0, 255),
		clamp(rgb[2] + b, 0, 255))

#
# このクラスはトーンです。
#
class Tone():
	#
	# コンストラクタです。
	#
	def __init__(self, color, black = core.RGB(0, 0, 0), white = core.RGB(255, 255, 255)):
		self.color = color
		self.black = black
		self.white = white

	#
	# COLORREFを返します。
	# ratio == -1 の場合は黒を返します。
	# ratio ==  0 の場合は原色を返します。
	# ratio == +1 の場合は白を返します。
	#
	def blend(self, ratio):
		if (ratio < 0):
			return lerp_color(self.color, self.black, abs(ratio))
		else:
			return lerp_color(self.color, self.white, abs(ratio))
