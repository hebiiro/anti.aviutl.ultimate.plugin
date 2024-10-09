import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはポップアップメニューを描画します。
#
class MenuPopup(base.MenuPopup):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(MenuPopup, self).__init__()

	#
	# ポップアップメニューアイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (hasattr(args, 'text')):
			if (stuff_name):
				stuff = self.get_stuff(stuff_name)
				dark.draw_str(args, stuff)
		else:
			# gutterの部分を上書きしないように矩形を調整してから背景を描画します。
			rc = dark.RECT(args.rc)
			rc.top -= 1 # この調整がないとGDI+で描画した縁が残ってしまいます。
			rc.left += 28
			dark.exports.painter.fill_rect(args.dc, rc, self.get_stuff(STUFF_BACKGROUND))
			if (stuff_name):
				stuff = self.get_stuff(stuff_name)
				# 下線を描画します。
				draw_underline(args, stuff)
		return True
