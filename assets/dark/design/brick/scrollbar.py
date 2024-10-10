import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはスクロールバーを描画します。
#
class ScrollBar(base.ScrollBar):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ScrollBar, self).__init__()

		# 描画関数を追加します。
		self.add_handler(core.SBP_ARROWBTN, core.ABS_LEFTNORMAL, self.draw_left, STUFF_NORMAL)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_RIGHTNORMAL, self.draw_right, STUFF_NORMAL)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_UPNORMAL, self.draw_up, STUFF_NORMAL)
		self.add_handler(core.SBP_ARROWBTN, core.ABS_DOWNNORMAL, self.draw_down, STUFF_NORMAL)

	#
	# 矢印を描画します。
	#
	def draw_arrow(self, args, stuff_name, attrs, arrow):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			# ブロック状に見えるように描画します。
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
			dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
			# 矩形が正方形になるように調整します。
			rc = dark.RECT(args.rc)
			size = min(rc.width, rc.height)
			rc.left = int((rc.left + rc.right - size) / 2)
			rc.top = int((rc.top + rc.bottom - size) / 2)
			rc.right = rc.left + size
			rc.bottom = rc.top + size
			rc.offset(0, -1)
			# 矢印マークを描画します。
			core.draw_icon(args, stuff, 'Meiryo', arrow, rc, font_weight=900)
		return True

	#
	# 左矢印を描画します。
	#
	def draw_left(self, args, stuff_name, attrs):
		return self.draw_arrow(args, stuff_name, attrs, '\uE012')

	#
	# 右矢印を描画します。
	#
	def draw_right(self, args, stuff_name, attrs):
		return self.draw_arrow(args, stuff_name, attrs, '\uE013')

	#
	# 上矢印を描画します。
	#
	def draw_up(self, args, stuff_name, attrs):
		return self.draw_arrow(args, stuff_name, attrs, '\uE014')

	#
	# 下矢印を描画します。
	#
	def draw_down(self, args, stuff_name, attrs):
		return self.draw_arrow(args, stuff_name, attrs, '\uE015')

	#
	# つまみにマークを描画します。
	#
	def draw_thumb_mark(self, args, stuff, icon):
		# 矩形が正方形になるように調整します。
		rc = dark.RECT(args.rc)
		size = min(rc.width, rc.height)
		rc.left = int((rc.left + rc.right - size) / 2)
		rc.top = int((rc.top + rc.bottom - size) / 2)
		rc.right = rc.left + size
		rc.bottom = rc.top + size
		# 矢印マークを描画します。
		rc.inflate(7, 0)
		core.draw_icon2(args, stuff, 'Meiryo', icon, rc)

	#
	# 水平方向のつまみを描画します。
	#
	def draw_horz_thumb(self, args, stuff_name, attrs):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			# ブロック状に見えるように描画します。
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
			dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
			self.draw_thumb_mark(args, stuff, '⚡')
		return True

	#
	# 垂直方向のつまみを描画します。
	#
	def draw_vert_thumb(self, args, stuff_name, attrs):
		self.draw_background(args)
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			# ブロック状に見えるように描画します。
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
			dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
			self.draw_thumb_mark(args, stuff, '⚡')
		return True
