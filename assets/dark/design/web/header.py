import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはヘッダーコントロールを描画します。
#
class Header(base.Header):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Header, self).__init__()

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
				if (stuff_name == STUFF_HOT or stuff_name == STUFF_PRESSED):
					# 下線を描画します。
					draw_underline(args, stuff)
				else:
					# 模様を描画します。
					draw_pattern(args, stuff)
		return True
