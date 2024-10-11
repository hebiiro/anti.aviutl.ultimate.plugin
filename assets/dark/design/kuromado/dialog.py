import sys
import ctypes
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .const import *

#
# このクラスはダイアログを描画します。
#
class Dialog(core.Control):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Dialog, self).__init__(core.VSCLASS_WINDOW + "_DIALOG")

		# テーマを追加します。
		self.add_theme(core.VSCLASS_WINDOW)
		self.add_theme(core.VSCLASS_EXPLORER_WINDOW)

		# 描画関数を追加します。
		self.add_handler(core.WP_DIALOG, 0, self.draw_dialog, STUFF_NORMAL)
		self.add_handler(core.WP_DIALOG, core.ETS_NORMAL, self.draw_dialog, STUFF_NORMAL)
		self.add_handler(core.WP_DIALOG, core.ETS_DISABLED, self.draw_dialog, STUFF_DISABLED)

	#
	# ダイアログを描画します。
	#
	def draw_dialog(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True
