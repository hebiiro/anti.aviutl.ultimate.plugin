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
# このクラスはウィンドウセパレータを描画します。
#
class WindowSeparator(Window):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(WindowSeparator, self).__init__("SEPARATOR")

		# 描画関数を追加します。
		self.add_handler(core.WP_SEPARATOR, core.CS_ACTIVE, self.draw_separator, STUFF_ACTIVE)
		self.add_handler(core.WP_SEPARATOR, core.CS_INACTIVE, self.draw_separator, STUFF_INACTIVE)
		self.add_handler(core.WP_SEPARATOR, core.CS_DISABLED, self.draw_separator, STUFF_DISABLED)

	#
	# セパレータを描画します。
	#
	def draw_separator(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
		return True
