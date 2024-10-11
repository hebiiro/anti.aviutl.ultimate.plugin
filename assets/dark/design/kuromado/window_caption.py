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
# このクラスはウィンドウキャプションを描画します。
#
class WindowCaption(Window):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(WindowCaption, self).__init__("CAPTION")

		# 描画関数を追加します。
		self.add_handler(core.WP_CAPTION, core.CS_ACTIVE, self.draw_caption, STUFF_ACTIVE)
		self.add_handler(core.WP_CAPTION, core.CS_INACTIVE, self.draw_caption, STUFF_INACTIVE)
		self.add_handler(core.WP_CAPTION, core.CS_DISABLED, self.draw_caption, STUFF_DISABLED)

	#
	# キャプションを描画します。
	#
	def draw_caption(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				dark.draw_str(args, stuff)
			else:
				dark.exports.gdiplus.draw_round_all(args.dc, args.rc, stuff)
				#dark.exports.painter.draw_rect(args.dc, args.rc, stuff)
		return True
