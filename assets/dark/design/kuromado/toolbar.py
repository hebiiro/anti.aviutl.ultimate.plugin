import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはツールバーを描画します。
#
class ToolBar(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ToolBar, self).__init__(core.VSCLASS_TOOLBAR)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_TOOLBAR)
		self.add_theme(core.VSCLASS_EXPLORER_TOOLBAR)
		self.add_theme(core.VSCLASS_PLACEBAR_TOOLBAR)

		# 描画関数を追加します。
		self.add_handler(0, 0, self.draw_background, STUFF_BACKGROUND)
		self.add_handler(0, core.TS_NORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(0, core.TS_DISABLED, self.draw_item, STUFF_DISABLED)
		self.add_handler(0, core.TS_HOT, self.draw_item, STUFF_HOT)
		self.add_handler(0, core.TS_PRESSED, self.draw_item, STUFF_PRESSED)
		self.add_handler(0, core.TS_CHECKED, self.draw_item, STUFF_CHECKED)
		self.add_handler(0, core.TS_HOTCHECKED, self.draw_item, STUFF_CHECKED)
		"""
		self.add_handler(core.TP_BUTTON, 0, self.draw_background, STUFF_BACKGROUND)
		self.add_handler(core.TP_BUTTON, core.TS_NORMAL, self.draw_item, STUFF_NORMAL)
		self.add_handler(core.TP_BUTTON, core.TS_DISABLED, self.draw_item, STUFF_DISABLED)
		self.add_handler(core.TP_BUTTON, core.TS_HOT, self.draw_item, STUFF_HOT)
		self.add_handler(core.TP_BUTTON, core.TS_PRESSED, self.draw_item, STUFF_PRESSED)
		self.add_handler(core.TP_BUTTON, core.TS_CHECKED, self.draw_item, STUFF_CHECKED)
		self.add_handler(core.TP_BUTTON, core.TS_HOTCHECKED, self.draw_item, STUFF_CHECKED)
		"""

	#
	# 背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.gdiplus.draw_round_all(args.dc, args.rc, stuff)
		return True
