import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはボタンを描画します。
#
class Button(base.Button):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Button, self).__init__()

	#
	# プッシュボタンを描画します。
	#
	def draw_push_button(self, args, stuff_name, attrs):
		if (core.debug): print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			rc.inflate(1, 1)
			if (hasattr(args, 'text')):
				if (attrs.pressed): rc.offset(1, 1)
				dark.draw_str(args, rc, stuff)
			else:
				# フラットに見えるように描画します。
				dark.exports.painter.fill_rect(args.dc, args.rc, stuff)
				if (stuff_name == STUFF_HOT):
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.etched)
				elif (attrs.pressed):
					dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.sunken)
		return True
