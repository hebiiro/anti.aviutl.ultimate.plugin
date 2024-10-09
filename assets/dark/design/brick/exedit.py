import sys
import win32gui
import win32api
import win32ui
import win32con
import dark
import core
from .include import *

#
# このクラスは拡張編集を描画します。
#
class ExEdit(base.ExEdit):
	#
	# コンストラクタです。
	#
	def __init__(self):
		# スーパークラスのコンストラクタを呼び出します。
		super(ExEdit, self).__init__()

	#
	# レイヤーボタンを描画します。
	#
	def draw_layer_button(self, args, stuff_name, attrs):
		core.debug_print(f'{__name__}.{self.__class__.__name__}.{sys._getframe().f_code.co_name}({stuff_name}, {dark.str(args)})')
		if (stuff_name):
			stuff = self.get_stuff(stuff_name)
			if (hasattr(args, 'text')):
				if (hasattr(args, 'options')):
					if (args.options & win32con.ETO_OPAQUE):
						args.options &= ~win32con.ETO_OPAQUE
				rc = dark.RECT(args.rc)
				rc.inflate(1, 1)
				dark.exports.painter.fill_rect(args.dc, rc, self.get_stuff(STUFF_EXEDIT_DUMMY))
				rc.deflate(0, 1)
				rc.left += 1
				# ブロック状に見えるように描画します。
				dark.exports.painter.fill_rect(args.dc, rc, stuff)
				dark.exports.painter.draw_edge(args.dc, rc, dark.exports.config_manager.edges.etched)
				dark.draw_str(args, stuff)
		return True
