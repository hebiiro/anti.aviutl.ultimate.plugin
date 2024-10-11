import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはコマンドモジュールを描画します。
#
class CommandModule(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(CommandModule, self).__init__(core.VSCLASS_COMMANDMODULE)

		# テーマを追加します。
		self.add_theme(core.VSCLASS_COMMANDMODULE, True)

		# 描画関数を追加します。
		self.add_handler(1, 0, self.draw_background, STUFF_BACKGROUND)
		self.add_handler(3, 1, self.draw_item, STUFF_NORMAL)
		self.add_handler(3, 2, self.draw_item, STUFF_HOT)
		self.add_handler(3, 3, self.draw_item, STUFF_PRESSED)
		self.add_handler(3, 4, self.draw_item, STUFF_CHECKED)
		self.add_handler(6, 0, self.draw_drop_down_button, STUFF_NORMAL)

	#
	# 背景を描画します。
	#
	def draw_background(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True

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
				dark.exports.gdiplus.draw_round_all(args.dc, args.rc, stuff)
		return True

	#
	# ドロップダウンボタンを描画します。
	#
	def draw_drop_down_button(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			#width = rc.width
			#rc.top = int((rc.top + rc.bottom - width) / 2)
			#rc.bottom = rc.top + width
			return core.draw_icon(args, stuff, 'Webdings', '\u0036', rc)

