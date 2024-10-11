import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *
from .window import *

#
# このクラスはウィンドウフェイスを描画します。
#
class WindowFace(Window):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(WindowFace, self).__init__("FACE")

		# 描画関数を追加します。
		self.add_handler(core.WP_WINDOW_FACE, 0, self.draw_window_face, STUFF_BACKGROUND)
		self.add_handler(core.WP_BORDER, 0, self.draw_border, STUFF_BACKGROUND)
		self.add_handler(core.WP_WINDOW_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.raised)
		self.add_handler(core.WP_CLIENT_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.sunken)
		self.add_handler(core.WP_STATIC_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.light_sunken)
		self.add_handler(core.WP_LIGHT_RAISED_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.light_raised)
		self.add_handler(core.WP_LIGHT_SUNKEN_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.light_sunken)
		self.add_handler(core.WP_RAISED_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.raised)
		self.add_handler(core.WP_SUNKEN_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.sunken)
		self.add_handler(core.WP_BUMP_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.bump)
		self.add_handler(core.WP_ETCHED_EDGE, 0, self.draw_edge, None, edge=dark.exports.config_manager.edges.etched)

	#
	# ウィンドウフェイスを描画します。
	#
	def draw_window_face(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True

	#
	# ボーダーを描画します。
	#
	def draw_border(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.frame_rect(args.dc, args.rc, stuff)
		return True

	#
	# エッジを描画します。
	#
	def draw_edge(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		dark.exports.painter.draw_edge(args.dc, args.rc, attrs.edge)
		return True
