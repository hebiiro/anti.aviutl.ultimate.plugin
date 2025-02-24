import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはスクロールバーを描画します。
#
class ScrollBar(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ScrollBar, self).__init__(core.VSCLASS_SCROLLBAR)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_SCROLLBAR)
		self.add_theme(core.VSCLASS_EXPLORER_SCROLLBAR)

		# 描画関数を追加します。
		self.add_handler(core.SBP_ARROWBTN, core.ABS_LEFTNORMAL, self.draw_left, None)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_LEFTHOVER, self.draw_left, STUFF_HOVER)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_LEFTHOT, self.draw_left, STUFF_HOT)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_LEFTPRESSED, self.draw_left, STUFF_PRESSED)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_LEFTDISABLED, self.draw_left, STUFF_DISABLED)

		self.add_handler(core.SBP_ARROWBTN, core.ABS_RIGHTNORMAL, self.draw_right, None)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_RIGHTHOVER, self.draw_right, STUFF_HOVER)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_RIGHTHOT, self.draw_right, STUFF_HOT)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_RIGHTPRESSED, self.draw_right, STUFF_PRESSED)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_RIGHTDISABLED, self.draw_right, STUFF_DISABLED)

		self.add_handler(core.SBP_ARROWBTN, core.ABS_UPNORMAL, self.draw_up, None)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_UPHOVER, self.draw_up, STUFF_HOVER)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_UPHOT, self.draw_up, STUFF_HOT)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_UPPRESSED, self.draw_up, STUFF_PRESSED)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_UPDISABLED, self.draw_up, STUFF_DISABLED)

		self.add_handler(core.SBP_ARROWBTN, core.ABS_DOWNNORMAL, self.draw_down, None)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_DOWNHOVER, self.draw_down, STUFF_HOVER)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_DOWNHOT, self.draw_down, STUFF_HOT)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_DOWNPRESSED, self.draw_down, STUFF_PRESSED)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_DOWNDISABLED, self.draw_down, STUFF_DISABLED)

		self.add_handler(core.SBP_THUMBBTNHORZ, core.SCRBS_NORMAL, self.draw_horz_thumb, STUFF_NORMAL)
		self.add_handler(core.SBP_THUMBBTNHORZ, core.SCRBS_HOVER, self.draw_horz_thumb, STUFF_HOVER)
		self.add_handler(core.SBP_THUMBBTNHORZ, core.SCRBS_HOT, self.draw_horz_thumb, STUFF_HOT)
		self.add_handler(core.SBP_THUMBBTNHORZ, core.SCRBS_PRESSED, self.draw_horz_thumb, STUFF_PRESSED)
		self.add_handler(core.SBP_THUMBBTNHORZ, core.SCRBS_DISABLED, self.draw_horz_thumb, STUFF_DISABLED)

		self.add_handler(core.SBP_THUMBBTNVERT, core.SCRBS_NORMAL, self.draw_vert_thumb, STUFF_NORMAL)
		self.add_handler(core.SBP_THUMBBTNVERT, core.SCRBS_HOVER, self.draw_vert_thumb, STUFF_HOVER)
		self.add_handler(core.SBP_THUMBBTNVERT, core.SCRBS_HOT, self.draw_vert_thumb, STUFF_HOT)
		self.add_handler(core.SBP_THUMBBTNVERT, core.SCRBS_PRESSED, self.draw_vert_thumb, STUFF_PRESSED)
		self.add_handler(core.SBP_THUMBBTNVERT, core.SCRBS_DISABLED, self.draw_vert_thumb, STUFF_DISABLED)

		self.add_handler(core.SBP_LOWERTRACKHORZ, core.SCRBS_NORMAL, self.draw_horz_upper_track, STUFF_NORMAL)
		self.add_handler(core.SBP_LOWERTRACKHORZ, core.SCRBS_HOVER, self.draw_horz_upper_track, STUFF_HOVER)
		self.add_handler(core.SBP_LOWERTRACKHORZ, core.SCRBS_HOT, self.draw_horz_upper_track, STUFF_HOT)
		self.add_handler(core.SBP_LOWERTRACKHORZ, core.SCRBS_PRESSED, self.draw_horz_upper_track, STUFF_PRESSED)
		self.add_handler(core.SBP_LOWERTRACKHORZ, core.SCRBS_DISABLED, self.draw_horz_upper_track, STUFF_DISABLED)

		self.add_handler(core.SBP_UPPERTRACKHORZ, core.SCRBS_NORMAL, self.draw_horz_upper_track, STUFF_NORMAL)
		self.add_handler(core.SBP_UPPERTRACKHORZ, core.SCRBS_HOVER, self.draw_horz_upper_track, STUFF_HOVER)
		self.add_handler(core.SBP_UPPERTRACKHORZ, core.SCRBS_HOT, self.draw_horz_upper_track, STUFF_HOT)
		self.add_handler(core.SBP_UPPERTRACKHORZ, core.SCRBS_PRESSED, self.draw_horz_upper_track, STUFF_PRESSED)
		self.add_handler(core.SBP_UPPERTRACKHORZ, core.SCRBS_DISABLED, self.draw_horz_upper_track, STUFF_DISABLED)

		self.add_handler(core.SBP_LOWERTRACKVERT, core.SCRBS_NORMAL, self.draw_vert_upper_track, STUFF_NORMAL)
		self.add_handler(core.SBP_LOWERTRACKVERT, core.SCRBS_HOVER, self.draw_vert_upper_track, STUFF_HOVER)
		self.add_handler(core.SBP_LOWERTRACKVERT, core.SCRBS_HOT, self.draw_vert_upper_track, STUFF_HOT)
		self.add_handler(core.SBP_LOWERTRACKVERT, core.SCRBS_PRESSED, self.draw_vert_upper_track, STUFF_PRESSED)
		self.add_handler(core.SBP_LOWERTRACKVERT, core.SCRBS_DISABLED, self.draw_vert_upper_track, STUFF_DISABLED)

		self.add_handler(core.SBP_UPPERTRACKVERT, core.SCRBS_NORMAL, self.draw_vert_upper_track, STUFF_NORMAL)
		self.add_handler(core.SBP_UPPERTRACKVERT, core.SCRBS_HOVER, self.draw_vert_upper_track, STUFF_HOVER)
		self.add_handler(core.SBP_UPPERTRACKVERT, core.SCRBS_HOT, self.draw_vert_upper_track, STUFF_HOT)
		self.add_handler(core.SBP_UPPERTRACKVERT, core.SCRBS_PRESSED, self.draw_vert_upper_track, STUFF_PRESSED)
		self.add_handler(core.SBP_UPPERTRACKVERT, core.SCRBS_DISABLED, self.draw_vert_upper_track, STUFF_DISABLED)

		self.add_handler(core.SBP_SIZEBOX, core.SZB_RIGHTALIGN, self.draw_sizebox_right_align, STUFF_NORMAL)

	#
	# スクロールバーの太さの倍率を返します。
	#
	def get_ratio(self):
		return (100 - dark.hive.scrollbar_reduction) / 200

	#
	# スクロールバーの背景を描画します。
	#
	def draw_background(self, args):
		dark.exports.painter.fill_rect(args.dc, args.rc, self.get_stuff(STUFF_BACKGROUND))
		return True

	#
	# 背景と水平方向の矢印を描画します。
	#
	def draw_horz_arrow(self, args, stuff_name, attrs):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			if (stuff.accent.color != core.CLR_NONE):
				dark.exports.gdiplus.draw_round_horz(args.dc, rc, stuff.accent.color)
			ratio = self.get_ratio()
			rc.deflate(int(rc.width * ratio), int(rc.height * ratio))
			#dark.exports.gdiplus.draw_ellipse(args.dc, rc, stuff)
			dark.exports.gdiplus.draw_round_horz(args.dc, rc, stuff)
		return True

	#
	# 背景と垂直方向の矢印を描画します。
	#
	def draw_vert_arrow(self, args, stuff_name, attrs):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			if (stuff.accent.color != core.CLR_NONE):
				dark.exports.gdiplus.draw_round_vert(args.dc, rc, stuff.accent.color)
			ratio = self.get_ratio()
			rc.deflate(int(rc.width * ratio), int(rc.height * ratio))
			#dark.exports.gdiplus.draw_ellipse(args.dc, rc, stuff)
			dark.exports.gdiplus.draw_round_vert(args.dc, rc, stuff)
		return True

	#
	# 左矢印を描画します。
	#
	def draw_left(self, args, stuff_name, attrs):
		return self.draw_horz_arrow(args, stuff_name, attrs)

	#
	# 右矢印を描画します。
	#
	def draw_right(self, args, stuff_name, attrs):
		return self.draw_horz_arrow(args, stuff_name, attrs)

	#
	# 上矢印を描画します。
	#
	def draw_up(self, args, stuff_name, attrs):
		return self.draw_vert_arrow(args, stuff_name, attrs)

	#
	# 下矢印を描画します。
	#
	def draw_down(self, args, stuff_name, attrs):
		return self.draw_vert_arrow(args, stuff_name, attrs)

	#
	# 水平方向のつまみを描画します。
	#
	def draw_horz_thumb(self, args, stuff_name, attrs):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			ratio = self.get_ratio()
			rc.deflate(0, int(rc.height * ratio))
			dark.exports.gdiplus.draw_round_horz(args.dc, rc, stuff)
		return True

	#
	# 垂直方向のつまみを描画します。
	#
	def draw_vert_thumb(self, args, stuff_name, attrs):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			ratio = self.get_ratio()
			rc.deflate(int(rc.width * ratio), 0)
			dark.exports.gdiplus.draw_round_vert(args.dc, rc, stuff)
		return True

	#
	# 水平方向の下層トラックを描画します。
	#
	def draw_horz_upper_track(self, args, stuff_name, attrs):
		self.draw_background(args)
		return True

	#
	# 水平方向の上層トラックを描画します。
	#
	def draw_horz_upper_track(self, args, stuff_name, attrs):
		self.draw_background(args)
		return True

	#
	# 垂直方向の下層トラックを描画します。
	#
	def draw_vert_upper_track(self, args, stuff_name, attrs):
		self.draw_background(args)
		return True

	#
	# 垂直方向の上層トラックを描画します。
	#
	def draw_vert_upper_track(self, args, stuff_name, attrs):
		self.draw_background(args)
		return True

	#
	# サイズ変更グリップを描画します。
	#
	def draw_sizebox_right_align(self, args, stuff_name, attrs):
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (stuff.text.background_color != core.CLR_NONE):
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff.text.background_color)
			core.draw_icon(args, stuff, 'Marlett', '\u006F')
		return True
