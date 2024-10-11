import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはメニューバーを描画します。
#
class MenuBar(base.MenuBar):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(MenuBar, self).__init__()

	#
	# メニューバーの背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.bottom += 1
			# 模様を描画します。
			dark.exports.painter.fill_rect(args.dc, rc, stuff)
			draw_pattern(args, stuff)
		return True

	#
	# メニューバーアイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				# 下線を描画します。
				draw_underline(args, stuff)
		return True
