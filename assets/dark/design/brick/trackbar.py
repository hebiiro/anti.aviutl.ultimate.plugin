import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはトラックバーを描画します。
#
class TrackBar(base.TrackBar):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(TrackBar, self).__init__()

	#
	# 水平方向のトラックを描画します。
	#
	def draw_horz_track(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.deflate(0, 1)
			# ブロック状に見えるように描画します。
			dark.exports.painter.draw_edge(args.dc, rc, dark.exports.config_manager.edges.light_sunken)
		return True

	#
	# 水平方向のつまみを描画します。
	#
	def draw_horz_thumb(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		#if (stuff_name): dark.exports.painter.draw_round_rect(args.dc, args.rc, self.get_stuff(stuff_name))
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			# ブロック状に見えるように描画します。
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
			dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
		return True

	#
	# 垂直方向のトラックを描画します。
	#
	def draw_vert_track(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			# ブロック状に見えるように描画します。
			dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.light_sunken)
		return True

	#
	# 垂直方向のつまみを描画します。
	#
	def draw_vert_thumb(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		#if (stuff_name): dark.exports.painter.draw_round_rect(args.dc, args.rc, self.get_stuff(stuff_name))
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			# ブロック状に見えるように描画します。
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
			dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.raised)
		return True
