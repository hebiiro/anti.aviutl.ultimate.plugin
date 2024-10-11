import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスはスピンボタンを描画します。
#
class Spin(base.Spin):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(Spin, self).__init__()

	#
	# スピンボタンを描画します。
	#
	def draw_spin_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			rc = dark.RECT(args.rc)
			# フラットに見えるように描画します。
			dark.exports.painter.fill_rect(args.dc, rc, self.get_stuff(STUFF_BACKGROUND))
			if (attrs.pressed):
				dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.sunken)
				rc.offset(1, 1)
			elif (stuff_name == STUFF_HOT):
				dark.exports.painter.draw_edge(args.dc, args.rc, dark.exports.config_manager.edges.etched)
			self.draw_icon(args, stuff, attrs, rc)
		return True
