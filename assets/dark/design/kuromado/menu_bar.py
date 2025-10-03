import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *
from .menu import *

#
# このクラスはメニューバーを描画します。
#
class MenuBar(Menu):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(MenuBar, self).__init__("BAR")

		# 描画関数を追加します。
		self.add_handler(core.MENU_BARBACKGROUND, core.MB_ACTIVE, self.draw_background, STUFF_ACTIVE)
		self.add_handler(core.MENU_BARBACKGROUND, core.MB_INACTIVE, self.draw_background, STUFF_INACTIVE)
		self.add_handler(core.MENU_BARITEM, core.MBI_NORMAL, self.draw_item, None, STUFF_NORMAL)
		self.add_handler(core.MENU_BARITEM, core.MBI_DISABLED, self.draw_item, None, STUFF_DISABLED)
		self.add_handler(core.MENU_BARITEM, core.MBI_HOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.MENU_BARITEM, core.MBI_DISABLEDHOT, self.draw_item, None, STUFF_DISABLED)
		self.add_handler(core.MENU_BARITEM, core.MBI_PUSHED, self.draw_item, STUFF_PUSHED)
		self.add_handler(core.MENU_BARITEM, core.MBI_DISABLEDPUSHED, self.draw_item, None, STUFF_DISABLED)

	#
	# メニューバーの背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.bottom += 1
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, rc, stuff)
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
				dark.exports.gdiplus.draw_round_all(args.dc, args.rc, stuff)
				if (stuff.accent.color != core.CLR_NONE):
					rc = dark.RECT(args.rc)
					rc.top = rc.bottom - 2
					dark.exports.painter.fill_rect(args.dc, rc, stuff.accent_color)
		return True
