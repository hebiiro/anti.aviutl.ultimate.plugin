import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *
from .function import *
from .menu import *

#
# このクラスはポップアップメニューを描画します。
#
class MenuPopup(Menu):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(MenuPopup, self).__init__("POPUP")

		# 描画関数を追加します。
		self.add_handler(core.MENU_POPUPBACKGROUND, 0, self.draw_background, STUFF_BACKGROUND)
		self.add_handler(core.MENU_POPUPBORDERS, 0, self.draw_borders, STUFF_BACKGROUND)
		self.add_handler(core.MENU_POPUPGUTTER, 0, self.draw_gutter, STUFF_GUTTER)
		self.add_handler(core.MENU_POPUPSEPARATOR, 0, self.draw_separator, STUFF_SEPARATOR)
		self.add_handler(core.MENU_POPUPITEM, core.MPI_NORMAL, self.draw_item, None, STUFF_NORMAL)
		self.add_handler(core.MENU_POPUPITEM, core.MPI_DISABLED, self.draw_item, None, STUFF_DISABLED)
		self.add_handler(core.MENU_POPUPITEM, core.MPI_HOT, self.draw_item, STUFF_HOT, STUFF_HOT)
		self.add_handler(core.MENU_POPUPITEM, core.MPI_DISABLEDHOT, self.draw_item, None, STUFF_DISABLED)
		self.add_handler(27, core.MPI_NORMAL, self.draw_item, None, STUFF_NORMAL)
		self.add_handler(27, core.MPI_DISABLED, self.draw_item, None, STUFF_DISABLED)
		self.add_handler(27, core.MPI_HOT, self.draw_item, STUFF_HOT, STUFF_HOT)
		self.add_handler(27, core.MPI_DISABLEDHOT, self.draw_item, None, STUFF_DISABLED)

	#
	# ポップアップメニューの背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True

	#
	# ポップアップメニューのボーダーを描画します。
	#
	def draw_borders(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.draw_round_rect(args.dc, args.rc, stuff)
		return True

	#
	# ポップアップメニューの背景左側を描画します。
	#
	def draw_gutter(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.top -= 1 # この調整がないとGDI+で描画した縁が残ってしまいます。
			dark.exports.painter.fill_rect(args.dc, rc, stuff)
		return True

	#
	# ポップアップメニューのセパレータを描画します。
	#
	def draw_separator(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.deflate(0, 2)
			dark.exports.painter.fill_rect(args.dc, rc, stuff)
		return True

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
				draw_underline(args, stuff)
		return True
