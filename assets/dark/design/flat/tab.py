import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはタブコントロールを描画します。
#
class Tab(base.Tab):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Tab, self).__init__()

	#
	# アイテムを描画します。
	#
	def draw_item(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			if (not attrs.selected): rc.inflate(0, 2)
			rc.bottom -= 4
			if (hasattr(args, 'text')):
				dark.draw_str(args, rc, stuff)
			else:
				# フラットに見えるように描画します。
				dark.exports.painter.fill_rect(args.dc, rc, stuff)
				if (stuff_name != STUFF_NORMAL):
					dark.exports.painter.draw_edge(args.dc, rc, dark.exports.config_manager.edges.etched)
		return True
