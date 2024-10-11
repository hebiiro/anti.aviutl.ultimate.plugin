import sys
import struct
import ctypes
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはトラックバーを描画します。
#
class TrackBar(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(TrackBar, self).__init__(core.VSCLASS_TRACKBAR)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_TRACKBAR)
		self.add_theme(core.VSCLASS_EXPLORER_TRACKBAR)

		# 描画関数を追加します。
		self.add_handler(core.TKP_TRACK, core.TRS_NORMAL, self.draw_horz_track, STUFF_TRACK)
		self.add_handler(core.TKP_TRACK, core.TRS_BACKGROUND, self.draw_horz_track, STUFF_BACKGROUND)
		self.add_handler(core.TKP_TRACK, core.TRS_SELECTED, self.draw_horz_track, STUFF_SELECTED)
		self.add_handler(core.TKP_THUMBBOTTOM, core.TUS_NORMAL, self.draw_horz_thumb, STUFF_NORMAL)
		self.add_handler(core.TKP_THUMBBOTTOM, core.TUS_DISABLED, self.draw_horz_thumb, STUFF_DISABLED)
		self.add_handler(core.TKP_THUMBBOTTOM, core.TUS_HOT, self.draw_horz_thumb, STUFF_HOT)
		self.add_handler(core.TKP_THUMBBOTTOM, core.TUS_PRESSED, self.draw_horz_thumb, STUFF_PRESSED)
		self.add_handler(core.TKP_THUMBBOTTOM, core.TUS_FOCUSED, self.draw_horz_thumb, STUFF_HOT)
		self.add_handler(core.TKP_TRACKVERT, core.TRS_NORMAL, self.draw_vert_track, STUFF_TRACK)
		self.add_handler(core.TKP_TRACKVERT, core.TRS_BACKGROUND, self.draw_horz_track, STUFF_BACKGROUND)
		self.add_handler(core.TKP_TRACKVERT, core.TRS_SELECTED, self.draw_horz_track, STUFF_SELECTED)
		self.add_handler(core.TKP_THUMBRIGHT, core.TUS_NORMAL, self.draw_vert_thumb, STUFF_NORMAL)
		self.add_handler(core.TKP_THUMBRIGHT, core.TUS_DISABLED, self.draw_vert_thumb, STUFF_DISABLED)
		self.add_handler(core.TKP_THUMBRIGHT, core.TUS_HOT, self.draw_vert_thumb, STUFF_HOT)
		self.add_handler(core.TKP_THUMBRIGHT, core.TUS_PRESSED, self.draw_vert_thumb, STUFF_PRESSED)
		self.add_handler(core.TKP_THUMBRIGHT, core.TUS_FOCUSED, self.draw_vert_thumb, STUFF_HOT)

	#
	# 水平方向のトラックを描画します。
	#
	def draw_horz_track(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (1):
				dark.exports.painter.draw_rect(args.dc, args.rc, stuff)
				#dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
			else:
				# つまみの左右で別々のトラックを描画します。
				# 今のところ、ドラッグしたとき塗り残しが発生してしまいます。
				TBM_GETTHUMBRECT = win32con.WM_USER + 25

				lparam = struct.pack("4i", 0, 0, 0, 0)
				win32gui.SendMessage(args.hwnd, TBM_GETTHUMBRECT, None, lparam)
				thumb = dark.RECT()
				thumb.left, thumb.top, thumb.right, thumb.bottom = struct.unpack("4i", lparam)
				#print(f"thumb = {thumb.left}, {thumb.top}, {thumb.right}, {thumb.bottom}")
				center = int((thumb.left + thumb.right) / 2)
				near = dark.RECT(args.rc)
				near.right = thumb.right
				#near.right = center
				far = dark.RECT(args.rc)
				far.left = thumb.left
				#far.left = center
				dark.exports.gdiplus.draw_round_left(args.dc, near, self.get_stuff(STUFF_HOT))
				dark.exports.gdiplus.draw_round_right(args.dc, far, stuff)
		return True

	#
	# 水平方向のつまみを描画します。
	#
	def draw_horz_thumb(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		#if (stuff_name): dark.exports.painter.draw_round_rect(args.dc, args.rc, self.get_stuff(stuff_name))
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.right -= 1
			rc.bottom -= 1
			dark.exports.gdiplus.draw_round_all(args.dc, rc, stuff)
		return True

	#
	# 垂直方向のトラックを描画します。
	#
	def draw_vert_track(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.draw_rect(args.dc, args.rc, stuff)
			#dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
		return True

	#
	# 垂直方向のつまみを描画します。
	#
	def draw_vert_thumb(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		#if (stuff_name): dark.exports.painter.draw_round_rect(args.dc, args.rc, self.get_stuff(stuff_name))
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.right -= 1
			rc.bottom -= 1
			dark.exports.gdiplus.draw_round_all(args.dc, rc, stuff)
		return True
